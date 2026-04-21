/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <AP_Common/AP_FWVersion.h>
#include <AP_Vehicle/AP_Vehicle_Type.h>

#include "AP_MSP.h"
#include "AP_MSP_Telem_DisplayPort.h"

#if AP_VIDEOTX_ENABLED
#include <AP_VideoTX/AP_VideoTX.h>
#include <GCS_MAVLink/GCS.h>
#endif

#if HAL_WITH_MSP_DISPLAYPORT

extern const AP_HAL::HAL& hal;

using namespace MSP;

MSPCommandResult AP_MSP_Telem_DisplayPort::msp_process_out_fc_variant(sbuf_t *dst)
{
    const AP_MSP *msp = AP::msp();
    if (msp == nullptr) {
        return MSP_RESULT_ERROR;
    }

    bool use_btfl = (msp->is_option_enabled(AP_MSP::Option::DISPLAYPORT_BTFL_SYMBOLS)
                     && !msp->is_option_enabled(AP_MSP::Option::DISPLAYPORT_INAV_SYMBOLS));

#if AP_VIDEOTX_ENABLED
    // HDZero VTX firmware only accepts VTX config from "BTFL"/"EMUF"/"QUIC"/"INAV".
    // Identify as BTFL when VTX control is active so the VTX processes our config.
    if (AP::vtx().get_enabled()) {
        use_btfl = true;
        // HDZero sends MSP_FC_VARIANT on every boot as part of its handshake.
        // Use this as a trigger to restart the VTX config push cycle so pit mode
        // is correctly applied even when the VTX powered on after FC boot.
        _vtx_send_count = 0;
        GCS_SEND_TEXT(MAV_SEVERITY_INFO, "MSP VTX: HDZero boot detected, re-pushing config");
    }
#endif

    if (use_btfl) {
        sbuf_write_data(dst, BETAFLIGHT_IDENTIFIER, FLIGHT_CONTROLLER_IDENTIFIER_LENGTH);
    } else {
        sbuf_write_data(dst, ARDUPILOT_IDENTIFIER, FLIGHT_CONTROLLER_IDENTIFIER_LENGTH);
    }

    return MSP_RESULT_ACK;
}

bool AP_MSP_Telem_DisplayPort::init_uart()
{
    if (_msp_port.uart != nullptr)  {
        // re-init port here for use in this thread
        // displayport needs a bigger tx buffer
        // rx buffer can be small for we mostly write
        _msp_port.uart->begin(0, 256, 768);
        return true;
    }
    return false;
}

#if AP_VIDEOTX_ENABLED
/*
  Send VTX config to HDZero (or other MSP VTX) over DisplayPort UART.
  Follows Betaflight's vtx_msp.c approach: sends MSP_VTX_CONFIG (cmd 88)
  response frames with the full 15-byte payload.
*/
void AP_MSP_Telem_DisplayPort::update_vtx_params()
{
    AP_VideoTX &vtx = AP::vtx();

    if (!vtx.get_enabled()) {
        return;
    }

    if (!_vtx_initialized) {
        vtx.set_defaults();
        _vtx_initialized = true;
        _vtx_send_count = 0;
        GCS_SEND_TEXT(MAV_SEVERITY_INFO, "MSP VTX: init, starting config push");
    }

    // Only check for new param changes after we've finished a send cycle.
    // During a send cycle (_vtx_send_count < VTX_SEND_REPEATS) we keep sending
    // without re-checking, to avoid resetting the counter each call.
    if (_vtx_send_count >= VTX_SEND_REPEATS) {
        if (vtx.have_params_changed()) {
            _vtx_send_count = 0;
            GCS_SEND_TEXT(MAV_SEVERITY_INFO, "MSP VTX: params changed, re-sending");
        } else {
            return;
        }
    }

    // rate-limit to 200ms
    const uint32_t now_ms = AP_HAL::millis();
    if (now_ms - _vtx_last_send_ms < VTX_SEND_INTERVAL_MS) {
        return;
    }
    _vtx_last_send_ms = now_ms;

    // reconcile band/channel with frequency
    uint8_t band = vtx.get_configured_band();
    uint8_t channel = vtx.get_configured_channel();
    uint16_t frequency = AP_VideoTX::get_frequency_mhz(band, channel);

    // if the user set a frequency directly, reverse-map to band/channel
    const uint16_t configured_freq = vtx.get_configured_frequency_mhz();
    if (configured_freq > 0 && configured_freq != frequency) {
        AP_VideoTX::VideoBand vband;
        uint8_t vchan;
        if (AP_VideoTX::get_band_and_channel(configured_freq, vband, vchan)) {
            band = vband;
            channel = vchan;
            frequency = configured_freq;
        }
    }

    const bool pitmode = vtx.get_configured_pitmode();

    // Map milliwatt power to MSP power index (1-indexed).
    // HDZero power table: 1=25mW, 2=200mW, 3=500mW, 4=MAX
    // Note: get_configured_power_level() returns SmartAudio levels which
    // don't map to MSP power indices.
    const uint16_t power_mw = vtx.get_configured_power_mw();
    uint8_t msp_power;
    if (power_mw <= 25) {
        msp_power = 1;
    } else if (power_mw <= 200) {
        msp_power = 2;
    } else if (power_mw <= 500) {
        msp_power = 3;
    } else {
        msp_power = 4;
    }

    // build 15-byte MSP_VTX_CONFIG payload (Betaflight format, 1-indexed)
    const uint8_t payload[15] = {
        4,                                      // VTX device type: VTXDEV_MSP
        uint8_t(band + 1),                      // band (1-indexed)
        uint8_t(channel + 1),                   // channel (1-indexed)
        msp_power,                              // power index (1-indexed)
        uint8_t(pitmode ? 1 : 0),               // pit mode
        uint8_t(frequency & 0xFF),              // frequency low byte
        uint8_t((frequency >> 8) & 0xFF),       // frequency high byte
        1,                                      // device ready
        0,                                      // low power disarm
        0,                                      // pit mode frequency low byte
        0,                                      // pit mode frequency high byte
        0,                                      // VTX table available
        0,                                      // VTX table band count
        0,                                      // VTX table channel count
        0,                                      // VTX table power level count
    };

    // send as MSP_VTX_CONFIG response (cmd 88) on the DisplayPort UART
    // use V2 native framing ($X>) — matches Betaflight's vtx_msp.c approach
    msp_send_packet(MSP_VTX_CONFIG, MSP::MSP_V2_NATIVE, payload, sizeof(payload), false);

    _vtx_send_count++;

    if (_vtx_send_count == 1 || _vtx_send_count == VTX_SEND_REPEATS) {
        GCS_SEND_TEXT(MAV_SEVERITY_INFO, "MSP VTX: send %u/%u B%u/CH%u %uMHz P%u(%umW)",
                      _vtx_send_count, VTX_SEND_REPEATS,
                      band + 1, channel + 1, frequency, msp_power, power_mw);
    }

    // mark params as current after all repeated sends
    if (_vtx_send_count >= VTX_SEND_REPEATS) {
        vtx.set_freq_is_current();
        vtx.set_power_is_current();
        vtx.set_options_are_current();
        vtx.set_configuration_finished(true);
        vtx.set_provider_enabled(AP_VideoTX::VTXType::MSP);
        vtx.announce_vtx_settings();
        GCS_SEND_TEXT(MAV_SEVERITY_INFO, "MSP VTX: config push complete");
    }
}
#endif  // AP_VIDEOTX_ENABLED

#endif //HAL_WITH_MSP_DISPLAYPORT
