# T430 Holyiot preliminary PCB wiring and power plan

This document plans the custom PCB that will replace the nRF52840 DK with a
Holyiot 18010 V1.0 module. It covers the T430 keyboard connector, TrackPoint,
backlight, indicator LEDs, USB, power, SWD programming, and the corresponding
firmware/build work.

The current cut-tape shopping list and purchasing assumptions are in the
[preliminary home-assembly BOM](preliminary-bom.md).

> **Status:** this is the proposed Holyiot layout. It has not yet been
> applied to the firmware. The current `thinkpad_t430.overlay` still contains
> the tested nRF52840 DK pin assignment. Do not route a preliminary PCB from the
> DK table in the root README.

## Design goals

- Keep the 44-pin T430 keyboard connector routes as straight as practical.
- Use the nRF52840 internal pull-ups for every 3.0 V GPIO input that needs one.
- Keep all 5 V signals away from nRF52840 GPIOs.
- Support USB HID, BLE, USB logging, and three BLE profiles.
- Wake a suspended host from keyboard or TrackPoint activity over the selected
  USB or BLE transport, subject to the host's wake settings.
- Make SWD programming and recovery possible even when USB firmware is broken.
- Preserve the keyboard backlight, TrackPoint, power LED, mute LED, and
  microphone-mute LED.
- Add three low-current indicators for BLE profiles 1-3.
- Keep the RF antenna clear of copper and the T430 keyboard's metal backplate.

## References and pin-number conventions

The preliminary plan uses the newer **18010-A top-view numbering** shown in the
[Holyiot 18010-A manual](https://fccid.io/2ALGY-18010-A/User-Manual/User-Manual-5877632.pdf).
The drawing is 13.5 mm by 18 mm and labels the module `VER1.0`.

An older Holyiot manual numbers many of the same physical pads while looking
at the bottom of the module. It also disagrees with the newer manual about the
P1.10/P1.11 order. Use GPIO names such as `P1.10`, not pad numbers alone, in
the schematic and PCB net names. Before assembly, compare the purchased module
with the top-view drawing and continuity-check the questionable P1.10/P1.11
pads if possible.

The T430 connector is `J7` in the
[T430 motherboard schematic](https://indiarefix.in/download/file.php?id=9313).
Connector pin numbering reverses visually depending on which side of the flex
cable is being viewed. Mark pin 1 on both the schematic and PCB silkscreen and
verify continuity before applying power.

## System voltage domains

Revision A operates the nRF52840 GPIO domain at a nominal 3.0 V. Its GPIOs are
not 5 V tolerant.

| Rail or signal group | Voltage | Notes |
| --- | ---: | --- |
| `VCC` at Holyiot `VDD-nRF`, module pad 14 | 3.0 V nominal | Never connect directly to USB or `+5V` |
| T430 J7 pin 35 | 3.0 V nominal | Original rail was 3.3 V; supplies the indicator LEDs |
| T430 J7 pins 29 and 31 | 5 V | Keyboard backlight supply |
| T430 J7 pin 38 | 5 V | TrackPoint supply |
| T430 J7 pins 27, 34, and 41-44 | Ground | Join all six contacts to one solid PCB ground system |
| `KBD_BL_PWM` | 3.0 V logic | Original EC used 3.3 V; never translate it to 5 V |
| TrackPoint DATA/CLOCK/RESET | 5 V open-collector side | Requires translation/isolation |

The keyboard matrix, Fn key, and power button are passive switch contacts. They
do not require 5 V translation.

## Module placement and PCB stack-up

Place the Holyiot at a PCB edge with its ceramic antenna at the outside edge.
The preferred order is:

```text
case or PCB edge
┌─────────────────────────────────────┐
│ Holyiot antenna: no copper or metal │
│ ┌─────────────────────────────────┐ │
│ │        Holyiot 18010 V1.0       │ │
│ └─────────────────────────────────┘ │
│       short fan-out and vias        │
│                                     │
│ J7 pin 1 ═════════════════ pin 44   │
└─────────────────────────────────────┘
```

Keep copper pours, traces, planes, stitching vias, the flex cable, battery,
and other metal away from the antenna on every layer. The T430 keyboard has a
large metal backplate, so position the antenna near a plastic case opening or
outside the backplate footprint.

A four-layer PCB is strongly recommended:

1. Top: module, J7, USB, translators, and short signal routes.
2. Layer 2: solid ground, except for the complete antenna keepout.
3. Layer 3: 3.0 V and 5 V distribution.
4. Bottom: remaining matrix routes and low-speed controls.

Place J7 with pin 1 nearest the Holyiot's pad 1/left-edge side. This lets the
matrix lines leave the module in nearly the same order in which they enter J7.

## Proposed Holyiot GPIO assignment

This assignment is optimized for the 18010-A **top-view** footprint. It must
replace the current DK assignment in a future Holyiot-specific devicetree.

| Function | T430 J7 pin | Holyiot pad | nRF52840 GPIO | Electrical behavior |
| --- | ---: | ---: | --- | --- |
| Fn, `-HOTKEY` | 1 | 2 | P1.11 | Active-low input, internal pull-up |
| `DRV4` | 2 | 3 | P1.10 | Matrix output |
| `SENSE5` | 3 | 4 | P1.13 | Active-low input, internal pull-up |
| `DRV5` | 4 | 5 | P1.15 | Matrix output |
| `SENSE0` | 5 | 6 | P0.03 | Active-low input, internal pull-up |
| `DRV8` | 6 | 7 | P0.02 | Matrix output |
| `SENSE3` | 7 | 8 | P0.28 | Active-low input, internal pull-up |
| `DRV6` | 8 | 9 | P0.29 | Matrix output |
| `SENSE2` | 9 | 10 | P0.30 | Active-low input, internal pull-up |
| `DRV3` | 10 | 11 | P0.31 | Matrix output |
| `SENSE4` | 11 | 12 | P0.04 | Active-low input, internal pull-up |
| `DRV7` | 12 | 13 | P0.05 | Matrix output |
| `SENSE1` | 13 | 15 | P0.07 | Active-low input, internal pull-up |
| `DRV2` | 14 | 16 | P1.09 | Matrix output |
| `SENSE6` | 15 | 17 | P0.12 | Active-low input, internal pull-up |
| `DRV10` | 16 | 18 | P0.23 | Matrix output |
| `SENSE7` | 17 | 19 | P0.21 | Active-low input, internal pull-up |
| `DRV1` | 18 | 20 | P0.19 | Matrix output |
| Power, `-PWRSWITCH` | 19 | 26 | P0.22 | Active-low input, internal pull-up |
| `DRV9` | 20 | 27 | P1.00 | Matrix output; therefore SWO is unavailable |
| `DRV0` | 22 | 28 | P1.03 | Matrix output |
| `-LEDPWR` | 23 | 29 | P1.01 | Drives an external low-side MOSFET |
| `DRV11` | 24 | 30 | P1.02 | Matrix output |
| `KBD_BL_PWM` | 25 | 33 | P1.04 | Direct 1 kHz, 0-3.0 V push-pull PWM; no external pull or level shifter |
| `DRV14` | 26 | 34 | P1.06 | Matrix output |
| `DRV12` | 28 | 47 | P1.07 | Matrix output |
| `DRV15` | 30 | 48 | P1.05 | Matrix output |
| `DRV13` | 32 | 49 | P0.24 | Matrix output |
| `-LED_MUTE` | 33 | 50 | P0.20 | Drives an external low-side MOSFET |
| `-LEDMICMUTE` | 36 | 51 | P0.17 | Drives an external low-side MOSFET |
| `TP4DATA` | 37 | 52 | P0.15 | UARTE RX and GPIO, internal 3.0 V pull-up |
| `TP4CLK` | 39 | 53 | P0.14 | GPIO edge interrupt, internal 3.0 V pull-up |
| `TP4_RESET` | 40 | 54 | P0.13 | Drives an open-drain reset MOSFET |

Reserved module connections:

| Holyiot pad | Signal | Use |
| ---: | --- | --- |
| 1, 25, 37 | GND | Ground plane |
| 14 | `VDD-nRF` | Regulated module supply |
| 21 | P0.18/nRESET | Reset button and TC2050 reset |
| 22 | VBUS | USB VBUS detection only |
| 23 | USB D- | USB connector |
| 24 | USB D+ | USB connector |
| 31 | SWDCLK | TC2050 programming footprint |
| 32 | SWDIO | TC2050 programming footprint |
| 38 | P1.14 | Spare/test pad; USB input current is fixed by SY6280 hardware |
| 39 | P1.12 | `NRF_BL_ENABLE`, active-high drive for the BSS138/P-MOS backlight switch |
| 40 | P0.25 | `BLE_PROFILE_LED_1`, active-low current sink |
| 41 | P0.11 | `BLE_PROFILE_LED_2`, active-low current sink |
| 42 | P1.08 | `BLE_PROFILE_LED_3`, active-low current sink |
| 35-36, 43-46, 55 | Spare GPIOs | Test pads or future expansion; pads 35-36 are P0.09/P0.10 and are not used for NFC |

This design does not support NFC. Holyiot pads 35 and 36 expose P0.09 and
P0.10 as spare GPIOs alongside pads 43-46 and 55. They may remain unconnected
or reach labeled test pads. Before assigning either pin in firmware, enable
`CONFIG_NFCT_PINS_AS_GPIOS=y` so the nRF52840 configures them as digital I/O
instead of its default NFC function.

## Internal pull-up policy

The firmware must explicitly configure internal nRF pull-ups on:

- `SENSE0` through `SENSE7`;
- Fn (`-HOTKEY`);
- the power button (`-PWRSWITCH`); and
- the `VCC` sides of TrackPoint DATA and CLOCK.

Do not place external pull-up resistors on these nRF GPIOs. Matrix `DRV` lines,
PWM, reset-drive, and LED-drive pins are outputs and do not need pull-ups.

The 5 V TrackPoint bus is a separate electrical domain. Its high-side pull-ups
are required because the nRF's internal pull-ups cannot pull a BSS138's 5 V
side high.

## TrackPoint level conversion

### DATA and CLOCK

Use one BSS138 for DATA and one for CLOCK in the usual bidirectional,
open-collector arrangement:

```text
                          4.7 kΩ
                            │
                           +5V
                            │
nRF GPIO ── source  BSS138  drain ── J7 DATA or CLOCK
 internal       gate │
 pull-up             └────────────── VCC
```

- BSS138 source: `VCC`/nRF side.
- BSS138 drain: 5 V/TrackPoint side.
- BSS138 gate: `VCC`.
- High-side pull-up: 4.7 kΩ to 5 V on each signal.
- Low-side pull-up: nRF internal pull-up only.
- Place both MOSFETs and their high-side resistors close to J7 pins 37 and 39.

### RESET

Use a third BSS138 as a unidirectional open-drain reset driver:

```text
              +5 V
                │
              10 kΩ
                │
J7 TP4_RESET ─ drain  BSS138  source ─ GND
                         gate
                           │
                     nRF P0.13
                           │
                     100 kΩ to GND
```

GPIO high turns the MOSFET on and asserts the active-low 5 V reset. The 100 kΩ
gate pull-down keeps reset released while the MCU is unpowered or starting.
The Holyiot devicetree will need the correct reset polarity for this circuit.

## Indicator LEDs

The T430 power, mute, and microphone-mute LEDs originally used the keyboard's
3.3 V rail, not its 5 V rail. Revision A supplies them from the nominal 3.0 V
logic rail. They do not require voltage translation. Low-side MOSFETs are still
recommended to isolate the nRF and reproduce the original motherboard's
current-sinking behavior. Confirm acceptable brightness at 3.0 V during
bring-up before freezing the LED resistor values.

Use three additional BSS138s:

```text
J7 pin 35, 3.0 V nominal
        │
 keyboard's internal LED
        │
      J7 LED pin
        │
 original series resistor
        │
 BSS138 drain
 BSS138 source ───────── GND
 BSS138 gate ─────────── nRF GPIO
        │
      100 kΩ
        │
       GND
```

| LED | J7 signal/pin | Series resistor | Proposed GPIO |
| --- | --- | ---: | --- |
| Power/connectivity | `-LEDPWR`, pin 23 | 220 Ω | P1.01 |
| Speaker mute | `-LED_MUTE`, pin 33 | 3.9 kΩ | P0.20 |
| Microphone mute | `-LEDMICMUTE`, pin 36 | 3.9 kΩ | P0.17 |

The resistors reproduce R13, R41, and R44 in the T430 motherboard schematic.
With MOSFET gate drive, GPIO high means LED on; the preliminary devicetree must
therefore use active-high GPIO semantics rather than the DK's current
active-low direct-drive semantics.

### BLE-profile indicator LEDs

Place three adjacent 0805 top-view LEDs where they are visible to the user and label
them `1`, `2`, and `3` on the silkscreen. Consecutive Holyiot pads 40-42 keep
their routes parallel and avoid crossing the matrix fan-out.

Use a direct, active-low connection for each LED:

```text
VCC ── 4.7 kΩ ── LED anode
                      LED cathode ── Holyiot GPIO
```

| Profile | Holyiot pad | GPIO | Firmware behavior |
| ---: | ---: | --- | --- |
| 1 | 40 | P0.25 | Selected BLE profile 1 |
| 2 | 41 | P0.11 | Selected BLE profile 2 |
| 3 | 42 | P1.08 | Selected BLE profile 3 |

- Use NATIONSTAR NCD0805O1 orange LEDs (`C84262`) with 4.7 kOhm initial series
  resistors. Do not substitute white LEDs on the 3.0 V rail without rechecking
  forward-voltage headroom and resistor values. Keep footprints compatible
  with 2.2-10 kOhm for brightness tuning.
- Do not use BSS138s, 5 V, or external pull resistors on these lines.
- A GPIO low turns its LED on; a GPIO high or high-impedance state leaves it
  off. The Holyiot devicetree must therefore mark all three as active-low.
- If the PCB sits below an opaque enclosure surface, align an optional 2.5 mm
  end-glow PMMA light pipe over each LED. Keep its polished input end centered
  within 0.5 mm of the LED, lightly frost the visible end, and isolate adjacent
  LEDs with opaque wells. Measure the purchased pipe before fixing the case-hole
  diameter.
- Fast blink means the selected profile is empty and advertising. A 120 ms
  pulse every two seconds means it is bonded but disconnected. Solid for 2.5
  seconds identifies a newly selected or newly connected profile, then turns
  off while the connection remains healthy.
- When USB is selected, a profile-change command may identify its BLE slot for
  2.5 seconds; otherwise all three profile LEDs remain off.

The LEDs are logic-rail loads, not connectivity-critical pull-ups. Omitting
them or marking them do-not-populate does not affect BLE operation.

## Backlight

The original T430 motherboard separates backlight power from its control
signal. On schematic sheet 62, the 3.3 V MEC1619 embedded controller drives
`KBD_BL_PWM` directly from `GPIO153/LED2`. Sheet 63 routes that net directly to
J7 pin 25 with no level shifter. The 5 V backlight supply reaches J7 separately
on pins 29 and 31.

Use this preliminary circuit. The PWM pin controls brightness while a simple
P-channel MOSFET high-side switch removes backlight power when it is off. The
BSS138 is a gate pull-down/level interface, not an LED driver:

```text
Holyiot P1.04 ─────────────── J7 pin 25 KBD_BL_PWM

Holyiot P1.12 ── 1 kΩ ── BSS138 gate
                              │
                            100 kΩ
                              │
                             GND

                       100 kΩ
+5V ────────────────/\/\/──┐
                              │
                    10 nF     │       YJL3401A P-MOS
+5V ──────────────||────── gate ─── source: +5V
                              │         drain
                         BSS138 drain     │
                         source: GND      └── BL_5V ── J7 pins 29 and 31
```

- Supply 5 V to both J7 pins 29 and 31 with suitably wide traces.
- Drive J7 pin 25 only with 0-3.0 V PWM from P1.04.
- Match the original motherboard topology: the PWM path is a direct push-pull
  logic connection with no series resistor or external pull-down. The separate
  high-side supply switch keeps the backlight unpowered during GPIO startup.
- Do not put a BSS138 or a pull-up to 5 V in the PWM path. Translating the PWM
  high level to 5 V would exceed the level used by the original motherboard.
- Place local bulk and high-frequency decoupling near J7. Start with 22 µF plus
  100 nF on the 5 V backlight rail and adjust after measuring inrush/noise.
- Fit 100 kΩ from P-MOS gate to source so `BL_5V` defaults off. Fit 10 nF from
  gate to source as a preliminary slew capacitor and tune it after measuring
  inrush. Do not claim a precise rise time until the MOSFET gate charge and
  assembled load have been measured.
- Firmware must assert `NRF_BL_ENABLE` before generating nonzero PWM, wait at
  least 35 ms, and set PWM to zero before deasserting `NRF_BL_ENABLE`.
- The known T430 assembly makes `-KBD_BL_DTCT` on J7 pin 21 unnecessary; leave
  it unconnected unless automatic keyboard-type detection is added later.

Before connecting P1.04 during first bring-up, power the keyboard's 3.0 V and
5 V rails with J7 pin 25 disconnected and measure pin 25 relative to ground.
It must not rise toward 5 V. This verifies that the particular keyboard,
including an aftermarket replacement, does not contain an unexpected 5 V
pull-up. Then connect the direct PWM signal and confirm that its high level
remains at or below the Holyiot supply.

## BSS138 quantity and supporting resistors

| Purpose | BSS138 quantity |
| --- | ---: |
| TrackPoint DATA and CLOCK translators | 2 |
| TrackPoint RESET open-drain driver | 1 |
| Power, mute, and microphone-mute LED sinks | 3 |
| **Total fitted** | **6** |

Buy at least 10; buying 20 is reasonable for prototypes and rework.

Use IEC E12/E24 values only. (`E10` and `E20` are not standard IEC resistor
series.) Prefer isolated four-resistor `0603x4` arrays for repeated values:

| Array/value | Packages | Elements used | Purpose |
| --- | ---: | ---: | --- |
| 4 x 4.7 kOhm | 1 | 2 | TrackPoint DATA/CLOCK 5 V pull-ups; two unused elements are NC |
| 4 x 4.7 kOhm | 1 | 3 | BLE-profile LEDs; one unused element is NC |
| 4 x 100 kOhm | 2 | Up to 8 | MOSFET gates and control defaults |

The arrays must contain four independent resistors in an eight-pad body. Do
not substitute bussed/common-terminal networks. Keep the USB-C 5.1 kOhm
resistors, converter feedback divider, and charger programming resistor as
individual 0603 parts for inspection and tuning.

## USB-C

Use Korean Hroparts `TYPE-C-31-M-12` (`C165948`), a right-angle 16-contact
USB 2.0 receptacle. Use its exact manufacturer land pattern: the signal pads
are SMD and the four shell stakes pass through the PCB. Put it on the board
edge with the shell opening and board-edge setback checked against the
mechanical drawing; do not substitute a visually similar Type-C footprint.

| USB-C function | Holyiot connection |
| --- | --- |
| VBUS | Module pad 22 for USB detection; also feed the protected power path |
| D- | Module pad 23 |
| D+ | Module pad 24 |
| Ground/shield | Ground plane using the selected shield/ESD strategy |

Include:

- one 5.1 kΩ pull-down from CC1 to ground;
- one 5.1 kΩ pull-down from CC2 to ground;
- a low-capacitance USB ESD array immediately beside the connector;
- a fuse or current-limited load switch on VBUS; and
- short, parallel D+/D- traces over uninterrupted ground.

Do not connect USB VBUS directly to Holyiot `VDD-nRF`. Module pad 14 must be
powered through the board's nominal 3.0 V rail.

## SWD programming with TC2050

Use the official Tag-Connect `TC2050-NL` target footprint. For a direct
one-to-one connection to the nRF52840 DK's 0.050-inch P19 header, use the
[TC2050-IDC-NL-050-ALL](https://www.tag-connect.com/product/tc2050-idc-nl-050-all)
cable. The ordinary `TC2050-IDC-NL` terminates in a 0.1-inch connector and
requires an adapter.

Wire the target footprint one-to-one with Nordic P19:

| TC2050 pad | DK P19 signal | preliminary PCB connection |
| ---: | --- | --- |
| 1 | `SWD0_VTG` | Regulated target VDD at module pad 14 |
| 2 | `SWD0_SWDIO` | Holyiot pad 32, SWDIO |
| 3 | `SWD0_SELECT` | Ground; selects the DK's external target |
| 4 | `SWD0_SWDCLK` | Holyiot pad 31, SWDCLK |
| 5 | GND | Ground |
| 6 | SWO | Leave unconnected; RTT does not use SWO |
| 7 | NC | Leave unconnected |
| 8 | NC | Leave unconnected |
| 9 | GNDDetect | Ground; lets the debugger detect a valid target-ground connection |
| 10 | `SWD0_RESET` | Holyiot pad 21, P0.18/nRESET |

Nordic documents the P19 selection and power behavior in
[Programming an external board](https://docs.nordicsemi.com/r/bundle/ug_nrf52840_dk/page/ug/dk/ext_programming_support_p19.html).

TC2050 layout requirements:

- use the official footprint dimensions and non-plated locating holes;
- mark pad 1 clearly on silkscreen;
- put the footprint somewhere accessible after assembly;
- orient the cable away from the RF antenna and J7 flex cable;
- keep components out of the probe and retaining-clip area; and
- use the `TC2050-CLIP` for extended debugging with the no-legs cable, or hold
  it in a preliminary fixture for short programming operations.

Use SHOU HAN `TS24CA` (`C393942`) as the reset pushbutton from P0.18 to ground.
It is a side-actuated, momentary SPST-NO switch; place its actuator at an
accessible board edge and use the exact drawing footprint. It closes only
while pressed and must not be used as the main-power latch. The Holyiot board
configuration must enable `CONFIG_GPIO_AS_PINRESET=y`; the current DK build
does not enable that option.

### Debug-target power rules

Prefer to power the preliminary PCB normally and connect its regulated VDD to
TC2050 pad 1 for target detection/reference. Do not configure P19 to source
power at the same time.

The DK normally runs at 3.0 V when USB-powered, and Nordic says the external
target voltage must match. During bring-up, either operate the module rail at
3.0 V or configure the DK/reference power arrangement for the target voltage.
Do not short DK solder bridge SB47 while the target is independently powered.

SEGGER RTT is the preferred bring-up log path. It operates through SWD and does
not consume SWO or a UART.

## Power architecture

### Top-level power distribution plan

The proof-of-concept board uses nine named power domains. Keep these names in
the schematic, PCB, test pads, and bring-up notes:

| Net | Source | Consumers | Normal range / limit |
| --- | --- | --- | --- |
| `BAT_RAW+` | J3 JST-PH or alternative solder holes | F2 only | Protected 1S LiPo, 3.7 V nominal/4.2 V maximum; Adafruit/Feather polarity |
| `BAT_PROTECTED` | F2 output, clamped against reverse polarity by D6 | LNK_BAT and ETA6002 BATT | Bidirectional charge/discharge path, at least 1.2 A design current |
| `USB_VBUS_RAW` | USB-C J1 through F1, shunt-clamped by PTVS5V0S1UR | Holyiot VBUS-detect pad 22 and U4 input | Real cable VBUS only, nominal 5 V |
| `USB_5V_LIM` | SY6280AAC U4 output | ETA6002 U1 input | Approximately 453 mA nominal limit |
| `VSYS` | ETA6002 SYS output, supplemented by battery | YJL3401A system high-side P-MOSF | Approximately 3.6-4.5 V while USB-powered; tracks battery through the power path otherwise |
| `VSYS_SW` | System high-side P-MOSF output | ME6211 U2 and SY7069 U3 | Switched system supply; rated for at least 1.2 A |
| `VCC` | ME6211C30 U2 from `VSYS_SW` | Holyiot VDD, keyboard logic/LED rail, low sides of translators | Regulated 3.0 V |
| `+5V` | SY7069ADC U3 from `VSYS_SW` | TrackPoint branch and backlight switch input | Approximately 4.96 V nominal; validate on the first board |
| `BL_5V` | YJL3401A Q8 | J7 pins 29 and 31 | Switched `+5V`; normally off |

```text
                                      ┌── Holyiot pad 22: VBUS detect only
USB-C VBUS ─ F1 ─┬─ USB_VBUS_RAW
                  └─ PTVS5V0S1UR ─ GND
                                      └── SY6280AAC, ~453 mA limit ─ USB_5V_LIM
                                                                          │
                                                                          v
Protected 1S LiPo ─ J3 ─ F2 ─ BAT_PROTECTED ─ LNK_BAT ─ BATT ┌──────────┐ SYS ─ VSYS ─ system P-MOS ─ VSYS_SW ─┬── ME6211C30 ─ VCC
                                  │                           │ ETA6002  │                            │                  ├─ Holyiot VDD
                             D6 cathode                        │ charger  │                            │                  └─ keyboard logic/LEDs
                                  │                            │ + power  │                            │
                            D6 anode: GND                      │   path   │                            └── SY7069 ─ +5V ─┬─ TrackPoint
USB_5V_LIM ─────────────────────────────────────────────── IN │          │                                                 └─ YJL3401A ─ BL_5V ─ backlight
                                                              └──────────┘
```

All domains share the PCB ground plane. Never switch the keyboard ground; the
matrix, TrackPoint, LEDs, USB, charger, and regulators require a common signal
reference.

### Operating states

| State | Power flow | Required behavior |
| --- | --- | --- |
| USB absent, battery present | `BAT -> ETA6002 power path -> VSYS` | With the latch on, VCC and +5V operate from the cell; backlight remains independently switchable |
| USB present, battery present | `USB_VBUS_RAW -> SY6280 -> ETA6002`; battery charges or supplements VSYS | Total USB draw is limited near 453 mA; battery supplies load beyond the available USB input budget |
| USB present, battery absent/dead | USB supplies ETA6002 VSYS instant-on path | Logic should boot; maximum 5 V/backlight load is constrained by U4 and converter losses |
| Both absent | No powered rail | No rail may be held up through GPIO, USB data, SWD, or protection-diode backfeed |

### Physical switch roles

| Physical control | Type | Connected function | User-visible result |
| --- | --- | --- | --- |
| SHOU HAN `TS24CA` | Momentary SPST-NO | Holyiot P0.18/`nRESET` to GND | Restarts the controller while held; power remains on |
| XKB `XKB5858-Z-E` | Latching DPDT, top-actuated | One pole drives the system P-MOSFET gate through 10 kOhm; the other pole is unused | Holds `VSYS_SW` on or off after the button is released |
| T430 `-PWRSWITCH` button | Separate momentary keyboard contact | Holyiot GPIO/firmware input | Toggles USB/BLE output or performs the configured key behavior; it does not switch a supply rail |

The YJL3401A system high-side P-MOSFET is downstream of ETA6002 SYS. Turning
the XKB5858-Z-E latch off therefore removes power from both regulators while
leaving the battery connected to the charger, so USB can charge the cell with
the keyboard off. The latch carries gate current only: connect its common to
the P-MOSFET gate through 10 kOhm, its on throw to ground, and leave its off
throw unconnected. Leave every contact of its second pole unconnected. The
datasheet shows the DPDT contact arrangement, but confirm the common and the
throw closed in the desired latched-ON state with a continuity meter before
assigning PCB pads. A 100 kOhm gate-to-source pull-up defaults the system off;
fit 100 nF gate-to-source to slow contact-bounce and inrush edges. The nominal
RC time constants are about 1 ms turning on and 10 ms turning off, but MOSFET
threshold and load make the actual rail ramp nonlinear. Put neither the
P-MOSFET nor the latch in series with the battery lead, because that would
disable off-state charging. Do not add an LC network: there is no inductive
latch load, and an underdamped LC can create the ringing it is intended to
prevent.

`USB_VBUS_RAW` must not be joined to `+5V`. The former indicates an actual
host cable and feeds the charger; the latter is a locally boosted battery/SYS
rail. This separation lets firmware distinguish USB attachment and prevents
the boost converter from falsely asserting VBUS.

### USB input and protection

J1 is a USB-C sink with individual 5.1 kOhm `Rd` resistors from CC1 and CC2 to
ground. Route VBUS through the resettable fuse to `USB_VBUS_RAW`. Shunt that
net to ground with a Nexperia `PTVS5V0S1UR,115` (`C478011`): cathode to VBUS,
anode to ground, using a short wide ground return. This TVS protects against
transient energy; its 9.2 V specified high-current clamp is not a precision
5 V overvoltage regulator. Validate leakage and temperature with a
current-limited 5.5 V input on the first board. Protect D+/D- with the TECH
PUBLIC `USBLC6-2SC6` beside J1 and route the pair directly to Holyiot D-/D+.

Place the SY6280AAC between `USB_VBUS_RAW` and ETA6002 IN:

- IN = `USB_VBUS_RAW`, OUT = `USB_5V_LIM`;
- active-high EN tied to IN so it never floats;
- 15 kOhm, 1% from ISET to ground, giving about 453 mA nominal from
  `I_LIM = 6800/R_SET`;
- local input/output bypass as required by the Silergy data sheet; and
- test pads on both sides to verify drop, limiting, reverse blocking, and
  shutdown discharge.

The 453 mA setting deliberately leaves margin below 500 mA. Its tolerance must
be measured on the first boards. This is a fixed hardware ceiling; P1.14 and
USB enumeration no longer control charge-current mode.

### Charger and dynamic power path

J3 is a genuine JST `S2B-PH-SM4-TB(LF)(SN)` wired for Adafruit/Feather
polarity. The connector and nearby rear silkscreen must say `1S LiPo ONLY`,
`4.2 V MAX`, `RED +`, and `BLACK -`. JST-PH prevents rotated insertion but does
not standardize which wire is positive, so these labels remain necessary.

Protect the bidirectional battery connection as follows:

```text
J3 red/BAT+ ─ BAT_RAW+ ─ F2, 2 A fast fuse ─ BAT_PROTECTED ─ LNK_BAT ─ ETA6002 BATT
J3 black/GND ────────────────────────────────────────────────────────── GND

                                      BAT_PROTECTED
                                            │
                                      cathode D6 SS34
                                      anode   │
                                            GND
```

Correct polarity reverse-biases D6, leaving only the fuse resistance in the
normal charge/discharge path. Reverse polarity forward-biases D6 and makes F2
or the protected pack open the fault instead of applying a negative voltage to
the charger. Do not substitute a conventional single-P-MOS reverse protector:
that common load-only circuit can turn back on when USB powers a charger.
Qualify the fuse/diode combination with a current-limited battery emulator both
before and after attaching USB. F2 is replaceable and must be inspected after
any polarity fault.

Place D6 and F2 directly beside J3 with short, wide copper. Add `BAT_RAW+`,
`BAT_PROTECTED`, and paired ground test pads. Optional large plated BAT+/GND
solder holes may bypass the connector for an expert-wired pack, but they feed
`BAT_RAW+` ahead of F2 and must never be used at the same time as J3.

ETA6002E8A is the only populated charger/power-path implementation:

- IN from `USB_5V_LIM`;
- BATT to `BAT_PROTECTED` through `LNK_BAT`; the system switch is not in the battery lead;
- SYS to `VSYS`, never substitute BATT for this connection;
- cjiang `FXL0420-2R2-M` 2.2 uH molded inductor from SW to SYS, rated 4.5 A
  with 5 A saturation current; use short, wide high-current routing;
- 10 uF at IN, 22 uF at SYS, and 1 uF at BATT, placed as the reference circuit
  requires;
- 5.1 kOhm ISET for approximately 196 mA nominal fast charge; and
- use the battery thermistor where available. A fixed in-range divider is a
  diagnostic fallback, not thermal protection.

The ETA6002 regulates SYS and dynamically connects the battery when input
power is insufficient. This is why the backlight may use battery supplement
while USB remains capped by the SY6280. Validate approximately 196 mA charge
current because the ETA6002 data sheet only characterizes higher example
currents explicitly.

### VCC logic distribution (nominal 3.0 V)

ME6211C30M5G-N generates `VCC` from `VSYS_SW`:

- 1 uF input and output capacitors immediately beside U2;
- EN pulled high to VSYS with a 100 kOhm array element unless a later power
  sequencing requirement is demonstrated;
- feed Holyiot VDD/module pad 14, T430 J7 pin 35, profile LEDs, and the `VCC`
  translator gates from this rail; and
- provide `LNK_VCC` so all logic-rail current can be measured.

The expected logic load is far below the regulator's 500 mA headline rating.
Test output droop and BLE radio transients down to the intended battery cutoff.

### Regulated 5 V distribution

SY7069ADC generates `+5V` from `VSYS_SW`:

- tie EN to `VSYS_SW`, so the system high-side switch controls the converter and its shutdown output
  disconnect prevents the 5 V rail from being held up through U3;
- cjiang `FXL0420-1R5-M` 1.5 uH molded inductor, rated 5 A with 6 A saturation;
- no external rectifier diode: SY7069 is synchronous;
- 470 kOhm from OUT to FB and 150 kOhm from FB to ground, for approximately
  4.96 V nominal from the 1.2 V reference;
- 22 uF input and two 22 uF output ceramics, with effective capacitance checked
  at operating bias; and
- compact VIN/SW/OUT current loops, with feedback kept away from SW and
  shielded by ground where practical.

SY7069's 3 A figure is its minimum valley-current limit, not an output-current
rating. Treat 550 mA at 5 V as an unproven design target until
the assembled PCB passes load, startup, ripple, and thermal tests from a 3.2 V
input. Its data sheet shows operation at 500 mA and 1 A from 3 V, but does not
guarantee either as a production output-current rating.

Split `+5V` into two measured branches:

```text
+5V ── LNK_TP ── TP_5V ───────────── J7 pin 38 TrackPoint
       └─ LNK_BL ── Q8 P-MOS ── BL_5V ─ J7 pins 29 and 31 backlight
```

The TrackPoint branch remains powered whenever +5V is enabled. Q8 provides
high-side backlight supply gating; `KBD_BL_PWM` remains the independent 3.0 V
brightness-control signal.

### Provisional current and converter budget

Until the actual T430 assembly is measured, use the following design values.
The expected values are battery-life estimates; the larger design values are
what the regulator, inductor, battery protection, connectors, and copper must
survive without excessive voltage droop or heating.

| Load | Expected operating current | Provisional design allowance |
| --- | ---: | ---: |
| TrackPoint, idle | **Measured 5.94 mA at 5 V** | 10 mA continuous |
| TrackPoint, continuous movement | **Measured 6.47 mA at 5 V** | 10 mA continuous |
| TrackPoint sensing/startup peak | **Measured 37.1 mA** | 50 mA peak |
| Holyiot, matrix, and BLE, release firmware | Approximately 0.5-2 mA at 3.0 V, excluding LEDs | 25 mA peak |
| Selected BLE-profile LED | Approximately 0.2 mA while illuminated | 0.5 mA peak |
| Backlight | Provisionally 100-300 mA at 5 V | 500 mA continuous |
| Complete 5 V rail | Load-dependent | 550 mA continuous |

The TrackPoint values were measured with a PPK2 at 5.0 V and include the
high-side translator/pull-up rail. The two 4.7 kOhm PS/2 pull-ups can add
approximately 1.06 mA each while their signals are held low. The 10 kOhm reset
pull-up adds 0.5 mA only while reset is asserted. These loads are included in
the measured waveform and the 50 mA peak allowance.

At a nominal 3.7 V battery and 85% boost efficiency, the battery current caused
by a 5 V load is approximately:

```text
I_battery = (5 V * I_5V) / (3.7 V * 0.85) = 1.59 * I_5V
```

The measured TrackPoint therefore contributes approximately 9.4 mA battery
current while idle and 10.3 mA during movement. Until the complete 3.0 V branch
is measured, use 12-15 mA as the backlight-off whole-keyboard planning range.
A 1000 mAh battery should be treated as roughly a 55-70 hour ordinary-use
battery after practical capacity and conversion-loss allowances.

The first PCB revision must support at least 550 mA continuous at 5 V and at
least 1.2 A through the battery-side power path. Low-battery SY7069 output,
inductor/IC temperature, ripple, and the measured backlight load still
require validation. A switch-current rating must not be mistaken for
guaranteed 5 V output current.

The connectivity LED must normally be off or use short low-duty-cycle pulses.
A continuously lit power LED can consume several milliamps and materially
reduce battery life.

### Current-measurement provisions

Add labeled removable links and test pads so the Power Profiler Kit II (PPK2)
can be inserted without cutting PCB traces:

```text
+5V -- LNK_TP -- TP_5V  -------- J7 pin 38
       `- LNK_BL -- Q8 -- BL_5V  -- J7 pins 29 and 31

BAT_RAW+ -- F2/D6 protection -- BAT_PROTECTED -- LNK_BAT -- ETA6002 BATT
VCC    -- LNK_VCC -- Holyiot and keyboard-logic/LED branch
```

- Use an 0805 0 Ohm link or a solder-bridge-plus-test-pad arrangement for
  `LNK_TP`, `LNK_BL`, and `LNK_VCC`.
- Make `LNK_BAT` and its pads suitable for at least 1.5 A and for attaching
  probes without stressing a small passive footprint.
- Put a ground test pad beside each measurement pair.
- Keep the TrackPoint and backlight on separate branches even if both originate
  at the same boost converter.

The [Nordic PPK2 documentation](https://docs.nordicsemi.com/r/bundle/ug_ppk2/page/ug/ppk/ppk_user_guide_intro.html)
specifies 0.8-5.0 V measurement, 100 ksps sampling, up to 1 A in ampere-meter
mode, and 600 mA continuous in source mode. Use it as follows:

1. **TrackPoint alone:** open `LNK_TP`; use PPK2 source mode at 5.0 V between
   `TP_5V` and ground. Power the nRF/keyboard separately, join grounds, and make
   certain no other supply is driving `TP_5V`.
2. Record short, stable windows covering power-on reset and initialization,
   idle, continuous circles in both directions, and every TrackPoint button.
   At 100 ksps, sub-second steady-state windows already contain many sensing
   cycles; use longer captures only to investigate retries, power-state changes,
   disconnects, or thermal drift.
3. Use a PPK2 digital input on `TP4_RESET`, or on a temporary firmware marker
   GPIO, to align reset/initialization events with current peaks. Continue to
   use the Saleae for detailed PS/2 DATA/CLOCK decoding.
4. **3.0 V subsystem:** open `LNK_VCC` and profile the release firmware at 3.0 V
   during advertising, BLE connected idle, typing, TrackPoint reporting, and
   USB-suspended idle. Compare it with the debug build so logging overhead is
   not mistaken for product consumption.
5. **Whole board on battery:** open `LNK_BAT` and use PPK2 ampere-meter mode in
   series with a protected external battery supply. Disconnect USB so the
   charger cannot bypass or back-feed the measurement path.
6. **USB operation/charging:** measure USB VBUS separately. Do not infer charge
   current or USB load from the battery measurement while the power path is
   selecting between both sources.
7. Do not use PPK2 source mode for the maximum-backlight test if the load can
   exceed its 600 mA continuous source rating. Use a current-limited bench
   supply and a suitable shunt/current probe instead. Do not exceed the PPK2's
   1 A ampere-meter limit.

First-build acceptance limits are TrackPoint current no more than 10 mA
continuous and 50 mA peak at 5 V, complete no-backlight battery current no more
than 20 mA during ordinary connected idle, and no unexplained reset-correlated
current spikes. A result above a limit is not automatically a faulty keyboard,
but it must be understood before freezing the boost converter or battery size.

## Suspend, idle, and host wake behavior

The initial preliminary firmware deliberately uses light CPU idle, not nRF52840
System OFF. Zephyr can idle the CPU between interrupts while the Bluetooth
controller maintains a connection or advertising. A matrix, power-button, or
TrackPoint interrupt wakes the CPU and ZMK immediately resumes reporting.

The shield configuration makes this policy explicit:

```text
CONFIG_USB_DEVICE_REMOTE_WAKEUP=y
CONFIG_ZMK_SLEEP=n
```

`CONFIG_ZMK_SLEEP=n` does **not** mean the CPU runs continuously. It prevents
ZMK's longer idle timeout from entering System OFF, which would terminate BLE
and require a hardware wake followed by boot and reconnection. Reconsider
System OFF only after measuring the finished board's battery life and wake
latency.

### USB wake

ZMK v0.3 requests USB remote wake automatically when it has a HID report to
send while USB is suspended. The host must have enumerated this keyboard, kept
VBUS present, enabled wake for the USB device, and armed remote wake before it
suspended. BIOS/UEFI and operating-system settings can still prevent wake.

USB wake follows ZMK's selected output. If BLE is selected, normal key and
TrackPoint reports go to BLE and are not duplicated to the sleeping USB host.
Select USB before suspending when the wired PC must be woken. Supporting
simultaneous wake of an unselected USB host would require a deliberate custom
endpoint policy and is not part of this design.

The matrix, Fn, power-button, and TrackPoint inputs must remain interrupt
capable. The current shield already marks its composite key scanner as a
devicetree wake source. Connect module pad 22 only to real USB VBUS so the
firmware can distinguish attached USB power from the boosted internal 5 V rail.

### BLE wake

ZMK's BLE HID service advertises the HID RemoteWake capability. While BLE is
selected, a bonded and connected host can therefore receive the first input
after the keyboard CPU wakes from idle. Whether that input wakes the whole PC
depends on the host Bluetooth controller, its driver, firmware, OS power
policy, and whether the controller remains powered during sleep. A host that
turns its Bluetooth radio off cannot be woken by this keyboard; the keyboard
will reconnect when that host resumes.

For predictable behavior, validate every intended host/OS combination. Keep
the power/connectivity LED off or at a very low duty cycle during idle so it
does not dominate battery consumption.

## Firmware and build plan

Keep the working DK target and add a separate preliminary board target:

| Target | Purpose |
| --- | --- |
| `nrf52840dk_nrf52840 + thinkpad_t430` | Existing bench and TrackPoint testing |
| `holyiot_18010_nrf52840 + thinkpad_t430` | preliminary PCB |

The Holyiot board definition must provide:

- the preliminary GPIO assignment in this document;
- the module's low-frequency crystal configuration;
- USB device support and internal flash/settings partitions;
- P0.18 hardware reset;
- J-Link/SWD runner support;
- no DK buttons, DK LEDs, QSPI flash, or J-Link UART assumptions;
- UARTE reception plus GPIO clock interrupts for the TrackPoint;
- no firmware-controlled charger-current mode; P1.14 remains spare because
  SY6280 sets the USB input ceiling in hardware;
- P1.12 backlight-rail enable sequencing around the existing PWM control;
- active-low P0.25/P0.11/P1.08 outputs for BLE profile LEDs 1-3;
- interrupt-capable wake inputs for the matrix, Fn, power button, and
  TrackPoint, while retaining USB remote wake and connected BLE idle; and
- the existing TrackPoint interrupt-priority overrides regenerated from the
  compiled Holyiot devicetree.

Planned build variants:

| Build | Logging | Use |
| --- | --- | --- |
| Holyiot debug | SEGGER RTT | Initial PCB and TrackPoint bring-up |
| Holyiot USB log | ZMK `zmk-usb-logging` snippet | Routine testing without SWD attached |
| Holyiot release | Minimal or disabled debug logs | Normal keyboard use |

Initial and recovery programming will use SWD, approximately:

```sh
west flash -d build/thinkpad_t430_holyiot --runner jlink
```

The exact build directory and target name will be finalized when the board
definition is added. A Holyiot-specific UF2 or MCUboot configuration can be
added later for convenient USB updates, but SWD must remain accessible as the
guaranteed recovery path.

## PCB bring-up order

1. Inspect the unpowered PCB for shorts, particularly VBUS-to-VDD and 5 V-to-GPIO.
2. Apply only the regulated module supply; verify VDD and idle current.
3. Attach TC2050 and confirm the DK can identify and erase the nRF52840.
4. Flash a minimal Holyiot board test and confirm RTT logging and reset.
5. Test USB enumeration without the keyboard connected.
6. Connect only the passive matrix/Fn/power signals and verify all keys.
7. Open `LNK_TP`, use the PPK2 to supply and profile the TrackPoint 5 V branch,
   and capture reset, initialization, idle, movement, and all-button currents.
8. Connect TrackPoint DATA/CLOCK and repeat the existing movement/button tests.
9. Profile the isolated 3.0 V branch with debug logging and then release logging.
10. Test each indicator MOSFET before attaching the keyboard LEDs.
11. Verify profile LEDs 1-3 during profile selection, pairing, connection, and
    disconnection, including the all-off steady connected state.
12. Enable backlight power from a current-limited bench supply at minimum PWM
    duty, then measure 50% and 100% operation and startup inrush.
13. Pair and exercise all three BLE profiles.
14. Test USB/BLE switching, power cycling, bond persistence, and SWD recovery.
15. Select USB, suspend the wired host, and verify a key, power button, and
    TrackPoint activity wake it; note whether the first input is consumed.
16. On every supported BLE host, select its profile, suspend it for at least 30
    minutes, then verify key and TrackPoint wake/reconnect behavior and latency.
17. Profile the complete board through `LNK_BAT` during BLE idle/activity,
    advertising, wake, TrackPoint use, and each LED pattern.
18. Repeat the wake tests on battery power and confirm the keyboard remains
    responsive without an unexpected reboot or lost bond.

## Pre-fabrication checklist

- [ ] Purchased Holyiot pad layout checked against the 18010-A top-view drawing.
- [ ] J7 pin 1 orientation verified against the actual flex cable.
- [ ] Complete antenna keepout present on every copper layer.
- [ ] No 5 V net reaches an nRF GPIO or module VDD.
- [ ] All eight SENSE inputs rely on internal pull-ups.
- [ ] Fn and power inputs rely on internal pull-ups.
- [ ] TrackPoint low side relies on internal pull-ups; high side has 4.7 kΩ.
- [ ] Seven Yangjie BSS138 footprints fitted with correct source/drain orientation.
- [ ] LED current-limit and MOSFET gate pull-down resistors fitted.
- [ ] Three profile LEDs are labeled 1-3 and route through 4.7 kΩ from `VCC`
      to active-low P0.25, P0.11, and P1.08 respectively.
- [ ] `KBD_BL_PWM` is a direct 0-3.0 V push-pull signal with no series resistor,
      external pull-down, BSS138, or 5 V pull-up.
- [ ] J7 pin 25 was checked for an unexpected 5 V pull-up before connection to
      the Holyiot.
- [ ] USB CC resistors, ESD protection, and VBUS protection included.
- [ ] SY6280 is between raw VBUS and ETA6002 IN, uses 15 kOhm ISET, and its
      measured current ceiling remains below 500 mA across intended conditions.
- [ ] ETA6002 uses SYS, not BAT, as the board supply and charges near 196 mA
      with the selected 5.1 kOhm ISET resistor.
- [ ] ETA6002 and SY7069 discrete power stages follow their reference layouts;
      there are no daughter modules or alternate parallel regulator paths.
- [ ] YJL3401A backlight switch defaults off through a 100 kOhm gate-source
      pull-up, uses the preliminary 10 nF slew capacitor, and is driven by a
      BSS138 from P1.12 separately from the P1.04 brightness PWM.
- [ ] SWDIO, SWDCLK, reset, target VDD, and ground reach the TC2050 footprint.
- [ ] TC2050 pad 3 is grounded so DK P19 selects the external target, and pad
      9 (`GNDDetect`) is also grounded.
- [ ] Reset button and accessible VDD/GND test points included.
- [ ] `LNK_TP`, `LNK_BL`, `LNK_VCC`, and high-current `LNK_BAT` measurement
      points included and labeled, each with a nearby ground point.
- [ ] Holyiot operation, matrix scanning, LEDs, USB, and BLE are validated on
      the nominal 3.0 V rail down to the intended low-battery endpoint.
- [ ] USB VBUS detection is isolated from any boosted 5 V system rail.
- [ ] USB remote wake works with the intended BIOS/UEFI and OS settings.
- [ ] BLE wake behavior is verified on every intended host/OS combination.
- [ ] Idle current and 30-minute BLE response latency are measured on battery.
- [ ] J3 is genuine 2-pin JST-PH with Adafruit/Feather polarity and explicit
      red-positive, black-negative, 1S, and 4.2 V maximum markings.
- [ ] F2/D6 reverse-battery protection is tested using a current-limited
      emulator with USB both absent and present; F2 is replaced after tripping.
- [ ] TrackPoint stays within, or any deviation explains changes to, the
      measured 6.47 mA active, 10 mA continuous-design, and 50 mA peak budgets.
- [ ] Backlight current and inrush were measured without exceeding PPK2 limits.
- [ ] Battery, charger, regulator, and 5 V boost current budgets finalized.
- [ ] 5 V module passed 50/300/600 mA load tests at 3.2 V input without
      excessive droop, ripple, heat, or automatic low-load shutdown.
- [ ] No green or unintended always-on LED is fitted or enabled.
