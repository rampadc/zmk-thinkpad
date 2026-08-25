---
title: Changes deferred to Revision B
description: Revision A limitations that moved into the Revision B design.
---

# Changes deferred to Revision B

Revision A remains as built. Revision B carries these changes:

- Replace the ETA6002 and SY6280 power path with the BQ24073-based Revision B
  architecture.
- Move the keyboard connector to a replaceable passive adapter.
- Correct the TrackPoint reset circuit and profile LED order in hardware.
- Add test access for power rails, SWD, matrix lines, TrackPoint, and backlight.
- Keep USB/UF2 updates as the normal development path.
- Validate backlight startup and current before enabling it in release firmware.
- Replace the Q8/Q9 backlight switch with the controlled-rise Revision B load
  switch, and add the removable 5 V source link, current shunt, current-rated
  injection pads, and labeled power/PWM measurement points.
- Default to USB500 for legacy and default-current sources, but fit the
  Revision B Type-C CC-current detector so a valid 1.5 A or 3 A advertisement
  can select the BQ24073's approximately 1.34 A input limit. Permit
  batteryless backlight operation only in those detected high-current states;
  otherwise require a valid battery to supplement the cable input.
- Add a battery-voltage divider and ADC input if battery reporting remains a
  product requirement.

The [bring-up record](../../../design-decisions/revision-a-bringup.md) gives the
evidence behind these changes. The [Revision A schematic](schematic.pdf) and
[purchased BOM](purchased-bom.xls) remain the record of the manufactured board.
