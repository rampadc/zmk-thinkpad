# T470 Holyiot preliminary PCB wiring plan

This document plans a custom controller around the Holyiot 18010 V1.0
nRF52840 module. It covers both T470 flex interfaces, the complete keyboard
matrix, TrackPoint and its buttons, keyboard indicators, and backlight routing.

> **Status:** preliminary schematic-capture plan. The current firmware still
> targets the nRF52840 DK. Do not manufacture a PCB until connector orientation,
> keyboard power rails, and the physical module pad numbering have been checked
> on real parts. This plan follows the motherboard's default stuffing option:
> `VCC_TP` and the TrackPoint PS/2 signals operate at 5 V. The schematic also
> provides an optional, mutually exclusive 3.3 V `VCC_TP` configuration.

## Numbering and voltage conventions

Use the newer Holyiot 18010-A **top-view** pad numbering. Put both the module
pad number and nRF GPIO name on the schematic; GPIO names are authoritative if
different module drawings disagree. The module logic supply is planned as
3.0 V, and nRF52840 GPIOs must never receive 5 V.

The custom PCB uses a centered 40-pin FFC connector for the keyboard's
36-contact flex. On the final PCB, keyboard cable contact 1 mates with connector
pin 3 and cable contact 36 mates with pin 38: `final PCB pin = cable contact +
2`. Final connector pins 1-2 and 39-40 are unused. The temporary breakout board
used during DK testing reverses the cable and is documented separately below.
Within the 36 fine-pitch contacts, J38 pin 25 is the keyboard's `VCC3M` supply,
and J38 pins 30 and 32 are GND. J38 also has
connector/shield grounds 37 (`GND1`) and 38 (`GND2`), separate from those 36
contacts. Bond every available ground contact to board ground.

J37 remains the 12-contact TrackPoint/backlight connector with ground tabs 13
and 14. Verify connector pin 1 and cable orientation by continuity before
applying power.

## Complete proposed GPIO assignment

The matrix allocation intentionally matches the T430 Holyiot plan by signal
name. This keeps firmware and PCB review straightforward while leaving enough
GPIO for every T470-specific signal.

### 40-pin keyboard FFC connector

| Straight 40-pin pin | Reversed 40-pin pin | Signal | Holyiot pad | Unified GPIO | Proposed circuit |
| ---: | ---: | --- | ---: | --- | --- |
| 1-2 | 39-40 | NC | - | - | Outside the 36-contact cable; leave unconnected |
| 3 | 38 | SENSE3 | 8 | P0.28 | Active-low input, internal pull-up |
| 4 | 37 | SENSE7 | 43 | P0.27 | Active-low input, internal pull-up |
| 5 | 36 | SENSE6 | 17 | P0.12 | Active-low input, internal pull-up |
| 6 | 35 | DRV14 | 34 | P1.06 | Matrix output |
| 7 | 34 | SENSE4 | 12 | P0.04 | Active-low input, internal pull-up |
| 8 | 33 | SENSE1 | 40 | P0.25 | Active-low input, internal pull-up |
| 9 | 32 | DRV0 | 28 | P1.03 | Matrix output |
| 10 | 31 | SENSE2 | 10 | P0.30 | Active-low input, internal pull-up |
| 11 | 30 | SENSE0 | 6 | P0.03 | Active-low input, internal pull-up |
| 12 | 29 | DRV4 | 3 | P1.10 | Matrix output |
| 13 | 28 | DRV2 | 16 | P1.09 | Matrix output |
| 14 | 27 | SENSE5 | 4 | P1.13 | Active-low input, internal pull-up |
| 15 | 26 | DRV1 | 46 | P0.26 | Matrix output |
| 16 | 25 | DRV3 | 11+ | P0.31 | Matrix output |
| 17 | 24 | DRV6 | 9 | P0.29 | Matrix output |
| 18 | 23 | DRV7 | 13 | P0.05 | Matrix output |
| 19 | 22 | DRV5 | 5 | P1.15 | Matrix output |
| 20 | 21 | DRV15 | 48 | P1.05 | Matrix output |
| 21 | 20 | DRV13 | 49 | P0.24 | Matrix output |
| 22 | 19 | DRV9 | 27 | P1.00 | Matrix output; SWO unavailable |
| 23 | 18 | DRV12 | 47 | P1.07 | Matrix output |
| 24 | 17 | DRV10 | 42 | P1.08 | Matrix output |
| 25 | 16 | DRV8 | 7 | P0.02 | Matrix output |
| 26 | 15 | DRV11 | 30 | P1.02 | Matrix output |
| 27 | 14 | `VCC3M` (J38 pin 25) | - | - | Connect to regulated 3.0 V; decouple near the FFC |
| 28 | 13 | `-LED_FNLOCK` | 29 | P1.01 | External low-side MOSFET and 100 ohm series resistor |
| 29 | 12 | `-LED_MUTE` | 50 | P0.20 | External low-side MOSFET and 560 ohm series resistor |
| 30 | 11 | `-LED_MICMUTE` | 51 | P0.17 | External low-side MOSFET and 560 ohm series resistor |
| 31 | 10 | `-HOTKEY` (Fn) | 2 | P1.11 | Active-low input, internal pull-up |
| 32 | 9 | GND (J38 pin 30) | 1/25/37 | GND | Connect directly to board ground |
| 33 | 8 | `-LED_CAPSLOCK` (J38 pin 31) | 26 | P0.22 | External low-side MOSFET and 100 ohm series resistor |
| 34 | 7 | GND (J38 pin 32) | 1/25/37 | GND | Connect directly to board ground |
| 35 | 6 | `TP4LEFT` (J38 pin 33) | - | - | Direct passive bridge to J37 pin 7 |
| 36 | 5 | `TP4RIGHT` (J38 pin 34) | - | - | Direct passive bridge to J37 pin 6 |
| 37 | 4 | `TP4MIDDLE` (J38 pin 35) | - | - | Direct passive bridge to J37 pin 5 |
| 38 | 3 | `KBD_ID` (J38 pin 36) | - | - | Leave unconnected unless keyboard identification is implemented |
| 39-40 | 1-2 | NC | - | - | Outside the 36-contact cable; leave unconnected |
| Connector grounds (J38 37/38) | Connector grounds (J38 37/38) | GND | 1/25/37 | GND | Bond conductive connector/shield tabs to board ground when accessible |

Use the straight column for a correctly oriented connector and the reversed
column for the temporary DK breakout. J38 pin 25 is `VCC3M` and appears on
reversed breakout pin 14. `HOTKEY` is J38 pin 29 and therefore appears on
reversed breakout pin 10. J38 pins 30, 31, and 32 are GND, `-LED_CAPSLOCK`, and
GND, appearing on reversed breakout pins 9, 8, and 7 respectively.

Sheet 62 explicitly wires the three button nets between the keyboard flex and
J37; they do not leave that sheet for the EC. Reproduce those passive bridges
so the button contacts reach the TrackPoint controller and are reported through
PS/2. Do not connect them to Holyiot GPIOs. Put a removable 0-ohm link and test
pad in each bridge for bring-up isolation.

### Temporary nRF52840 DK matrix wiring

This is the complete wiring used by the current matrix-discovery firmware. It
uses the same GPIO assignment as the future Holyiot prototype, so no matrix
GPIO remapping will be needed when moving off the DK. The DK's onboard QSPI
flash remains physically connected to P0.17/P0.19-P0.23, so none of those GPIOs
are used for the matrix.

| Reversed breakout pin | Signal | nRF52840 DK GPIO |
| ---: | --- | --- |
| 38 | SENSE3 | P0.28 |
| 37 | SENSE7 | P0.27 |
| 36 | SENSE6 | P0.12 |
| 35 | DRV14 | P1.06 |
| 34 | SENSE4 | P0.04 |
| 33 | SENSE1 | P0.25 |
| 32 | DRV0 | P1.03 |
| 31 | SENSE2 | P0.30 |
| 30 | SENSE0 | P0.03 |
| 29 | DRV4 | P1.10 |
| 28 | DRV2 | P1.09 |
| 27 | SENSE5 | P1.13 |
| 26 | DRV1 | P0.26 |
| 25 | DRV3 | P0.31 |
| 24 | DRV6 | P0.29 |
| 23 | DRV7 | P0.05 |
| 22 | DRV5 | P1.15 |
| 21 | DRV15 | P1.05 |
| 20 | DRV13 | P0.24 |
| 19 | DRV9 | P1.00 |
| 18 | DRV12 | P1.07 |
| 17 | DRV10 | P1.08 |
| 16 | DRV8 | P0.02 |
| 15 | DRV11 | P1.02 |
| 14 | `VCC3M` (J38 pin 25) | DK 3.0 V |
| 10 | `-HOTKEY` (Fn) | P1.11 |
| 9 | GND (J38 pin 30) | GND |
| 8 | `-LED_CAPSLOCK` (J38 pin 31) | Leave disconnected during matrix discovery |
| 7 | GND (J38 pin 32) | GND |

Connect reversed breakout pin 14 to the DK's regulated 3.0 V supply. Leave the
LED, NC, and TrackPoint-button breakout pins disconnected during matrix
discovery. SENSE and `HOTKEY` inputs are active-low with internal
pull-ups. The firmware prompts for all 84 keys in physical US-layout order and
reports each captured `DRVn`/`SENSEn` coordinate over the DK's J-Link UART at
115200 baud. It prints the complete map after the final key.

The DK and Holyiot use the same matrix and `HOTKEY` GPIOs in this guide.

### J37 TrackPoint and backlight connector

| J37 | Signal / rail | Holyiot pad | GPIO | Proposed circuit |
| ---: | --- | ---: | --- | --- |
| 1 | `TP4DATA` | 52 | P0.15 | Mandatory 5 V-to-3.0 V open-collector translator |
| 2 | `VCC_TP` | - | - | Switched/fused 5 V branch, default schematic option |
| 3 | GND | 1/25/37 | GND | Solid ground plane |
| 4 | `TP4_RESET` | 54 | P0.13 | Active-high TrackPoint reset behind an inverting low-side MOSFET |
| 5 | `TP4MIDDLE` | - | - | Direct passive bridge to final PCB FFC pin 37 |
| 6 | `TP4RIGHT` | - | - | Direct passive bridge to final PCB FFC pin 36 |
| 7 | `TP4LEFT` | - | - | Direct passive bridge to final PCB FFC pin 35 |
| 8 | `VCC_TP` | - | - | Same switched/fused 5 V TrackPoint branch |
| 9 | `TP4CLK` | 53 | P0.14 | Mandatory 5 V-to-3.0 V open-collector translator |
| 10 | Backlight 5 V feed | - | - | Switched 5 V after backlight P-MOSF |
| 11 | `KBD_BL_PWM` | 33 | P1.04 | 0-3.0 V push-pull PWM, nominal 1 kHz |
| 12 | `-KBD_BL_DTCT` | 38 | P1.14 | Input/test pad; polarity and pull to be measured |
| 13-14 | GND tabs | 1/25/37 | GND | Solid ground plane |

Sheet 62 uses a stuffing option to select the source of `VCC_TP`:

- Default path: `VCC5B -> F13 -> VCC_TP`. F13 is shown as the fitted fuse.
- Alternate path: `VCC3B -> F42 -> VCC_TP`. F42 and its local capacitor are
  marked `@`, indicating the optional/not-default assembly path.
- J37 pins 2 and 8 are both fed by the selected `VCC_TP` rail.
- J37 pin 10 is separately fed from `VCC5B` through F23 for the backlight.
- R293 and R294 pull `TP4CLK` and `TP4DATA` up to `VCC_TP`.
- R324 pulls `TP4_RESET` up to `VCC_TP`.
- Sheet 63 shows additional 3.3 V pull-ups on the touchpad branch, but the
  connecting zero-ohm links R510/R511 on sheet 62 are marked `@`; that optional
  branch does not make the default J37 TrackPoint interface 3.3 V.

This controller intentionally implements the default 5 V option requested for
the TrackPoint. Fit separate removable 0-ohm links in the TrackPoint 5 V and
backlight 5 V branches so each load can be isolated and measured.

## TrackPoint electrical interface

Treat `TP4DATA` and `TP4CLK` as 5 V open-collector signals. Use one mandatory
BSS138 translator for each line:

```text
Holyiot GPIO -- BSS138 source   drain -- TP4DATA/TP4CLK
 internal          gate -> 3.0 V          |
  pull-up                                4.7k
                                           |
                                      VCC_TP 5 V
```

- BSS138 source: Holyiot/3.0 V side.
- BSS138 drain: J37/5 V side.
- BSS138 gate: fixed 3.0 V.
- Low side: Holyiot internal pull-up to 3.0 V; an optional DNP external pull-up
  footprint may be retained for debugging.
- High side: 4.7 kOhm pull-up to the switched `VCC_TP` 5 V rail.
- Do not provide a fitted direct bypass around either translator.

Use a third BSS138 for reset: drain to J37 pin 4, source to ground, gate from
P0.13 through 1 kOhm, and 100 kOhm gate-to-ground. Fit a 10 kOhm reset pull-up
to `VCC_TP` 5 V, matching the selected motherboard option. TrackPoint IV RESET
is active high: nRF low leaves the MOSFET off and asserts J37 reset high; after
600 ms nRF high turns the MOSFET on and releases J37 reset low. The gate
pull-down holds reset asserted, not released, while the Holyiot is unpowered.

`TP4LEFT`, `TP4RIGHT`, and `TP4MIDDLE` are point-to-point passive bridges from
final PCB FFC pins 35/36/37 to J37 pins 7/6/5. They do not route to the MEC1653 EC
on sheet 59. Preserve each bridge through a removable 0-ohm link and add a test
pad; do not add pull-ups, translators, or Holyiot GPIO connections. The
TrackPoint controller should encode these button states into its PS/2 packets.

## Keyboard indicator routing

All four keyboard FFC LED nets are active-low in the motherboard schematic.
Reproduce the motherboard series values and use a low-side BSS138 per LED:

```text
40-pin FFC LED net -- series resistor -- BSS138 drain
                                  source -- GND
Holyiot GPIO -- 1k -- gate
                         |
                       100k
                         |
                        GND
```

| Indicator | Series value | GPIO |
| --- | ---: | --- |
| Caps Lock | 100 ohm | P0.22 |
| Fn Lock | 100 ohm | P1.01 |
| Speaker mute | 560 ohm | P0.20 |
| Microphone mute | 560 ohm | P0.17 |

The MOSFET gate is active-high in firmware even though the connector signal is
named active-low. Verify which keyboard rail supplies the LED anodes and their
brightness before fixing resistor values; retain footprints that accept
100-3.9 kOhm tuning values.

## Backlight routing

Use separate power-enable and PWM controls:

```text
5V_SYS -- P-MOSFET -- 0R measurement link -- J37 pin 10
             gate -- 100k to 5V_SYS
             gate -- BSS138 drain
Holyiot P1.12 -- 1k -- BSS138 gate; source -- GND

Holyiot P1.04 ------------------------------ J37 pin 11 PWM
Holyiot P1.14 <-- configurable input -------- J37 pin 12 detect
```

- P1.12 (Holyiot pad 39) is `NRF_BL_ENABLE`; high enables the 5 V branch.
- P1.04 supplies brightness PWM only and must never be pulled to 5 V.
- Start with 1 kHz PWM, 0%, 50%, and 100% firmware levels.
- Put 22 uF plus 100 nF after the P-MOSF close to J37, and allow a 10 nF
  gate-to-source capacitor footprint for inrush tuning.
- Route detect through a 10 kOhm series resistor to P1.14, with DNP pull-up,
  pull-down, and clamp footprints. Characterize its voltage and polarity before
  enabling it in firmware.

## Reserved module pads

| Holyiot pad | GPIO / signal | Planned use |
| ---: | --- | --- |
| 14 | VDD-nRF | Regulated 3.0 V, local 100 nF + 4.7 uF |
| 21 | P0.18/nRESET | Reset switch and programming header |
| 22-24 | VBUS, USB D-, USB D+ | USB power detection and USB data |
| 31-32 | SWDCLK, SWDIO | Tag-Connect programming footprint |
| 35-36 | P0.09, P0.10 | Spare/test pads; require NFC-as-GPIO configuration |
| 39 | P1.12 | Backlight 5 V enable |
| 41-42, 55 | P0.11, P1.08, P0.16 | Optional BLE profile LEDs 1-3, active-low |
| 43-46 | P0.27, P0.08, P0.06, P0.26 | Spare/test pads |
| 55 | P0.16 | Spare/test pad |

For optional BLE indicators, route `3.0 V -> 4.7 kOhm -> LED anode`, LED
cathode to the GPIO. Keep these DNP-capable; they are not required for keyboard
operation.

## PCB routing priorities

1. Put the Holyiot antenna at the PCB edge with no copper, planes, vias, flex,
   battery, or keyboard backplate in its complete keepout on any layer.
2. Use a four-layer stack: signals/components, uninterrupted ground, power,
   signals. Break the ground plane only for the RF keepout.
3. Place the 40-pin keyboard FFC connector so its dense matrix fan-out reaches
   the module with minimal crossings. Matrix signals are low speed; orderly
   routing matters more than controlled impedance.
4. Place TrackPoint DATA/CLOCK translators, 5 V pull-ups, reset MOSFET, and
   test pads adjacent to J37. Keep the 5 V TrackPoint branch visually obvious.
5. Keep USB D+/D- short, paired, and away from matrix/boost switching nodes.
6. Keep the 5 V boost converter, inductors, and backlight switch away from the
   antenna and TrackPoint signals. Do not route switching loops beneath J37.
7. Label connector pin 1, every rail, every configurable jumper, SWD, reset,
   and all spare test pads on silkscreen.

## Required first-article checks

- Continuity-map both flex cables and confirm connector orientation.
- With the keyboard disconnected, verify 3.0 V and 5 V rails and ensure every
  GPIO-facing test pad remains below the nRF52840 absolute maximum.
- Power J37 branches independently with current limits and confirm that both
  DATA/CLOCK translator low sides remain at or below 3.0 V while measuring
  button idle/pressed levels, reset polarity, and current on pins 2 and 8.
- Verify each matrix coordinate with debug logging before installing a final
  physical-layout transform.
- Test all four indicators at conservative current and tune their resistors.
- Measure backlight startup current, steady current, PWM behavior, and detect
  polarity at 0%, 50%, and 100%.
- Validate USB, BLE, SWD recovery, reset, and RF range with the final keyboard
  backplate and enclosure installed.
