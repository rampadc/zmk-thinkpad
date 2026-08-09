# T430 production PCB wiring and design plan

This document plans the custom PCB that will replace the nRF52840 DK with a
Holyiot 18010 V1.0 module. It covers the T430 keyboard connector, TrackPoint,
backlight, indicator LEDs, USB, power, SWD programming, and the corresponding
firmware/build work.

The current component shortlist and purchasing assumptions are in the
[preliminary production PCB BOM](preliminary-bom.md).

> **Status:** this is the proposed production layout. It has not yet been
> applied to the firmware. The current `thinkpad_t430.overlay` still contains
> the tested nRF52840 DK pin assignment. Do not route a production PCB from the
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

The production plan uses the newer **18010-A top-view numbering** shown in the
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
| Holyiot `VDD-nRF`, module pad 14 | 3.0 V nominal | Never connect directly to USB or 5 V |
| T430 J7 pin 35 | 3.0 V nominal | Original rail was 3.3 V; supplies the indicator LEDs |
| T430 J7 pins 29 and 31 | 5 V | Keyboard backlight supply |
| T430 J7 pin 38 | 5 V | TrackPoint supply |
| T430 J7 pins 34 and 41-44 | Ground | Join to one solid PCB ground system |
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

## Proposed production GPIO assignment

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
| `KBD_BL_PWM` | 25 | 33 | P1.04 | 1 kHz, 3.0 V PWM through 1 kΩ; 100 kΩ pull-down at J7 |
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
| 38 | P1.14 | `CHG_USB500_EN`; low/boot = USB100, high after enumeration = USB500 |
| 39 | P1.12 | `BL_5V_EN`, active-high enable for the backlight load switch |
| 40 | P0.25 | `BLE_PROFILE_1_LED`, active-low current sink |
| 41 | P0.11 | `BLE_PROFILE_2_LED`, active-low current sink |
| 42 | P1.08 | `BLE_PROFILE_3_LED`, active-low current sink |
| 43-46, 55 | Spare GPIOs | Test pads or future expansion |

P0.09 and P0.10 remain reserved for NFC and are not needed by this design.

## Internal pull-up policy

The firmware must explicitly configure internal nRF pull-ups on:

- `SENSE0` through `SENSE7`;
- Fn (`-HOTKEY`);
- the power button (`-PWRSWITCH`); and
- the 3.0 V sides of TrackPoint DATA and CLOCK.

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
                           +5 V
                            │
nRF GPIO ── source  BSS138  drain ── J7 DATA or CLOCK
 internal       gate │
 pull-up             └────────────── 3.0 V
```

- BSS138 source: 3.0 V/nRF side.
- BSS138 drain: 5 V/TrackPoint side.
- BSS138 gate: 3.0 V.
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
With MOSFET gate drive, GPIO high means LED on; the production devicetree must
therefore use active-high GPIO semantics rather than the DK's current
active-low direct-drive semantics.

### BLE-profile indicator LEDs

Place three adjacent 0603 LEDs where they are visible to the user and label
them `1`, `2`, and `3` on the silkscreen. Consecutive Holyiot pads 40-42 keep
their routes parallel and avoid crossing the matrix fan-out.

Use a direct, active-low connection for each LED:

```text
3V0 ── 4.7 kΩ ── LED anode
                      LED cathode ── Holyiot GPIO
```

| Profile | Holyiot pad | GPIO | Firmware behavior |
| ---: | ---: | --- | --- |
| 1 | 40 | P0.25 | Selected BLE profile 1 |
| 2 | 41 | P0.11 | Selected BLE profile 2 |
| 3 | 42 | P1.08 | Selected BLE profile 3 |

- Use high-efficiency green, amber, or red LEDs with 4.7 kOhm initial series
  resistors. Keep footprints compatible with 2.2-10 kOhm for brightness tuning.
- Do not use BSS138s, 5 V, or external pull resistors on these lines.
- A GPIO low turns its LED on; a GPIO high or high-impedance state leaves it
  off. The Holyiot devicetree must therefore mark all three as active-low.
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

Use this production circuit. The PWM pin controls brightness while the load
switch removes backlight power when it is off:

```text
Holyiot P1.04 ── 1 kΩ ──┬── J7 pin 25 KBD_BL_PWM
                         │
                       100 kΩ
                         │
                        GND

Holyiot P1.12 ── 1 kΩ ── TPS22918 ON
                              │
                            100 kΩ
                              │
                             GND

5V_SYS ── TPS22918 ── BL_5V ── J7 pins 29 and 31
```

- Supply 5 V to both J7 pins 29 and 31 with suitably wide traces.
- Drive J7 pin 25 only with 0-3.0 V PWM from P1.04.
- Fit the 1 kΩ series resistor to limit fault/backfeed current and reduce edge
  ringing. It is not a voltage divider or level shifter.
- Fit the 100 kΩ pull-down on the J7 side of the series resistor so the
  backlight remains off while the nRF GPIO is high-impedance during boot.
- Do not put a BSS138 or a pull-up to 5 V in the PWM path. Translating the PWM
  high level to 5 V would exceed the level used by the original motherboard.
- Place local bulk and high-frequency decoupling near J7. Start with 22 µF plus
  100 nF on the 5 V backlight rail and adjust after measuring inrush/noise.
- Fit 10 nF from the TPS22918 `CT` pin to ground for a deliberately slow,
  roughly 20-30 ms rise at 5 V. Leave `QOD` floating on the first revision so the
  keyboard discharges the branch naturally; provide an unpopulated resistor
  footprint from `QOD` to `BL_5V` for later tuning.
- Firmware must assert `BL_5V_EN` before generating nonzero PWM, wait at least
  35 ms, and set PWM to zero before deasserting `BL_5V_EN`.
- The known T430 assembly makes `-KBD_BL_DTCT` on J7 pin 21 unnecessary; leave
  it unconnected unless automatic keyboard-type detection is added later.

Before connecting P1.04 during first bring-up, power the keyboard's 3.0 V and
5 V rails with J7 pin 25 disconnected and measure pin 25 relative to ground.
It must not rise toward 5 V. This verifies that the particular keyboard,
including an aftermarket replacement, does not contain an unexpected 5 V
pull-up. Then connect the PWM through the 1 kΩ resistor and confirm that its
high level remains at or below the Holyiot supply.

## BSS138 quantity and supporting resistors

| Purpose | BSS138 quantity |
| --- | ---: |
| TrackPoint DATA and CLOCK translators | 2 |
| TrackPoint RESET open-drain driver | 1 |
| Power, mute, and microphone-mute LED sinks | 3 |
| **Total fitted** | **6** |

Buy at least 10; buying 20 is reasonable for prototypes and rework.

Related resistor count:

| Value | Quantity | Purpose |
| ---: | ---: | --- |
| 4.7 kΩ | 2 | TrackPoint DATA/CLOCK 5 V pull-ups |
| 10 kΩ | 1 | TrackPoint RESET 5 V pull-up |
| 1 kΩ | 1 | `KBD_BL_PWM` series protection |
| 100 kΩ | 5 | RESET/LED MOSFET gates and `KBD_BL_PWM` boot-state pull-down |
| 220 Ω | 1 | Power LED current limiting |
| 3.9 kΩ | 2 | Mute and microphone-mute LED current limiting |

## USB-C

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

| TC2050 pad | DK P19 signal | Production PCB connection |
| ---: | --- | --- |
| 1 | `SWD0_VTG` | Regulated target VDD at module pad 14 |
| 2 | `SWD0_SWDIO` | Holyiot pad 32, SWDIO |
| 3 | `SWD0_SELECT` | Ground; selects the DK's external target |
| 4 | `SWD0_SWDCLK` | Holyiot pad 31, SWDCLK |
| 5 | GND | Ground |
| 6 | SWO | Leave unconnected; RTT does not use SWO |
| 7 | NC | Leave unconnected |
| 8 | NC | Leave unconnected |
| 9 | NC | Leave unconnected |
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
  it in a production fixture for short programming operations.

Add a reset pushbutton from P0.18 to ground. The Holyiot board configuration
must enable `CONFIG_GPIO_AS_PINRESET=y`; the current DK build does not enable
that option.

### Debug-target power rules

Prefer to power the production PCB normally and connect its regulated VDD to
TC2050 pad 1 for target detection/reference. Do not configure P19 to source
power at the same time.

The DK normally runs at 3.0 V when USB-powered, and Nordic says the external
target voltage must match. During bring-up, either operate the module rail at
3.0 V or configure the DK/reference power arrangement for the target voltage.
Do not short DK solder bridge SB47 while the target is independently powered.

SEGGER RTT is the preferred bring-up log path. It operates through SWD and does
not consume SWO or a UART.

## Power architecture

### USB-powered prototype

For a board that only needs to operate while USB is connected:

```text
USB VBUS ── protection ─┬─ 5 V TrackPoint/backlight rail
                        └─ 3.0 V regulator ─ Holyiot and keyboard logic
```

Module pad 22 should see actual USB VBUS so firmware can detect USB presence.

### Truly wireless BLE operation

A wireless board still needs 5 V for the TrackPoint and backlight. It therefore
needs a battery power path in addition to the Holyiot regulator:

```text
USB-C ─ charger/power path ─ battery
                 │
                 ├─ 3.0 V LDO ─ Holyiot + keyboard logic/LEDs
                 └─ 5 V boost ─┬─ TrackPoint
                               └─ backlight load switch ─ backlight
```

Do not connect boosted `5V_SYS` back to Holyiot VBUS pad 22. That pad must
indicate real USB cable presence only.

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
least 1.2 A through the battery-side power path. Revision A selects the
TPS61023 and the reference components below, but its low-battery output,
thermal performance, and the measured backlight load still require validation.
Its switch-current rating must not be mistaken for guaranteed 5 V output
current.

The connectivity LED must normally be off or use short low-duty-cycle pulses.
A continuously lit power LED can consume several milliamps and materially
reduce battery life.

### Revision-A power-stage population

The following values are deliberately conservative enough to fabricate the
first PCB before the backlight is measured. Preserve the indicated resistor,
solder-jumper, and test-pad access so values can be changed without respinning
the board.

#### USB charger and power path

Use a [BQ24074](https://www.ti.com/lit/ds/symlink/bq24074.pdf) with these
first-build settings:

| Pin/function | Revision-A connection/value | Result |
| --- | --- | --- |
| `IN` | USB VBUS after fuse/TVS; 1 µF to ground at the pin | 5 V input |
| `EN2` | Low using a 0 Ohm configuration strap | Selects USB100/USB500 modes |
| `EN1` | Holyiot P1.14 through 1 kOhm, with 100 kOhm pull-down and optional force-high strap to `IN` | 100 mA during reset; 500 mA when firmware drives high after enumeration |
| `ILIM` | 3.09 kOhm to ground | About 500 mA if straps are later changed to resistor-programmed mode |
| `ISET` | 4.42 kOhm to ground | About 200 mA maximum battery charge current |
| `ITERM` | 2.94 kOhm to ground | About 20 mA charge termination current |
| `TMR` | 46.4 kOhm to ground | Approximately 6.25-hour fast-charge safety timer |
| `TS` | Battery 10 kOhm NTC; alternatively fit 10 kOhm to ground | Temperature monitoring when the battery provides NTC |
| `CE` | Low using a 0 Ohm strap | Charger enabled |
| `BAT` | 4.7 µF to ground at the pins | Protected 1-cell LiPo connection |
| `OUT` | 10 µF to ground at the pins | System input rail for the LDO and boost converter |
| `CHG`, `PGOOD` | Test pads; optional 100 kOhm pull-ups to 3.0 V | Bring-up visibility without extra always-on LEDs |

This configuration starts in USB100 mode and allows USB500 only after firmware
confirms enumeration. The optional force-high strap is for a known 5 V charger
or controlled bench bring-up, not an unknown computer port. At USB500 it
charges a typical 1000 mAh cell at about 0.2 C. The BQ24074 gives system load
priority, so charging slows or stops when the backlight consumes most of the
available input power. Only use a battery whose manufacturer permits at least
200 mA charge current; otherwise increase `ISET`.

#### Nominal 3.0 V logic rail

Use a fixed-output
[TLV75530P](https://www.ti.com/lit/ds/symlink/tlv755p.pdf) LDO. This is the
cost-optimized Revision-A choice and eliminates the second switching converter
and its inductor:

- input from BQ24074 `OUT`, with 1 µF at `IN`;
- 1 µF at the 3.0 V output, with an additional 100 nF beside the Holyiot;
- `EN` pulled high to the LDO input with 100 kOhm, with a test pad; and
- SOT-23-5 `TLV75530PDBVR` footprint, with copper sized for useful heat
  spreading rather than only the minimum land pattern.

The TLV75530P regulates while its input remains above 3.0 V plus dropout. Near
the bottom of a LiPo discharge its output can fall below 3.0 V instead of
boosting the remaining battery voltage. The nRF52840 itself tolerates this, but
Revision A must verify matrix operation, indicator brightness, and BLE/USB
behavior from a full battery down to the protected battery's cutoff. Treat a
failed low-battery test as a reason to repopulate a later revision with a
buck-boost, not as permission to over-discharge the cell.

Place the LDO and its two capacitors close together. Keeping this rail linear
also removes a switching-noise source near the Holyiot antenna and TrackPoint
DATA/CLOCK routes.

#### Regulated 5 V rail

Populate the [TPS61023](https://www.ti.com/lit/ds/symlink/tps61023.pdf) using
its published single-cell-to-5 V starting values:

- 1 µH shielded inductor, at least 4 A RMS, at least 5 A saturation, and low DCR;
- 10 µF input capacitance;
- two 22 µF, 10 V output capacitors, with DC-bias derating checked;
- 732 kOhm from `VOUT` to `FB` and 100 kOhm from `FB` to ground for 5 V;
- `EN` tied to the system input through a default-fitted 0 Ohm link, with an
  alternate unpopulated GPIO-enable selection pad; and
- 22 µF plus 100 nF locally on each of the TrackPoint and switched-backlight
  branches near J7.

The converter has ample margin for the measured 37.1 mA TrackPoint peak and the
provisional 500 mA backlight allowance. Keep the `SW` copper small, keep the
inductor and input/output capacitors beside the IC, and do not route `FB` near
`SW` or the inductor.

### Current-measurement provisions

Add labeled removable links and test pads so the Power Profiler Kit II (PPK2)
can be inserted without cutting PCB traces:

```text
5V_SYS -- LNK_TP -- TP_5V  -- J7 pin 38
       `- LNK_BL -- BL_5V  -- J7 pins 29 and 31

BAT+   -- LNK_BAT -- complete board power path
3V0    -- LNK_3V0 -- Holyiot and keyboard-logic/LED branch
```

- Use an 0805 0 Ohm link or a solder-bridge-plus-test-pad arrangement for
  `LNK_TP`, `LNK_BL`, and `LNK_3V0`.
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
4. **3.0 V subsystem:** open `LNK_3V0` and profile the release firmware at 3.0 V
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

Revision-A acceptance limits are TrackPoint current no more than 10 mA
continuous and 50 mA peak at 5 V, complete no-backlight battery current no more
than 20 mA during ordinary connected idle, and no unexplained reset-correlated
current spikes. A result above a limit is not automatically a faulty keyboard,
but it must be understood before freezing the boost converter or battery size.

## Suspend, idle, and host wake behavior

The initial production firmware deliberately uses light CPU idle, not nRF52840
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

Keep the working DK target and add a separate production board target:

| Target | Purpose |
| --- | --- |
| `nrf52840dk_nrf52840 + thinkpad_t430` | Existing bench and TrackPoint testing |
| `holyiot_18010_nrf52840 + thinkpad_t430` | Production PCB |

The Holyiot board definition must provide:

- the production GPIO assignment in this document;
- the module's low-frequency crystal configuration;
- USB device support and internal flash/settings partitions;
- P0.18 hardware reset;
- J-Link/SWD runner support;
- no DK buttons, DK LEDs, QSPI flash, or J-Link UART assumptions;
- UARTE reception plus GPIO clock interrupts for the TrackPoint;
- P1.14 charger-current control that remains low during reset and selects
  USB500 only after the USB device is configured, returning low on detach;
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
- [ ] Six BSS138 footprints fitted with correct source/drain orientation.
- [ ] LED current-limit and MOSFET gate pull-down resistors fitted.
- [ ] Three profile LEDs are labeled 1-3 and route through 4.7 kΩ from `3V0`
      to active-low P0.25, P0.11, and P1.08 respectively.
- [ ] `KBD_BL_PWM` is a 3.0 V direct signal with 1 kΩ series protection and a
      100 kΩ pull-down; it has no BSS138 or 5 V pull-up.
- [ ] J7 pin 25 was checked for an unexpected 5 V pull-up before connection to
      the Holyiot.
- [ ] USB CC resistors, ESD protection, and VBUS protection included.
- [ ] BQ24074 EN1 defaults low for USB100 and can be driven by P1.14 only after
      enumeration; the force-USB500 strap is clearly marked and normally open.
- [ ] TPS22918 backlight switch defaults off, uses 10 nF on `CT`, and is driven
      by P1.12 separately from the P1.04 brightness PWM.
- [ ] SWDIO, SWDCLK, reset, target VDD, and ground reach the TC2050 footprint.
- [ ] TC2050 pad 3 is grounded so DK P19 selects the external target.
- [ ] Reset button and accessible VDD/GND test points included.
- [ ] `LNK_TP`, `LNK_BL`, `LNK_3V0`, and high-current `LNK_BAT` measurement
      points included and labeled, each with a nearby ground point.
- [ ] Holyiot operation, matrix scanning, LEDs, USB, and BLE are validated on
      the nominal 3.0 V rail down to the intended low-battery endpoint.
- [ ] USB VBUS detection is isolated from any boosted 5 V system rail.
- [ ] USB remote wake works with the intended BIOS/UEFI and OS settings.
- [ ] BLE wake behavior is verified on every intended host/OS combination.
- [ ] Idle current and 30-minute BLE response latency are measured on battery.
- [ ] TrackPoint stays within, or any deviation explains changes to, the
      measured 6.47 mA active, 10 mA continuous-design, and 50 mA peak budgets.
- [ ] Backlight current and inrush were measured without exceeding PPK2 limits.
- [ ] Battery, charger, regulator, and 5 V boost current budgets finalized.
