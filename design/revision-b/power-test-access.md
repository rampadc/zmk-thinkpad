# Revision B power test access

> **Status: design only.** Revision B has not been built or tested. The
> consolidated Revision B hardware specification remains authoritative.

Revision B must support hands-free oscilloscope measurements during power-path
and backlight bring-up. Do not rely on probing component leads or holding a
probe against an unpopulated ordinary measurement pad.

## Hookable measurement points

Use these stocked surface-mount loop test points for ordinary `MP_`
measurements:

| Use | Manufacturer part | LCSC part | Nominal board area |
| --- | --- | --- | --- |
| Main power rails and local grounds | Keystone `5016` | `C238129` | 4.70 mm by 3.43 mm |
| Control, status and secondary rails | Keystone `5015` | `C2906768` | 3.43 mm by 1.78 mm |

Use Keystone `5016` for `MP_USB_CONN_VBUS`, `MP_USB_VBUS_RAW`, `MP_BQ_IN`,
`MP_BAT_PROTECTED`, `MP_VSYS`, `MP_VCC_REG`, `MP_BOOST_5V`, `MP_5V`,
`MP_BL_SWITCH_IN`, `MP_BL_5V`, and the six local ground loops.

Use Keystone `5015` for `MP_TYPEC_VDD`, `MP_CC1`, `MP_CC2`,
`MP_TYPEC_OUT1`, `MP_TYPEC_OUT2`, `MP_HUSB_DEBUG_N`, `MP_BQ_EN1`,
`MP_BQ_EN2`, `MP_BQ_ILIM`,
`MP_PWR_GOOD_N`, `MP_CHARGING_N`, `MP_SYS_EN`, `MP_VCC`,
`MP_TRACKPOINT_5V`, `MP_BL_ENABLE`, `MP_BL_PWM`, `MP_TP4_RESET`,
`MP_TP4_CLOCK` and `MP_TP4_DATA`.

Provide these Keystone `5016` local ground loops:

| Ground loop | Measurement block |
| --- | --- |
| `MP_GND_USB` | USB connector, protection and Type-C detection |
| `MP_GND_BQ` | Charger input, battery and charger status |
| `MP_GND_SYS` | `VSYS` and common converter enable |
| `MP_GND_3V3` | TPS63031, `VCC_REG` and `VCC` |
| `MP_GND_5V` | SY7069, `BOOST_5V`, `+5V` and TrackPoint branch |
| `MP_GND_BL` | Backlight switch, current shunt and `BL_5V` |

Place the associated ground loop within 5 mm of each principal rail loop where
possible and never farther than 10 mm. Connect every ground loop independently
to its block's common local ground plane with short, wide copper and nearby
vias; do not daisy-chain the loops. Arrange the loops so two attached probe
hooks cannot rotate into one another or short adjacent rails. Labels must
remain visible with the battery and keyboard adapter fitted.

## Specialized access

Keystone loops are ordinary measurement points, not sustained-current
injection terminals. Keep `INJ_5V`, `INJ_GND` and `INJ_VCC` as the specified
current-rated flat pads. Keep the removable zero-ohm links in the TrackPoint,
backlight, 5 V source, 3.3 V source and battery-ADC paths.

Keep `KL_BL_CURRENT_P` and `KL_BL_CURRENT_N` as a matched pair of small bare
Kelvin pads, each routed independently and directly to its shunt termination.
Do not share high-current injection copper or a Keystone loop as a Kelvin
connection.

Do not fit a hookable loop to a TPS63031 or SY7069 switching node. If
switching-node access is required, provide a tiny bare probe-tip pad and an
immediately adjacent bare ground-spring pad beside the regulator.

Revision B has no switched-system intermediate rail. `VSYS` feeds the
converter inputs directly, while SHOU HAN `MSK12C02`, LCSC `C431540`, controls
only the shared `SYS_EN` net. On the reviewed EasyEDA device, connect common
pad 2 to `VSYS`, ON-throw pad 3 to `SYS_EN`, leave OFF-throw pad 1
unconnected, and connect frame/shield pad 4 to GND. Do not use this 50 mA
switch to interrupt a load rail.

## Bring-up use

The placement must allow at least two probes and their local grounds to remain
attached while power and backlight controls are operated. Before PCB release,
check loop courtyards, hook access, silkscreen visibility, nearby ground access,
removable-link isolation, and Kelvin routing.

Before calling the power path validated, capture startup and shutdown under
USB-only, battery-only, USB-plus-battery, and batteryless high-current USB-C
conditions. Validate `VSYS` against `SYS_EN`, `VCC_REG` against `VCC`,
`BOOST_5V` against `+5V`, and the backlight input and `BL_5V` on both sides of
the load switch and shunt.
