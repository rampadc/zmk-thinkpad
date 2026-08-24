# T470 connector - Revision B wiring

> **Status: design only.** Revision B has not been built or tested.

This page defines the passive T470 adapter. It uses a 40-pin, 0.5 mm
bottom-contact FFC connector for the keyboard and connects to the core through
the 60-contact interface in
[`universal-connector.md`](../../shared/revision-b/universal-connector.md). The keyboard
connector follows the **straight** mapping:

```text
FPC1 pin = T470 keyboard/J38 contact + 2
```

FPC1 pins 1-2 and 39-40 are not keyboard cable contacts. FPC1 pin 38 is cable
contact/J38 pin 36, `KBD_ID`. It crosses the universal FFC on the T470 adapter,
but the core leaves it electrically unimplemented until it is characterized.
The T430 connector has no equivalent `KBD_ID` signal.
Connector shield tabs 41 and 42 connect to ground. The table below is the
Revision B schematic source of truth; do not apply the reversed DK-breakout
numbering to this connector.

| FPC1 pin | Signal | Universal adapter destination |
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
| 27 | `VCC` | Universal core pin 56 / daughter pad 5 |
| 28 | `-LED_FNLOCK` | Universal core pin 46 / daughter pad 15 |
| 29 | `-LED_MUTE` | Universal core pin 47 / daughter pad 14 |
| 30 | `-LEDMICMUTE` | Universal core pin 48 / daughter pad 13 |
| 31 | `-HOTKEY` | Universal core pin 28 / daughter pad 33 |
| 32 | GND | Universal core pin 55 / daughter pad 6 |
| 33 | `-LED_CAPSLOCK` | Universal core pin 49 / daughter pad 12 |
| 34 | GND | Universal core pin 58 / daughter pad 3 |
| 35 | `TP4LEFT` | Universal core pin 30 / daughter pad 31 |
| 36 | `TP4RIGHT` | Universal core pin 31 / daughter pad 30 |
| 37 | `TP4MIDDLE` | Universal core pin 32 / daughter pad 29 |
| 38 | `KBD_ID` | Universal core pin 33 / daughter pad 28; core leaves unimplemented until characterized |
| 39-40 | NC | Outside the 36-contact cable; leave unconnected |
| 41-42 | Connector shield | Ground plane |

The separate T470 J37 TrackPoint/backlight connector maps as follows:

| J37 pin | Signal | Universal core pin | Universal daughter-board pad |
| ---: | --- | ---: | ---: |
| 1 | `TP4_DATA` | 37 | 24 |
| 2 | `+5V` | 42 | 19 |
| 3 | GND | 44 | 17 |
| 4 | `TP4_RESET` | 41 | 20 |
| 5 | `TP4MIDDLE` | 32 | 29 |
| 6 | `TP4RIGHT` | 31 | 30 |
| 7 | `TP4LEFT` | 30 | 31 |
| 8 | `+5V` | 43 | 18 |
| 9 | `TP4_CLOCK` | 39 | 22 |
| 10 | `BL_5V` | 51 | 10 |
| 11 | `KBD_BL_PWM` | 50 | 11 |
| 12 | `-KBD_BL_DTCT` | 35 | 26 |
| 13-14 | Ground tabs | GND plane | GND plane |

The five LED outputs are distinct core-board sink channels. Do not merge
T430 `-LEDPWR` with T470 `-LED_FNLOCK`. The T470 keyboard FFC maps its four
LED signals to the universal interface as follows:

| T470 FPC1 | Signal | Universal core pin | Universal daughter-board pad |
| ---: | --- | ---: | ---: |
| 28 | `-LED_FNLOCK` | 46 | 15 |
| 29 | `-LED_MUTE` | 47 | 14 |
| 30 | `-LEDMICMUTE` | 48 | 13 |
| 33 | `-LED_CAPSLOCK` | 49 | 12 |

The matrix nets cross the universal FFC and terminate at the HolyIOT pins
defined in [`core-wiring.md`](../../shared/revision-b/core-wiring.md). Install only
one keyboard adapter at a time.

Tie universal `ADAPTER_ID` (core pin 34 / daughter pad 27) to GND on the T470
adapter. Leave it open on the T430 adapter. This strap is board-local and does
not connect to either keyboard cable.

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
