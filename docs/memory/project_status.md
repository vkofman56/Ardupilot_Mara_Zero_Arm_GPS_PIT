---
name: MARA Zero Arm project status
description: Current state of custom ArduPilot firmware project — what's done, what's pending
type: project
originSessionId: e38aed35-0d20-4cb7-994d-dec5de15dbea
---
Custom ArduPilot 4.7.0-dev firmware for FPV flight controllers. All source lives in `MARA_Zero_Arm/ardupilot/` (submodule). Full notes in `PROJECT_NOTES.md`.

**Why:** FPV racing/freestyle — need zero arming delay, HDZero pit mode control, and custom OSD elements.

**How to apply:** Check `PROJECT_NOTES.md` for the authoritative board/patch table before suggesting changes.

## Patches committed

| Modification | Files |
|---|---|
| Zero arming delay | `ArduCopter/config.h` |
| Skip lean angle check | `AP_Arming_Copter.cpp`, `AP_Arming.cpp/h` |
| HDZero pit mode fix | `AP_MSP_Telem_DisplayPort.cpp/h`, `AP_MSP_Telem_Backend.cpp/h` |
| BOOTTIME OSD element | `AP_OSD.h`, `AP_OSD_Screen.cpp` |
| GPSTIME OSD element | `AP_OSD.h`, `AP_OSD_Screen.cpp` |
| OSD 15Hz refresh | `AP_OSD.cpp` |
| Extended OSD position range | `AP_OSD_Setting.cpp` |
| MAVLink attitude timestamp filter | `GCS_MAVLink_Copter.cpp`, `mode_guided.cpp`, `Log.cpp`, `Copter.h`, `mode.h` |
| MSP VTX type enum | `AP_VideoTX.h` |

## Board build status (as of 2026-04-21)

| Board | waf name | Status |
|---|---|---|
| Goku H743 Pro Mini | `FlywooH743Pro` | ✅ Built & hardware-tested |
| Kakute H7 Mini v1.5 | `KakuteH7Mini` | ⏳ Needs rebuild |
| Kakute H7 Mini v1.3 (NAND) | `KakuteH7Mini-Nand` | ⏳ Needs rebuild |
| TMotor H7 Mini | `TMotorH743` | ⏳ Needs rebuild |
| ARK FPV | `ARK_FPV` | ⏳ Needs rebuild |
| TBS LUCID H7 | `TBS_LUCID_H7` | ⏳ Needs rebuild |

## Open tasks

- No active tasks. Board rebuilds are not a current priority.

## Cleanup done

- Deleted `Ardupilot_MARA_2` folder ✅
- Deleted old duplicate repos (`Ardupilot_MARA`, `ardupilot-4.7.0-dev`, zip backup) ✅
