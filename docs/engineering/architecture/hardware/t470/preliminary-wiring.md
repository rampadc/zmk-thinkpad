---
sidebar_position: 2
title: Preliminary T470 wiring
description: Historical DK-era T470 wiring that predates Revision B.
---

# Preliminary T470 wiring

This early plan mapped a T470 keyboard and TrackPoint to a HolyIOT 18010 module.
It was not manufactured as Revision A. The T470 work now starts with the
[Revision B connector](revision-b/connector.md) and the
[shared Revision B core](../shared/revision-b/core-wiring.md).

## Confirmed work

- The keyboard uses a 40-contact, 0.5 mm FFC plus a separate J37 TrackPoint and
  backlight connector.
- The firmware supports the complete 84-key matrix and Fn media controls.
- Backlight power and PWM are separate signals.

The current T470 firmware does not enable the TrackPoint, keyboard LEDs, or
backlight. The following electrical requirements come from the connector design
and earlier ThinkPad work; they still need validation on T470 hardware:

- PS/2 DATA and CLOCK require level shifting between the TrackPoint supply and
  the 3.0 V nRF52840.
- TrackPoint reset requires an open-drain sink or equivalent driver.
- Keyboard indicator inputs require transistor sinks if they are confirmed as
  active low.

## Backlight measurements

The T470 keyboard backlight was tested with 50 kHz PWM:

| Duty cycle | Measured current | Result |
| ---: | ---: | --- |
| 80% | 595 mA | Stable |
| 85% | 620 mA | Stable |
| 90% | Not measured | The Analog Discovery supply shut down |

The 90% result marks a limit in the test setup, not a measured keyboard current
or a confirmed keyboard fault. Repeat the test with a suitable current-limited
supply and record the supply voltage, voltage at the keyboard, and current-limit
setting.

These results exceed the provisional 500 mA backlight budget before full duty.
Revision B must either support the measured load or enforce a tested brightness
limit. Include keyboard logic, TrackPoint, radio, and battery charging when
checking the complete USB power budget.

The DK wiring was useful for firmware development but was not a PCB release
definition. Connector orientation, pin numbering, voltage levels, current, and
mechanical fit still require first-article checks on Revision B.

## Current sources

- [T470 Revision B connector](revision-b/connector.md)
- [Shared Revision B module wiring](../shared/revision-b/core-wiring.md)
- [Universal adapter connector](../shared/revision-b/universal-connector.md)
- [Revision B power path](../shared/revision-b/power-path.md)
