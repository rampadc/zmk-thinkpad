---
sidebar_position: 1
slug: /
title: Documentation
description: Current status, architecture, specifications, and hardware records for ZMK-ThinkPad.
---

# ZMK-ThinkPad documentation

ZMK-ThinkPad is under development and has not reached a public release. These
pages record the current firmware, tested hardware, specifications, and work in
progress.

## Current status

- **T430 and T470 development-kit targets build today.** T430 includes its
  TrackPoint. T470 currently supports only the keyboard matrix.
- **T430 and T60 run on the custom Revision A controller.** T60 TrackPoint
  movement and all three buttons work. The T430 TrackPoint moves, but its
  keyboard assembly has not reported button presses.
- **Revision A is developmental groundwork.** It established first-time
  programming, USB/UF2 updates, serial debugging, and the bring-up lessons that
  inform Revision B. Its backlight remains disabled in release firmware.
- **Revision B, beginning with T470, is design work only.** It has not been
  implemented, fabricated, or validated.
- **Architecture** contains hardware design, programming infrastructure, and validated design decisions.
- **SPEC** contains authoritative behavioral and product contracts.

For current behavior, read the [transport and profile
contract](engineering/spec/transport-profile-contract.md). For planned hardware,
start with the [shared Revision B
core](engineering/architecture/hardware/shared/revision-b/core-wiring.md) and
[T470 adapter](engineering/architecture/hardware/t470/revision-b/connector.md).

The source code, issue tracker, and complete project history are available in the [GitHub repository](https://github.com/rampadc/zmk-thinkpad).
