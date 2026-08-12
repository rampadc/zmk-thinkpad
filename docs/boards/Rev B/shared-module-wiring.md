# Shared T430/T470 Revision B HolyIOT wiring

Revision B has one HolyIOT-18010 nRF52840 core and two alternative keyboard
connectors: T430 and T470. The connector nets meet at the shared named matrix
nets below. This document is authoritative for the module end of those nets;
connector pin numbers belong in the model-specific wiring documents.

The T470 straight 40-pin connector is defined in
[`t470-revB-connector.md`](t470-revB-connector.md).

Only one keyboard may be installed at a time. Do not connect T430 and T470
keyboards simultaneously, because their switch matrices would be electrically
paralleled. Pad numbers use the module manufacturer's top-view convention.

## Shared matrix nets

Both connectors connect to the same `DRV0`-`DRV15` and `SENSE0`-`SENSE7` nets
by signal name. Do not route by connector pin number or by the nRF52840 DK
prototype GPIO number.

| Matrix net | HolyIOT pad | nRF52840 GPIO |
| --- | ---: | --- |
| `DRV0` | 28 | `P1.03` |
| `DRV1` | 20 | `P0.19` |
| `DRV2` | 16 | `P1.09` |
| `DRV3` | 11 | `P0.31` |
| `DRV4` | 3 | `P1.10` |
| `DRV5` | 5 | `P1.15` |
| `DRV6` | 9 | `P0.29` |
| `DRV7` | 13 | `P0.05` |
| `DRV8` | 55 | `P0.16` |
| `DRV9` | 27 | `P1.00` |
| `DRV10` | 18 | `P0.23` |
| `DRV11` | 30 | `P1.02` |
| `DRV12` | 47 | `P1.07` |
| `DRV13` | 49 | `P0.24` |
| `DRV14` | 34 | `P1.06` |
| `DRV15` | 48 | `P1.05` |
| `SENSE0` | 6 | `P0.03` |
| `SENSE1` | 15 | `P0.07` |
| `SENSE2` | 10 | `P0.30` |
| `SENSE3` | 8 | `P0.28` |
| `SENSE4` | 12 | `P0.04` |
| `SENSE5` | 4 | `P1.13` |
| `SENSE6` | 17 | `P0.12` |
| `SENSE7` | 19 | `P0.21` |

Configure drives, in exact `DRV0`-`DRV15` order, as active-low open-drain
outputs. Configure senses, in exact `SENSE0`-`SENSE7` order, as active-low
inputs with internal pull-ups. Do not fit external matrix pull-ups by default.

These assignments intentionally differ from the T470 DK prototype. The DK
changes worked around peripherals physically fitted to the development kit;
they are not HolyIOT restrictions:

| Net | T470 DK prototype | Revision B HolyIOT |
| --- | --- | --- |
| `DRV1` | `P0.26` | `P0.19` |
| `DRV8` | `P0.02` | `P0.16` |
| `DRV10` | `P1.08` | `P0.23` |
| `SENSE1` | `P0.25` | `P0.07` |
| `SENSE7` | `P0.27` | `P0.21` |

`P0.07` is safe here: the CTS conflict belongs to the DK's J-Link VCOM circuit.
The DK's onboard QSPI wiring also does not exist on the HolyIOT module. Use a
HolyIOT-specific firmware overlay rather than the DK overlay unchanged.

## Shared and model-specific direct inputs

| Function | HolyIOT pad | GPIO | Configuration |
| --- | ---: | --- | --- |
| Fn / `-HOTKEY` | 2 | `P1.11` | Active-low input, internal pull-up |
| T430 power / `-PWRSWITCH` | 26 | `P0.22` | Active-low input, internal pull-up |
| T470 `TP4MIDDLE` | 46 | `P0.26` | Active-low input, internal pull-up |
| T470 `TP4RIGHT` | 45 | `P0.06` | Active-low input, internal pull-up |
| T470 `TP4LEFT` | 44 | `P0.08` | Active-low input, internal pull-up |

The T470 button nets are read directly by the nRF and reported as mouse
buttons. Do not also bridge them to the TrackPoint controller unless that
arrangement has been tested. No external button pull resistor is required.

## Reserved and auxiliary module pins

| Function | HolyIOT pad | GPIO |
| --- | ---: | --- |
| Battery sense | 7 | `P0.02/AIN0` |
| TrackPoint reset | 54 | `P0.13` |
| TrackPoint clock | 53 | `P0.14` |
| TrackPoint data | 52 | `P0.15` |
| Microphone-mute LED | 51 | `P0.17` |
| Speaker-mute LED | 50 | `P0.20` |
| Fn-lock LED | 43 | `P0.27` |
| BLE profile LED 1 | 42 | `P1.08` |
| BLE profile LED 2 | 41 | `P0.11` |
| BLE profile LED 3 | 40 | `P0.25` |
| Backlight enable | 39 | `P1.12` |
| Caps Lock LED | 38 | `P1.14` |
| Backlight PWM | 33 | `P1.04` |
| Power/status LED | 29 | `P1.01` |

TrackPoint clock, data and reset still require the model-specific voltage
conditioning. This table assigns GPIOs; it does not authorize direct 5 V
connections to the nRF52840.

## Power, programming and release checks

- Connect module grounds at pads 1, 25 and 37 to the ground plane.
- Connect pad 14, `VDD-nRF`, to the regulated supply with required decoupling.
- Reserve pads 22-24 for USB VBUS, D- and D+ and pads 31-32 for SWD.
- Leave `P0.09` and `P0.10` unused unless NFC is explicitly disabled.
- Verify both connector symbols map contacts to named nets; never infer T470
  auxiliary pins from nearby trace crossings.
- Run ERC to ensure no matrix net reaches two HolyIOT GPIOs.
- Confirm `P0.02` connects only to the battery divider and `DRV8` connects to
  pad 55 / `P0.16` from both connector options.
- Build distinct T430 and T470 HolyIOT firmware variants from this allocation.
- Populate one keyboard connector only and run a complete key test for each
  model on first articles.
