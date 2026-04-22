---
name: MARA ArduPilot project context
description: Key facts about the MARA ArduPilot custom firmware project
type: project
originSessionId: 2465e706-7786-4f72-95d0-5b0b54b24be9
---
Custom ArduPilot 4.7.0-dev firmware for FPV flight controllers. All work lives in `MARA_Zero_Arm/` folder.

**Project path:** `/Users/victorkofman/code/MARA/MARA_Zero_Arm/` (previously inside `Ardupilot_MARA_2/` which is pending deletion)

**Why:** `Ardupilot_MARA_2` was the old parent folder — consolidated everything into `MARA_Zero_Arm` on 2026-04-21.

**Single build source:** `MARA_Zero_Arm/ardupilot/` — the only ArduPilot repo to use.

**Toolchain:** Homebrew `arm-none-eabi-gcc` 15.x is broken (missing newlib on macOS 26). Use ARM GNU Toolchain 13.3 at `~/arm-gnu-toolchain-13.3/` (downloaded from developer.arm.com).

**Build command:**
```
cd MARA_Zero_Arm/ardupilot
PATH=~/arm-gnu-toolchain-13.3/bin:$PATH ./waf configure --board FlywooH743Pro
PATH=~/arm-gnu-toolchain-13.3/bin:$PATH ./waf copter
```

**Default board:** Goku H743 Pro Mini (`FlywooH743Pro`) — built and tested 2026-04-21.

**Other boards** (all need rebuilds — major changes since last build): KakuteH7Mini, KakuteH7Mini-Nand, TMotorH743, ARK_FPV, TBS_LUCID_H7.

**Custom changes in ardupilot/:**
- Zero arming delay (`config.h`)
- Skip lean angle check (`ARMING_OPTIONS` bit 3)
- HDZero pit mode fix (VTX config re-pushed on HDZero boot)
- BOOTTIME + GPSTIME OSD elements
- OSD 15Hz refresh, extended position ranges
- MAVLink attitude timestamp filtering
- MSP VTX type added

**GitHub (single source of truth):** https://github.com/vkofman56/Ardupilot_Mara_Zero_Arm_GPS_PIT
- Contains only custom-modified files + PROJECT_NOTES.md + README.md + docs/memory/
- Full ArduPilot source stays local only (too large for GitHub)

**Other GitHub repos** (`vkofman56/MARA_Zero_Arm`, `vkofman56/ardupilot`): pending archive/delete — nothing there not already in GPS_PIT or local.

**Cleanup done:** `Ardupilot_MARA_2` deleted. GPS_PIT is now the one remote repo.
