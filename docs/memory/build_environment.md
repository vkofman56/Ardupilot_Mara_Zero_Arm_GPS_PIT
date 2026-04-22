---
name: Build environment and toolchain
description: How to build ArduPilot firmware on this machine — toolchain, commands, output paths
type: project
originSessionId: e38aed35-0d20-4cb7-994d-dec5de15dbea
---
**Machine:** Mac Apple Silicon, macOS 26

**Toolchain:** ARM GNU Toolchain 13.3 at `~/arm-gnu-toolchain-13.3/`

**Why:** Homebrew `arm-none-eabi-gcc` 15.x does NOT work on macOS 26 — missing newlib headers. Must use the ARM official 13.3.Rel1 download (macOS Apple Silicon, arm-none-eabi).

**How to apply:** Always prepend `PATH=~/arm-gnu-toolchain-13.3/bin:$PATH` to every waf build command.

## Build commands

```bash
cd ardupilot

# Configure + build (first time or new board):
PATH=~/arm-gnu-toolchain-13.3/bin:$PATH ./waf configure --board FlywooH743Pro
PATH=~/arm-gnu-toolchain-13.3/bin:$PATH ./waf copter

# Rebuild same board:
PATH=~/arm-gnu-toolchain-13.3/bin:$PATH ./waf copter

# Switch board:
PATH=~/arm-gnu-toolchain-13.3/bin:$PATH ./waf configure --board KakuteH7Mini
PATH=~/arm-gnu-toolchain-13.3/bin:$PATH ./waf copter
```

**Output:** `ardupilot/build/<BOARD>/bin/arducopter.apj`

## GitHub backup

Custom files (not full history) backed up at: https://github.com/vkofman56/Ardupilot_Mara_Zero_Arm_GPS_PIT
