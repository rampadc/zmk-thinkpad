---
title: Wiring summary
description: Historical summary of the manufactured T430 Revision A controller.
---

# T430 Revision A wiring

Revision A replaced the nRF52840 DK with a HolyIOT 18010 module. It proved the
keyboard, TrackPoint movement, USB, Bluetooth, LEDs, battery power, and update
path on a custom board. It includes a backlight control circuit, but the
backlight caused resets during testing and remains disabled in release
firmware.

Use these files for exact construction details:

- [Revision A schematic](schematic.pdf)
- [Purchased BOM](purchased-bom.xls)
- [BOM notes](bom.md)
- [Bring-up findings](../../../design-decisions/revision-a-bringup.md)

Revision A is historical. Do not use it as the starting point for Revision B.

## Interfaces

The board connects:

- the T430 16-by-8 keyboard matrix;
- separate Fn and power-button inputs;
- TrackPoint DATA, CLOCK, RESET, and three buttons;
- the power, speaker-mute, and microphone-mute LEDs;
- three Bluetooth profile LEDs;
- keyboard backlight PWM and power control;
- USB 2.0 device data; and
- SWD through a Tag-Connect footprint.

The nRF52840 uses 3.0 V logic. The keyboard matrix uses internal pull-ups.
TrackPoint DATA and CLOCK cross between 3.0 V and 5 V through BSS138 level
shifters. Never connect a 5 V signal directly to the module.

## Power

Revision A uses these rails:

| Rail | Purpose |
| --- | --- |
| `USB_VBUS_RAW` | Protected USB input and presence detection |
| `VSYS` | Charger power-path output |
| `VSYS_SW` | Main switched system supply |
| `VCC` | Regulated 3.0 V logic and module supply |
| `+5V` | Boost output for TrackPoint and backlight |

The latching power button controls the main P-channel MOSFET. The T430 power
button remains a firmware input. The reset button only drives nRESET.

The charger remains connected when the main system power is off. Use only a
protected single-cell 4.2 V Li-ion or LiPo battery with the marked polarity.

## Programming and updates

The Tag-Connect footprint provides SWDIO, SWCLK, nRESET, VCC sense, and ground.
The target powers itself; an external debugger must not drive the 3.0 V rail.

SWD installs or recovers the UF2 bootloader. Normal development uses USB and
UF2. See [Revision A programming](../../../programming/revision-a-programming.md).

## Known Revision A issues

- TrackPoint reset polarity requires a firmware workaround.
- The original charger and power path are replaced in Revision B.
- Fine-pitch assembly and connector inspection need better test coverage.
- TrackPoint and backlight current require separate measurement points.
- Repeated development must not depend on holding a Tag-Connect probe.

The [Revision A bring-up record](../../../design-decisions/revision-a-bringup.md)
contains the measurements and resulting Revision B requirements.
