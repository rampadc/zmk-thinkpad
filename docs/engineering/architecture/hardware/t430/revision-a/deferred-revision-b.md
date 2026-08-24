---
title: Changes deferred to Revision B
description: Revision A limitations that moved into the Revision B design.
---

# Changes deferred to Revision B

Revision A remains as built. Revision B carries these changes:

- Replace the ETA6002 and SY6280 power path with the
  [BQ24073 design](../../shared/revision-b/power-path.md).
- Move the keyboard connector to a replaceable passive adapter.
- Correct the TrackPoint reset circuit and profile LED order in hardware.
- Add test access for power rails, SWD, matrix lines, TrackPoint, and backlight.
- Keep USB/UF2 updates as the normal development path.
- Validate backlight startup and current before enabling it in release firmware.
- Add a battery-voltage divider and ADC input if battery reporting remains a
  product requirement.

The [bring-up record](../../../design-decisions/revision-a-bringup.md) gives the
evidence behind these changes. The [Revision A schematic](schematic.pdf) and
[purchased BOM](purchased-bom.xls) remain the record of the manufactured board.
