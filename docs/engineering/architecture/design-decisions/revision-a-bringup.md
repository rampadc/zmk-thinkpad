---
title: Revision A bring-up
description: Observations from the first T430 board and the requirements they created for Revision B.
---

# Revision A bring-up

Revision A proved the main design and exposed the problems Revision B must fix.
This page separates measured results from likely causes.

- **Confirmed:** observed in measurements, logs, continuity tests, or working hardware.
- **Probable:** consistent with the evidence but not isolated.
- **Open:** needs a targeted test.

## Working features

- ZMK runs on the HolyIOT 18010 nRF52840 module.
- USB HID, USB serial, BLE pairing, reconnection, and profile switching work.
- The 16-by-8 T430 matrix, Fn, and power button scan correctly.
- TrackPoint movement works over PS/2.
- Firmware controls the profile, power, mute, and microphone-mute LEDs.
- SWD works through an STM32F429I-DISCO ST-LINK.
- The UF2 bootloader supports later updates over USB.

## Programming

The TC2050 footprint uses the ARM 10-pin SWD assignment:

| Pin | Signal |
| ---: | --- |
| 1 | Target VCC sense |
| 2 | SWDIO |
| 3, 5, 9 | Ground |
| 4 | SWCLK |
| 10 | nRESET |

The target powers itself. Do not connect the debugger's 3.3 V or 5 V output.
Use SWD for first installation, factory programming, and recovery. Use USB/UF2
for normal development. Normal updates must preserve settings and BLE bonds.

Revision B needs accessible SWD test pads, clear pin-1 marking, fixture support,
and USB updates that require neither an open enclosure nor a held probe. See
[Revision A programming](../programming/revision-a-programming.md).

## Power

Measured with USB connected:

| Node | Voltage |
| --- | ---: |
| `USB-C_VBUS` | 5.246 V |
| `USB_VBUS_RAW` | 5.248 V |
| `VSYS` | about 4.5 V |
| `+5V` | about 4.939 V |
| `VCC` | 2.99 V |

Measure rails against ground and record every connected power source. A
voltage across a series resistor is not the rail voltage.

Synchronized 1 Hz flashing on several LEDs came from the HolyIOT module's
factory firmware before ZMK was installed. It did not indicate a reset or power
fault.

Revision B uses a BQ24073-based power path. It also needs measurement points
for each rail, separable TrackPoint and backlight
branches, defined startup states, and tests for every USB, battery, and latch
combination.

## Keyboard connector and matrix

The T430 uses 16 driven rows, eight pulled-up columns, and separate active-low
Fn and power inputs. Each keyboard model needs its own connector map and matrix
transform even when it shares scanner code.

Flux contamination and weak joints around the 0.5 mm connector caused
intermittent opens and shorts. Inspection and one continuity reading did not
find every fault. This result led to the replaceable Revision B keyboard
adapter.

A production fixture must:

1. Find opens and adjacent-pin shorts.
2. Check each drive and sense line at the expected adapter pin.
3. Check Fn and power separately.
4. Exercise all 128 matrix intersections.
5. Check ground and power isolation before applying power.
6. Repeat while flexing the connector.

The standalone image in `factory_test/t430_reva` is the starting point for
automated fixture output.

## TrackPoint

Movement works and produces valid three-byte PS/2 packets. On the T430
keyboard, the packet button bits remained zero during button presses:

- bit 0: left;
- bit 1: right; and
- bit 2: middle.

The same firmware reports all three buttons correctly on a T60 keyboard. This
confirms the ZMK decoder and the shared PS/2 button path. Treat the T430
keyboard assembly as defective unless a T470 keyboard produces the same
all-zero button bits. If T470 also fails, reopen the shared hardware and
firmware investigation.

Revision B needs accessible DATA, CLOCK, RESET, supply, ground, and button test
points. Acceptance tests must check individual and simultaneous button presses
in raw PS/2 packets.

## Backlight

Enabling the backlight caused an audible beep and reboot even with sequenced
rail enable and PWM. Firmware sequencing did not solve the hardware fault.
Inrush, current limiting, wiring, or rail coupling remains the probable cause.

Keep the backlight disabled until testing captures input current, `VSYS`,
`VCC`, `+5V`, `BL_5V`, reset, enable, and PWM on one timebase. Revision B needs
a measurable backlight branch, controlled startup, local decoupling, and safe
default states.

## LEDs and transport

Revision A wired the three profile LEDs in reverse order. Firmware compensates
for that board. Revision B must wire logical profiles 1, 2, and 3 to the matching
labels.

Profile LEDs are active low. The keyboard power, mute, and microphone-mute
signals use external low-side stages and active-high MCU controls. Tests must
check polarity on both sides of each stage.

Transport behavior remains defined by the
[transport profile contract](../../spec/transport-profile-contract.md). Test
every transition on T430 and T470, and test T60 when shared Revision A behavior
changes. The active endpoint must not replace the stored transport request.

Mute LEDs do not receive authoritative mute state from standard host keyboard
reports. Until a tested feedback channel exists, they can show only the
keyboard's last command.

## Revision B release checks

Before releasing Revision B:

1. Audit the schematic, layout, connector views, devicetree pins, polarity,
   pulls, and startup state.
2. Test the universal FFC and each adapter for opens and shorts.
3. Test all matrix intersections, Fn, power, and media keys.
4. Test TrackPoint movement and every button combination.
5. Test every LED channel and label.
6. Run the complete USB/BLE profile contract, including update persistence.
7. Capture backlight and power-path behavior under every supply condition.
8. Verify first-time SWD, repeated USB updates, and recovery.
9. Measure RF range and current in the assembled enclosure.
10. Inspect antenna clearance, strain relief, connectors, battery protection,
    USB support, and programming access.
