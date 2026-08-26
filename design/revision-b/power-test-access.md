# Revision B power test access

> **Status: design only.** Revision B has not been built or tested.

Revision B must support hands-free oscilloscope measurements during power-path
and backlight bring-up. Do not rely on probing component leads or holding a
probe against an unpopulated copper pad.

## Hookable test points

Use these stocked surface-mount loop test points:

| Use | Manufacturer part | LCSC part | Nominal board area |
| --- | --- | --- | --- |
| Main power rails and grounds | Keystone `5016` | `C238129` | 4.70 mm by 3.43 mm |
| Control, status and secondary rails | Keystone `5015` | `C2906768` | 3.43 mm by 1.78 mm |

The larger `5016` loops must be usable with ordinary oscilloscope probe hooks.
Use `5016` for `USB_VBUS_RAW`, the charger input after input limiting, `VSYS`,
`VSYS_SW`, `BAT_PROTECTED`, `+5V`, `BL_5V`, and their principal ground points.

Use `5015` for `VCC`, charger status/power-good, power-path enable, 5 V boost
enable, backlight enable, backlight PWM, USB-C current-mode detector outputs,
battery sense, the TrackPoint 5 V branch, and other low-current diagnostic
signals that remain accessible after assembly.

Place a hookable ground loop within 5 mm preferred and 10 mm maximum of each
main rail loop. At minimum, provide these adjacent pairs:

```text
VSYS       GND
VSYS_SW    GND
+5V        GND
BL_5V      GND
VCC        GND
```

Keep labels visible with the keyboard adapter and battery fitted. Do not place
loops where a probe hook can short two rails or foul the enclosure.

## Current measurement and injection

Hookable loops are measurement points, not sustained high-current injection
terminals. Provide named, removable zero-ohm links in the TrackPoint 5 V and
backlight 5 V branches. Keep the specified 50 mOhm backlight current shunt;
do not infer current from the unspecified resistance of a zero-ohm link.

Provide large, current-rated plated pads on both sides of each removable link
and at `INJ_5V`. Also provide an unpopulated 2.54 mm two-pin header footprint
where space permits. Size the pads, traces and optional header for the branch's
fault current as well as its expected operating current.

Give every current shunt a separate pair of Kelvin sense pads routed directly
to the shunt terminals. Do not share the high-current injection pads as the
Kelvin measurement connection.

## Switching-node probing

Do not fit a hookable loop to a regulator switching node such as boost `LX`.
The loop and a long oscilloscope ground lead add capacitance, collect noise and
increase the chance of a destructive short. If switching-node access is
required, provide a small bare probe pad beside the regulator and a directly
adjacent ground pad for a probe-tip ground spring.

## Bring-up use

The placement must allow at least two probes to remain attached while the
keyboard power and backlight controls are operated. The first backlight
capture uses `VSYS_SW` and `BL_5V`. Subsequent captures compare `VSYS` with
`VSYS_SW`, then `VCC` with `BL_5V`, without changing the ground arrangement.

Before PCB release, check test-point courtyard clearance, hook access,
silkscreen labels, adjacent grounds, removable-link isolation and Kelvin
routing. Before calling the power path validated, capture startup and shutdown
under USB-only, battery-only, USB-plus-battery and batteryless high-current
USB-C conditions.
