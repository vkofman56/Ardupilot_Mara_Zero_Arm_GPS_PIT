# MARA Zero Arm - Project Notes

**Last Updated:** April 21, 2026
**Status:** ✅ Active — Goku H743 Pro Mini firmware built and hardware-tested
**Default Build Board:** Goku H743 Pro Mini (FlywooH743Pro)

---

## Project Overview

Custom ArduPilot 4.7.0-dev firmware for FPV flight controller boards.

- **Full source (local only):** `~/code/MARA/MARA_Zero_Arm/ardupilot/`
- **Custom files backup (GitHub):** https://github.com/vkofman56/Ardupilot_Mara_Zero_Arm_GPS_PIT

The GitHub repo contains only the modified files (not the full ArduPilot source).

---

## Custom Modifications

All changes are relative to upstream ArduPilot 4.7.0-dev.

| Modification | File(s) | Notes |
|---|---|---|
| Zero arming delay | `ArduCopter/config.h` | `ARMING_DELAY_SEC 0.0f` |
| Skip lean angle check | `ArduCopter/AP_Arming_Copter.cpp`, `libraries/AP_Arming/AP_Arming.cpp/h` | Via `ARMING_OPTIONS` bit 3 |
| HDZero pit mode | `libraries/AP_MSP/AP_MSP_Telem_DisplayPort.cpp/h`, `AP_MSP_Telem_Backend.cpp/h`, `libraries/AP_OSD/AP_OSD_MSP_DisplayPort.cpp` | FC identifies as BTFL; re-pushes VTX config on HDZero boot (FC_VARIANT query trigger) |
| BOOTTIME OSD element | `libraries/AP_OSD/AP_OSD.h`, `AP_OSD_Screen.cpp` | System uptime HH:MM:SS.mmm, right-aligned |
| GPSTIME OSD element | `libraries/AP_OSD/AP_OSD.h`, `AP_OSD_Screen.cpp` | GPS UTC time HH:MM:SS.mmm |
| OSD 15Hz refresh | `libraries/AP_OSD/AP_OSD.cpp` | 67ms delay (was 100ms) |
| Extended OSD position range | `libraries/AP_OSD/AP_OSD_Setting.cpp` | X: -10–70, Y: -10–30 |
| MAVLink attitude timestamp filter | `ArduCopter/GCS_MAVLink_Copter.cpp`, `mode_guided.cpp`, `Log.cpp`, `Copter.h`, `mode.h` | Drops out-of-order SET_ATTITUDE_TARGET messages |
| MSP VTX type enum | `libraries/AP_VideoTX/AP_VideoTX.h` | Added `MSP = 1U<<3` to VTXType enum |

---

## VTX / Pit Mode Parameters

| Parameter | Value | Notes |
|---|---|---|
| `VTX_ENABLE` | 1 (Enable) | Must be enabled for pit mode |
| `VTX_OPTIONS` | 7 | Bit 0: always pit, Bit 1: exit on arm, Bit 2: re-enter on disarm |
| `VTX_POWER` | 25 | mW — used when not in pit mode |
| `OSD_TYPE` | 5 (MSP DisplayPort) | Required for HDZero |
| `SERIALx_PROTOCOL` | 42 (DisplayPort) | UART connected to HDZero |

**Pit mode flow:** Relay on → HDZero boots → queries FC_VARIANT → FC detects query → sends 10-burst MSP_VTX_CONFIG with pitmode=1 → HDZero enters pit mode. Armed → vtx.update() clears pitmode bit → FC sends pitmode=0 → VTX full power. Disarmed → vtx.update() sets pitmode bit → FC sends pitmode=1 → pit mode again.

---

## Arming Without RC

Set `ARMING_SKIPCHK = 1` to bypass arming checks (allows GCS/MAVLink arming without RC).

---

## Supported Boards

| Board | waf name | Status |
|---|---|---|
| Goku H743 Pro Mini | `FlywooH743Pro` | ✅ Built & hardware-tested Apr 21, 2026 |
| Kakute H7 Mini v1.5 | `KakuteH7Mini` | ⏳ Needs rebuild |
| Kakute H7 Mini v1.3 (NAND) | `KakuteH7Mini-Nand` | ⏳ Needs rebuild |
| TMotor H7 Mini | `TMotorH743` | ⏳ Needs rebuild |
| ARK FPV | `ARK_FPV` | ⏳ Needs rebuild |
| TBS LUCID H7 | `TBS_LUCID_H7` | ⏳ Needs rebuild |

---

## Build Environment

**Machine:** Mac Apple Silicon, macOS 26

**Toolchain:** ARM GNU Toolchain 13.3 — `~/arm-gnu-toolchain-13.3/`
- Homebrew `arm-none-eabi-gcc` 15.x does NOT work on macOS 26 (missing newlib)
- Download: developer.arm.com → macOS Apple Silicon, arm-none-eabi, 13.3.Rel1

**Build Commands:**
```bash
cd ~/code/MARA/MARA_Zero_Arm/ardupilot

# First time / new board:
PATH=~/arm-gnu-toolchain-13.3/bin:$PATH ./waf configure --board FlywooH743Pro
PATH=~/arm-gnu-toolchain-13.3/bin:$PATH ./waf copter

# Subsequent builds (same board):
PATH=~/arm-gnu-toolchain-13.3/bin:$PATH ./waf copter
```

**Output:** `ardupilot/build/<BOARD>/bin/arducopter.apj`

---

## Flashing Firmware

**Mission Planner (recommended):** Initial Setup → Install Firmware → Load custom firmware → select `.apj`

**QGroundControl:** Vehicle Setup → Firmware → Advanced → Custom firmware → select `.apj`

**DFU recovery:** Hold bootloader button, connect USB, release after 2s → `dfu-util -d 0483:df11 -a 0 -s 0x08000000 -D arducopter.bin`

---

## Session History

### April 21, 2026
- Fixed FC boot failure: OSD param table overflow (GPSTIME/BOOTTIME indices > 63) — moved to var_info2
- Fixed HDZero pit mode: FC now re-pushes VTX config when HDZero boots (FC_VARIANT query trigger)
- Confirmed VTX_OPTIONS=7 arm/disarm pit mode cycling works via AP_VideoTX::update() scheduler task
- Confirmed on hardware: pit mode ON when relay on, full power when armed, pit mode ON when disarmed ✅
- Consolidated project to single folder MARA_Zero_Arm, deleted old duplicate repos
- Toolchain: switched from Homebrew gcc 15.x to ARM GNU Toolchain 13.3
