# Revision A bring-up learnings and Revision B requirements

This document records the electrical, firmware, programming, assembly, and
user-interface lessons learned while bringing up the first assembled T430
Revision A board. It is an input to the Revision B schematic, PCB, firmware,
fixture, and acceptance-test design.

Statements are classified as follows:

- **Confirmed** means directly observed or verified from generated firmware,
  measurements, continuity tests, logs, or working behavior.
- **Probable** is the best current explanation but has not been isolated.
- **Unresolved** must not be converted into a schematic assumption without a
  targeted test.

## What worked on Revision A

- The HolyIOT 18010 nRF52840 module boots and runs ZMK.
- USB device operation, USB CDC logging, BLE pairing, BLE reconnection, and
  switching among USB and three BLE profiles work.
- The 16-by-8 T430 keyboard matrix, separate Fn input, and separate power-button
  input can be scanned by the nRF52840.
- Normal typing works over both USB and BLE after the matrix map was corrected.
- TrackPoint movement is received over the standard PS/2 DATA and CLOCK lines.
- The three profile LEDs and the keyboard's power, mute, and microphone-mute
  LEDs can be controlled by firmware.
- The nRF52840 can be programmed through SWD using the ST-LINK/V2-B on an
  STM32F429I-DISCO. After the one-time bootloader installation, software-only
  UF2 updates over USB remove the need to hold a no-leg Tag-Connect cable.

## Programming and debug

### Tag-Connect footprint and cable orientation

The fitted footprint is the ARM Cortex 10-pin SWD assignment, even though an
ECAD symbol or connector end-view can make the numbering appear reversed. Pin
numbers, not apparent left-to-right drawing order, are authoritative:

| TC2050 pin | Revision A signal | Requirement |
| ---: | --- | --- |
| 1 | Target `VCC` / reference voltage | Sense the target's own nominal 3.0 V rail |
| 2 | `SWDIO` | Required |
| 3, 5, 9 | GND | Connect at least one; connect all in the PCB footprint |
| 4 | `SWDCLK` | Required |
| 6, 7, 8 | Not used for SWD | Leave unconnected |
| 10 | `nRESET` | Strongly recommended |

The STM32F429I-DISCO debugger must not power the target from its 3.3 V or 5 V
output. Remove the Discovery CN4 isolation jumpers, connect the target's 3.0 V
rail to debugger target-voltage sense, and open SB7 if required to prevent the
onboard STM32 reset circuit from loading an externally connected reset line.
Voltage sensing working does not prove SWDIO, SWDCLK, reset, or module solder
integrity.

### Make development updates independent of SWD

The no-leg Tag-Connect footprint is useful for manufacturing and recovery but
is unpleasant for repeated development because it must be held down. Revision A
therefore uses an Adafruit-compatible UF2 bootloader with a 1200-baud software
reboot trigger. Keep this workflow in Revision B:

1. Use SWD once to install and verify the bootloader and application.
2. Perform ordinary iterations through USB using `tools/flash-t430-reva` or its
   Revision B successor.
3. Keep SWD available for recovery, factory programming, and pre-USB faults.

No-button iteration is a product requirement. Do not require a reset-button
double press, case opening, or a held pogo cable for ordinary firmware updates.

Normal updates must preserve the settings partition and BLE bonds. Do not use
`mass_erase`, `recover`, or the nRF52840 DK `/Volumes/JLINK` mass-storage path.
The exact current memory layout and commands are in
[`../../flashing.md`](../../flashing.md).

### Revision B programming improvements

- Retain an accurately numbered TC2050-NL footprint and add unmistakable pin-1
  and cable-orientation silkscreen on both sides where useful.
- Add labeled, accessible pads for `VCC`, GND, SWDIO, SWDCLK, and `nRESET` so a
  fixture can diagnose the interface without relying on the TC2050 holes.
- Make the footprint reachable after final keyboard and enclosure assembly.
- Prefer a fixture or retaining clip for production; the operator must not hold
  a probe throughout flashing and verification.
- Factory programming must read the FICR device ID first, program at a
  conservative 1 MHz SWD rate, verify the image, reset, and confirm USB
  enumeration.
- Do not treat illuminated ST-LINK LEDs as evidence that the target is powered
  or communicating.

## Power-path observations

### Confirmed measurements

With USB connected during the initial investigation:

| Node | Observed voltage |
| --- | ---: |
| `USB-C_VBUS` | 5.246 V |
| `USB_VBUS_RAW` | 5.248 V |
| `VSYS` | approximately 4.5 V |
| `+5V` | approximately 4.939 V |
| regulator side of R22 / regulated `VCC` | steady 2.99 V |

Relevant continuity/resistance observations were:

- R22 measured approximately 0.21 ohm end to end.
- R22 output to HolyIOT VDD measured approximately 0.22 ohm.
- R22 output to the point initially identified as an LED `VCC` measured about
  4.7 kohm, demonstrating that those two probed points were not the same net.
- No persistent hard short was found from `VCC` to GND by the diode/continuity
  tests performed.

The regulator-side pad rose to 2.99 V in less than a second. A measurement made
between R22 pads appeared to take about seven seconds to reach 2.99 V; that was
a differential measurement across a very low-value series element and is not a
valid measurement of the output rail relative to ground. Every voltage test
must state both probe locations, and rail voltage must be measured to GND.

USB had remained connected during one apparently contradictory test. Record
USB, battery, latch, and debugger connections for every power measurement;
otherwise multiple source paths make the result ambiguous.

### One-hertz flashing was a reboot symptom

All profile LEDs, and at one point the T430 power LED, appeared to flash near
1 Hz while the supply/reset behavior cycled. This is not a documented HolyIOT
module heartbeat. The HolyIOT is a module containing the nRF52840 and support
components, but it has no intended circuit that drives the carrier's external
profile LEDs as a 1 Hz status display.

The observed reset voltage tracked the apparent rail disturbance: about 3 V
when the circuit was up and about 0 V during the low interval. Treat synchronized
external LED flashing as evidence of rebooting, rail collapse, or firmware
status restart—not as a HolyIOT diagnostic code.

### Revision B power requirements

- Implement the replacement power path specified in
  [`bq24073-power-path.md`](bq24073-power-path.md).
- Provide separately labeled measurement points for `USB_VBUS_RAW`,
  `BAT_PROTECTED`, `VSYS`, `VCC`, `+5V`, `TP_5V`, and `BL_5V`.
- Retain separable or measurable branches for logic, TrackPoint, and backlight.
  A zero-ohm link, current-measurement jumper, or fixture-accessible shunt must
  permit isolation without cutting a small resistor from an assembled board.
- Add explicit safe-state biasing so backlight power, LED sinks, TrackPoint
  reset, and every load switch remain inactive while the MCU is unpowered,
  resetting, or booting.
- Validate USB-only, battery-only, USB plus battery, battery-absent, depleted
  battery, latch-off, and source-transition cases.
- Measure both sides of every series element relative to GND during acceptance;
  never infer a rail from a voltage measured across the element.

## Keyboard connector, matrix, and assembly test

### Matrix architecture learned during bring-up

The T430 has sixteen driven matrix rows and eight pulled-up sense columns. Fn
and Power are separate active-low direct inputs. In particular, Power is not a
matrix position even though an early discovery build reported it as ZMK
position zero after the direct-input transform was appended.

The guided matrix finder runs inside ZMK after the keyboard is already active,
so ordinary key bindings can also type into the terminal running the test. This
made discovery output confusing and allowed transport behaviors to run while
the matrix was being characterized. A standalone factory image was added to
scan raw GPIOs without USB/BLE keyboard behavior. Retain that separation.

The matrix scanner was generalized from the T470 work, but a shared scanner
does not imply an identical physical map. T430 adds or exposes separate power,
hotkey/media, and LED functions, and each model must retain its own transform
and connector map.

### Assembly problems found

Intermittent shorts/opens around the 0.5 mm-pitch keyboard connector changed
with probing and flux contamination. After the flex area was cleaned/burnished
and joints were resoldered, the shorts disappeared and matrix testing improved.
Visual inspection and a single continuity measurement were insufficient.

Revision B intentionally breaks the fine-pitch keyboard interface out through
a replaceable FFC adapter. This is strongly supported by the bring-up result:

- the core board can be tested without the expensive keyboard connector;
- model-specific T430/T470 adapters can be tested separately;
- a damaged connector or adapter can be replaced without discarding the core;
- fixture access can be placed on the larger-pitch universal interface; and
- automated matrix stimulus/measurement does not require probing adjacent
  0.5 mm pins by hand.

### Required manufacturing tests

Provide a bed-of-nails or mating-FFC fixture capable of:

1. Testing every connector pin for opens and adjacent-pin shorts before a
   keyboard is attached.
2. Driving each `DRV0`-`DRV15` individually and verifying it only at the
   expected adapter pin.
3. Pulling each `SENSE0`-`SENSE7`, Fn, and Power low and verifying the correct
   firmware/raw-GPIO result.
4. Exercising every row-column combination with an analog-switch matrix or
   relay fixture and comparing the complete transform automatically.
5. Verifying that all ground contacts are low impedance and that power pins are
   isolated from adjacent signals before applying power.
6. Repeating the test while gently flexing the FFC/connector to expose
   intermittent solder joints.

The standalone T430 factory image under `factory_test/t430_reva` is the basis
for the firmware side of this fixture. Revision B should emit a stable,
machine-readable result in addition to human-readable serial logs.

## TrackPoint and its buttons

### Confirmed protocol behavior

T430 and T60-style TrackPoints use the same standard PS/2 mouse packet format.
The firmware performs the normal reset (`0xFF`) and enable-reporting (`0xF4`)
sequence. In the first byte of each three-byte packet:

- bit 0 is left;
- bit 1 is right; and
- bit 2 is middle.

The existing ZMK driver decodes those bits and forwards them as
`INPUT_BTN_0`, `INPUT_BTN_1`, and `INPUT_BTN_2`. No special T430 command or HID
mapping is required to enable the three buttons.

During diagnostic logging, TrackPoint movement generated valid packets, but
pressing left, middle, and right twice each always produced `b_l=0`, `b_m=0`,
and `b_r=0`. The raw keyboard matrix scanner also saw no events, as expected:
the TrackPoint buttons are not keyboard-matrix keys.

Open-source T60/older ThinkPad implementations confirm the electrical model:
the three physical switches ground three button-input pins at the TrackPoint
controller, which then places their state in the PS/2 packet. Consequently the
current failure is upstream of ZMK's packet decoder.

### Current TrackPoint-button conclusion

**Confirmed:** DATA, CLOCK, reset sequencing, PS/2 reception, movement decoding,
and HID pointer movement work. The received button bits never change.

**Probable:** the button flex, its shared ground, a connector contact, or the
three paths into the TrackPoint controller are open or incorrectly connected.

**Unresolved:** the exact physical discontinuity has not been isolated. Do not
label the TrackPoint ASIC defective until the closures are measured at both
sides of the flex/connector.

Revision B must expose fixture-accessible test points for TrackPoint DATA,
CLOCK, RESET, supply, ground, and—on an adapter where accessible—the three
button inputs. Acceptance testing must verify that each button independently
pulls its TrackPoint input to TrackPoint ground and changes the corresponding
PS/2 packet bit. Test simultaneous button combinations as well as individual
presses.

Keep the TrackPoint and backlight supply branches separate even though both are
derived from `+5V`. Movement working must never be used as proof that the
button flex or backlight branch is sound.

## Backlight fault and required redesign validation

Enabling the keyboard backlight produced an audible beep followed by an
apparent reboot/profile-LED restart. The behavior was repeatable enough that
Fn+Space was disabled in the Revision A keymap. The sequenced driver already
enables the rail, waits at least 35 ms, then applies nonzero PWM; on shutdown it
sets PWM to zero before disabling the rail.

Therefore:

- **Confirmed:** requesting backlight causes a system-level disturbance.
- **Confirmed:** firmware sequencing alone did not make Revision A safe.
- **Probable:** excessive inrush, converter/current-limit behavior, an
  incorrect backlight power/control connection, or rail coupling causes the
  reset and audible component noise.
- **Unresolved:** the failing node and current waveform have not been captured.

Do not re-enable the backlight in normal firmware until hardware validation is
complete. Revision B must provide:

- independently switchable and measurable `BL_5V`;
- current limiting or a controlled soft-start appropriate to measured load;
- local input/output bulk and high-frequency decoupling at the converter and
  keyboard connector;
- safe pull states on rail enable and PWM;
- test points for converter input, switch node where safely probeable, `+5V`,
  `BL_5V`, PWM, and enable; and
- oscilloscope/current-probe validation at off, minimum nonzero PWM, 50%, 100%,
  repeated transitions, USB-only, battery-only, and charging conditions.

Record input current, `VSYS`, `VCC`, `+5V`, `BL_5V`, reset, enable, and PWM on
the same timebase. A successful visual backlight test is not enough; no logic
rail or reset excursion and no audible magnetics noise should occur.

## LEDs and user-visible behavior

### Physical mapping

The Revision A profile LEDs were physically reversed relative to the intended
silkscreen/order. The observed mapping before the devicetree correction was:

| Command | Physical LED observed |
| --- | --- |
| Fn+1 | LED3 |
| Fn+2 | LED2 |
| Fn+3 | LED1 |
| Fn+4 / USB | all three |

The Revision A overlay now compensates in firmware. Revision B must correct the
schematic/layout/net labels so logical profile 1 reaches silkscreen LED1,
profile 2 reaches LED2, and profile 3 reaches LED3 without a board-specific
logical reversal. Add an LED fixture test that turns on one channel at a time
and requires the operator or camera to confirm the printed number.

All three profile LEDs are active-low. The keyboard's power, speaker-mute, and
microphone-mute signals use external low-side stages and are represented as
active-high MCU controls. Verify both MCU-side and keyboard-side polarity in
the fixture; signal names beginning with `-LED` describe the keyboard connector
side and do not necessarily describe the MCU GPIO polarity.

### Power LED

The keyboard power LED previously blinked with BLE reconnect/pairing status,
which was distracting and incorrectly coupled a general power indication to
the profile animation. Firmware was changed so pairing/reconnect animation is
exclusive to the three profile LEDs. The power LED is steady when USB HID is
ready or the selected BLE profile is connected and otherwise off.

Keep that separation in Revision B. Also retain hardware bias that leaves the
power LED off while the MCU boots, because a reboot must not look like an
intentional connection animation.

### Profile behavior

The authoritative behavior remains
[`../../transport-profile-contract.md`](../../transport-profile-contract.md):

- tapping Fn+1/Fn+2/Fn+3 selects a profile and must reconnect to an existing
  bond without asking to pair again;
- holding the command deliberately clears only that profile and starts pairing;
- Fn+4 explicitly selects USB and illuminates all three LEDs for one second;
- merely plugging or unplugging USB does not change the requested transport or
  create the three-LED animation; and
- the requested transport/profile is persistent and distinct from ZMK's
  currently usable endpoint fallback.

The T470 behavior is the compatibility reference, but changes must build and
pass the full contract on both T430 and T470.

## Media keys and host-state limitations

The T430 exposes dedicated speaker mute, volume down, volume up, microphone
mute, and OEM/ThinkVantage controls. They require explicit matrix positions and
consumer-page HID usages; they are not inferred from the LEDs.

Testing on macOS showed the volume overlay appearing even when the perceived
system value did not change, and mute/microphone-mute behavior was initially
confusing. This remains a host-compatibility test item, not a proven electrical
fault. Validate press and release reports over both USB and BLE against actual
system state, not merely the on-screen overlay.

The keyboard generally cannot read macOS speaker- or microphone-mute state.
Those consumer controls are commands/toggles, and the host does not normally
return authoritative state through the keyboard LED output report. Caps Lock
is different because standard keyboard output reports carry its LED state.
Consequently:

- never claim the mute LEDs mirror host state unless a tested host-specific
  feedback channel is implemented;
- if firmware locally toggles mute LEDs, document that they show the keyboard's
  last command, not guaranteed macOS state; and
- test state divergence caused by mouse/menu changes, another keyboard,
  application-specific microphone control, reconnect, and reboot.

## Firmware architecture retained for Revision B

- Keep the HolyIOT board definition separate from the T430/T470 shields.
- Keep model-specific physical transforms and connector mappings while sharing
  matrix-scanning logic, profile control, status semantics, and TrackPoint PS/2
  support.
- Keep smoke, standalone factory, USB-debug, RTT-debug, and release builds
  reproducible and separate.
- Use USB CDC for routine diagnostics on assembled hardware; do not assume the
  nRF52840 DK UART wiring exists on the custom board.
- Ensure release builds disable verbose packet and matrix logging.
- Keep hazardous or unvalidated loads, especially backlight, unbound in the
  normal keymap until their acceptance tests pass.
- Statically inspect generated devicetree, Kconfig, and HEX address ranges for
  every board revision. A successful compile does not validate physical GPIO
  order or active polarity.

## Revision B release gates

Do not release the Revision B PCB or call its firmware complete until all of
the following have objective evidence:

1. Schematic-to-layout and layout-to-devicetree pin audit, including connector
   viewing direction, pin 1, GPIO port, polarity, pull, and startup state.
2. Automated open/adjacent-short test of the universal FFC and each passive
   keyboard adapter.
3. Standalone raw test of all 128 matrix intersections plus Fn and Power.
4. Functional test of every physical key, including all dedicated media keys
   and Fn combinations, before enabling USB/BLE keyboard output.
5. TrackPoint movement plus independent and simultaneous left/middle/right
   packet-bit tests.
6. One-channel-at-a-time profile and keyboard LED polarity/order test.
7. Full USB/BLE profile contract test, including bond persistence after an
   ordinary UF2 update.
8. Backlight power-integrity test with synchronized voltage/current/reset
   captures and no audible noise or reboot.
9. Power-path test across every source/latch/battery state and source
   transition.
10. One-time SWD programming plus repeated no-button USB UF2 updates, with a
    verified recovery path.
11. RF range and current-consumption tests with the real keyboard backplate,
    battery, FFC, and enclosure installed.
12. Mechanical inspection confirming antenna keepout, strain relief, connector
    retention, USB shell support, battery protection, and programming access.

## External implementation references

The TrackPoint conclusions above are consistent with these open-source
implementations and reverse-engineering notes:

- [ZMK PS/2 mouse and TrackPoint driver](https://github.com/infused-kim/kb_zmk_ps2_mouse_trackpoint_driver)
- [ThinkPad keyboard and TrackPoint reuse](https://github.com/delingren/thinkpad_keyboards)
- [T60/T61 TrackPoint integration notes](https://github.com/joric/jorne/wiki/Tracking)
- [Older ThinkPad USB keyboard controller](https://github.com/hamishcoleman/thinkpad-usbkb)

These references corroborate the architecture; Revision B acceptance still
depends on measurements from the actual T430 keyboard and assembled board.
