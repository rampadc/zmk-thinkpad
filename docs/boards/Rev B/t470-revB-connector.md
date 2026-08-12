# T470 connector - Revision B wiring

Revision B uses a 40-pin, 0.5 mm bottom-contact FFC connector for the T470
keyboard cable. It follows the **straight** connector mapping:

```text
FPC1 pin = T470 keyboard/J38 contact + 2
```

FPC1 pins 1-2 and 39-40 are not keyboard cable contacts. FPC1 pin 38 is cable
contact/J38 pin 36, `KBD_ID`, intentionally left unconnected in Revision B.
Connector shield tabs 41 and 42 connect to ground. The table below is the
Revision B schematic source of truth; do not apply the reversed DK-breakout
numbering to this connector.

| FPC1 pin | Signal | Shared/module destination |
| ---: | --- | --- |
| 1-2 | NC | Leave unconnected |
| 3 | `SENSE3` | Shared matrix net |
| 4 | `SENSE7` | Shared matrix net |
| 5 | `SENSE6` | Shared matrix net |
| 6 | `DRV14` | Shared matrix net |
| 7 | `SENSE4` | Shared matrix net |
| 8 | `SENSE1` | Shared matrix net |
| 9 | `DRV0` | Shared matrix net |
| 10 | `SENSE2` | Shared matrix net |
| 11 | `SENSE0` | Shared matrix net |
| 12 | `DRV4` | Shared matrix net |
| 13 | `DRV2` | Shared matrix net |
| 14 | `SENSE5` | Shared matrix net |
| 15 | `DRV1` | Shared matrix net |
| 16 | `DRV3` | Shared matrix net |
| 17 | `DRV6` | Shared matrix net |
| 18 | `DRV7` | Shared matrix net |
| 19 | `DRV5` | Shared matrix net |
| 20 | `DRV15` | Shared matrix net |
| 21 | `DRV13` | Shared matrix net |
| 22 | `DRV9` | Shared matrix net |
| 23 | `DRV12` | Shared matrix net |
| 24 | `DRV10` | Shared matrix net |
| 25 | `DRV8` | Shared matrix net |
| 26 | `DRV11` | Shared matrix net |
| 27 | `VCC3M` | Regulated 3.0 V keyboard rail |
| 28 | `-LED_FNLOCK` | LED sink driver through 3.9 kOhm |
| 29 | `-LED_MUTE` | LED sink driver through 3.9 kOhm |
| 30 | `-LED_MICMUTE` | LED sink driver through 3.9 kOhm |
| 31 | `-HOTKEY` | HolyIOT pad 2 / `P1.11`, active-low with pull-up |
| 32 | GND | Ground plane |
| 33 | `-LED_CAPSLOCK` | LED sink driver through 3.9 kOhm |
| 34 | GND | Ground plane |
| 35 | `TP4LEFT` | HolyIOT pad 44 / `P0.08`, active-low with pull-up |
| 36 | `TP4RIGHT` | HolyIOT pad 45 / `P0.06`, active-low with pull-up |
| 37 | `TP4MIDDLE` | HolyIOT pad 46 / `P0.26`, active-low with pull-up |
| 38 | `KBD_ID` | Intentionally leave unconnected in Revision B |
| 39-40 | NC | Outside the 36-contact cable; leave unconnected |
| 41-42 | Connector shield | Ground plane |

The matrix nets terminate at the HolyIOT pins defined in
[`shared-module-wiring.md`](shared-module-wiring.md) and are shared with the
alternative T430 connector. Install only one keyboard at a time.

`TP4LEFT`, `TP4RIGHT`, and `TP4MIDDLE` are read directly by the nRF52840 in
Revision B. Do not bridge them to another TrackPoint-button input path. No
external pull-up or pull-down is required; firmware enables the nRF internal
pull-ups.

## Pre-release checks

- Confirm the footprint is bottom-contact and that physical pin 1 matches the
  schematic and PCB silkscreen orientation.
- Confirm FPC1 pin 27 reaches only the regulated 3.0 V rail and is not grounded.
- Confirm FPC1 pins 32 and 34, plus shield tabs 41 and 42, reach ground.
- Confirm pins 1-2 and 39-40 have no copper connection, and that pin 38
  (`KBD_ID`) is intentionally unconnected.
- Continuity-check every `DRV` and `SENSE` net between both alternative
  connector symbols and its single shared HolyIOT destination.
