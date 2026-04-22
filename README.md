# MARA Zero Arm - Custom ArduPilot Firmware

Custom ArduPilot 4.7.0-dev firmware for FPV flight controllers with instant arming, HDZero pit mode control, and GPS/boot time OSD elements.

## Default Board: Goku H743 Pro Mini (FlywooH743Pro)

## Build

Requires ARM GNU Toolchain 13.3 at `~/arm-gnu-toolchain-13.3/` (Homebrew gcc 15.x does not work on macOS 26).

```bash
cd ardupilot

# Configure (first time or when changing board)
PATH=~/arm-gnu-toolchain-13.3/bin:$PATH ./waf configure --board FlywooH743Pro

# Build
PATH=~/arm-gnu-toolchain-13.3/bin:$PATH ./waf copter

# Output
build/FlywooH743Pro/bin/arducopter.apj
```

## Custom Modifications

- Zero arming delay (instant arm)
- Skip lean angle check via `ARMING_OPTIONS` bit 3
- HDZero pit mode fix (VTX config re-pushed on every HDZero boot)
- BOOTTIME OSD element (system uptime HH:MM:SS.mmm)
- GPSTIME OSD element (GPS UTC time HH:MM:SS.mmm)
- OSD 15Hz refresh rate
- Extended OSD position ranges
- MAVLink attitude timestamp filtering

## Supported Boards

| Board | `--board` name |
|---|---|
| Goku H743 Pro Mini | `FlywooH743Pro` |
| Kakute H7 Mini v1.5 | `KakuteH7Mini` |
| Kakute H7 Mini v1.3 | `KakuteH7Mini-Nand` |
| TMotor H7 Mini | `TMotorH743` |
| ARK FPV | `ARK_FPV` |
| TBS LUCID H7 | `TBS_LUCID_H7` |

## GitHub Backup of Custom Files

https://github.com/vkofman56/Ardupilot_Mara_Zero_Arm_GPS_PIT

See `PROJECT_NOTES.md` for full details.
