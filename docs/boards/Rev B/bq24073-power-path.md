# Revision B BQ24073 charger and power-path wiring

Revision B replaces the Revision A `SY6280AAC` USB current limiter and
`ETA6002E8A` switching charger with one TI `BQ24073RGTR` linear charger and
dynamic power-path controller. This document is authoritative for that
conversion. The BQ24073 must be used only with a protected, single-cell
4.2 V Li-ion/LiPo battery.

The intended fixed configuration limits total USB input current to 500 mA,
programs approximately 189 mA nominal fast-charge current, gives priority to
the system load, and permits the battery to supplement `VSYS` when the system
load exceeds the available USB input power.

Use the manufacturer's RGT 16-pin, 3 mm x 3 mm exposed-pad footprint. Do not
reuse the pin numbers, footprint, or land pattern of either removed IC.
The specified orderable part is TI `BQ24073RGTR`, LCSC
[`C15220`](https://www.lcsc.com/product-detail/C15220.html). Verify the current
TI data-sheet revision, stock, and the exact RGT land pattern before release.

## Revision A parts and nets to remove

Remove the complete Revision A charger/current-limiter implementation:

| Remove | Revision A purpose | Revision B disposition |
| --- | --- | --- |
| `SY6280AAC` | USB input-current limiter | Function moves into BQ24073 |
| 15 kOhm SY6280 `ISET` resistor | Approximately 453 mA limit | Removed |
| SY6280 output bypass capacitor | Limiter stability/bypass | Removed |
| Separate SY6280 input bypass capacitor | Limiter input bypass | Replaced by the BQ24073 `IN` capacitor |
| `ETA6002E8A` | Switching charger and power path | Replaced by BQ24073 |
| 2.2 uH ETA6002 charger inductor | ETA6002 switching stage | Removed; BQ24073 is linear |
| 5.1 kOhm ETA6002 current-set resistor | Approximately 196 mA charging | Replaced by 4.7 kOhm BQ24073 `ISET` resistor |
| ETA6002-specific NTC divider/bypass | ETA6002 temperature input | Replaced by the BQ24073 `TS` connection |
| 1 uF ETA6002 battery capacitor | ETA6002 battery bypass | Replace with 10 uF |

Delete the `USB_5V_LIM` net. Connect `USB_VBUS_RAW` directly to BQ24073 `IN`.
Do not delete or merge `USB_VBUS_RAW`: it must remain the real, protected
cable-VBUS net used by the HolyIOT VBUS-detect input.

Retain all of the following:

- USB-C receptacle and the two individual 5.1 kOhm CC pull-down resistors;
- VBUS PTC, VBUS TVS, and USB D+/D- ESD protector;
- `BAT_RAW+`, the battery fuse, reverse-polarity clamp, `BAT_PROTECTED`, and
  battery measurement link;
- the downstream system high-side P-MOSF and its latching switch;
- `VSYS_SW`, the 3.0 V LDO, 5 V boost converter, branch switches, and branch
  measurement links; and
- the physical separation between `USB_VBUS_RAW` and the locally generated
  `+5V` rail.

## Resulting power path

```text
USB-C VBUS -- F1 PTC --+-- USB_VBUS_RAW -- BQ24073 IN
                       +-- VBUS TVS to GND
                       +-- HolyIOT VBUS detect only

protected 1S pack -- JBAT -- battery fault protection -- BAT_PROTECTED
                                                            |
                                                       BQ24073 BAT

BQ24073 OUT -- VSYS -- system P-MOSF -- VSYS_SW --+-- 3.0 V LDO -- VCC
                                                  +-- 5 V boost -- +5V
```

`OUT` is the system-power node and `BAT` is the battery node. Do not join them
externally. Do not put the main system switch in the battery lead: switching
downstream of `OUT` allows the battery to charge while the keyboard is off.

## Complete BQ24073 pin wiring

### ASCII schematic

```text
                                      REV B USB INPUT

 USB-C VBUS pins
        |
        +---- F1, existing 1 A hold PTC ----+---- USB_VBUS_RAW ----+----> HolyIOT pad 22
                                            |                       |      VBUS detect only
                                            |                       |
                                            +---- TVS ---- GND      +---- C_IN 10 uF / 10 V
                                            |                              |
 USB-C CC1 ---- 5.1 kOhm ---- GND           |                              GND
 USB-C CC2 ---- 5.1 kOhm ---- GND           |
                                            |
                                            v
                                   .-----------------.
                                   |   BQ24073RGTR   |
                                   |                 |
 USB_VBUS_RAW ---------------------| 13 IN           |
                                   |                 |
 GND ------------------------------|  4 CE           |  charge enabled
 GND ------------------------------|  5 EN2          |  EN2=0, EN1=1:
 VSYS -----------------------------|  6 EN1          |  fixed USB500 mode
                                   |                 |
 GND ---- R_ILIM 3.6 kOhm ---------| 12 ILIM         |  ignored in USB500;
                                   |                 |  safe fallback if mode changes
 GND ---- R_ISET 4.7 kOhm ---------| 16 ISET         |  about 189 mA charge
                                   |                 |
 GND ------------------------------| 15 TD           |  termination enabled
 no connection --------------------| 14 TMR          |  default safety timers
                                   |                 |
 fo ------------------------|  1 TS           |  see TS choices below
                                   |                 |
 BAT_PROTECTED --------------------|  2 BAT           |
 BAT_PROTECTED --------------------|  3 BAT           |
                 |                 |                 |
                 +-- C_BAT 10 uF --|-----------------+---- GND
                     10 V          |                 |
                                   |           OUT 10|----------------+---- VSYS
                                   |           OUT 11|----------------+
                                   |                 |                |
                                   |          PGOOD 7|---- NC          +---- C_OUT 22 uF / 10 V
                                   |                 |  or 10 kOhm           |
                                   |                 |  pull-up to VCC       GND
                                   |                 |
                                   |             CHG 9|---- NC
                                   |                 |  or charge indicator
                                   |                 |
 GND ------------------------------|  8 VSS          |
 GND plane ========================| exposed pad     |
                                   '-----------------'


                                  BATTERY CONNECTION

 Approved protected 1S LiPo, 3.7 V nominal / 4.2 V maximum

 battery BAT+ ---- JBAT ---- BAT_RAW+ ---- F2 ---- BAT_PROTECTED ----> pins 2 and 3
                                                        |
                                                  cathode of D6
                                                        |
                                                   D6, SS34
                                                        |
                                                   anode to GND

 battery BAT- ---- JBAT ------------------------------------ GND


                                 TS OPTION A: TWO-WIRE PACK

 BQ24073 pin 1 TS ---- 10 kOhm, 1% ---- GND

 This only supplies a valid fixed TS resistance. It does not measure the
 battery temperature. Do not replace the resistor with a direct wire.


                           TS OPTION B: THREE-WIRE NTC PACK

 BQ24073 pin 1 TS ---------------- pack TEMP lead
                                          |
                                  internal 10 kOhm NTC
                                  at 25 degrees C
                                          |
 pack BAT- -------------------------------+------------ GND

 Do not fit the fixed 10 kOhm resistor when the pack NTC is connected.


                                  DOWNSTREAM POWER

 BQ24073 pins 10,11 OUT ---- VSYS ---- existing system P-MOSF ---- VSYS_SW
                                                                         |
                                      +----------------------------------+
                                      |
                                      +---- ME6211 3.0 V LDO ---- VCC
                                      |                            |
                                      |                            +---- HolyIOT VDD
                                      |                            +---- 3 V logic
                                      |
                                      +---- SY7069 boost ---- +5V
                                                               |
                                                               +---- TrackPoint
                                                               +---- backlight switch

 USB_VBUS_RAW and +5V are different nets. Never join them.
 BAT_PROTECTED and VSYS are different nets. Never join them externally.
```

If `CHG` drives an LED directly, use the following active-low connection:

```text
 VCC ---- 4.7 kOhm ---- LED anode |>| LED cathode ---- pin 9 CHG

 charging:      CHG pulls low, LED on
 charge done:   CHG is high-impedance, LED off
```

| Pin | Name | Required connection | Passive or configuration |
| ---: | --- | --- | --- |
| 1 | `TS` | Approved pack's 10 kOhm NTC to GND, or fixed fallback to GND | Fit 10 kOhm, 1% fallback only for a two-wire battery build; do not parallel it with a pack NTC |
| 2, 3 | `BAT` | Both pins to `BAT_PROTECTED` with a short, wide connection | 10 uF, 10 V, X5R/X7R from `BAT` to GND, placed at the pins |
| 4 | `CE` | GND | Active-low charge enable; no pull resistor required in the fixed configuration |
| 5 | `EN2` | GND | Selects fixed USB500 mode together with `EN1 = high` |
| 6 | `EN1` | `OUT` | Direct connection selects fixed USB500 mode; do not leave floating |
| 7 | `PGOOD` | Optional HolyIOT input or indicator; otherwise no-connect | Open-drain; if used as logic, pull up to `VCC` with 10 kOhm; never pull above the receiving rail |
| 8 | `VSS` | Ground plane | Direct ground connection; this pin remains the primary electrical ground |
| 9 | `CHG` | Optional HolyIOT input or charge LED; otherwise no-connect | Open-drain; use 10 kOhm pull-up to `VCC` for logic, or an LED plus resistor as described below |
| 10, 11 | `OUT` | Both pins to `VSYS` with a short, wide connection | 22 uF, 10 V, X5R/X7R from `OUT` to GND, placed at the pins |
| 12 | `ILIM` | 3.6 kOhm, 1%, to GND | Ignored in USB500 mode; defines about 424 mA typical and less than 500 mA at the data-sheet maximum factor if `EN2/EN1` is ever changed to `1/0` |
| 13 | `IN` | `USB_VBUS_RAW` | 10 uF, 10 V, X5R/X7R from `IN` to GND, placed at the pin |
| 14 | `TMR` | No-connect | Selects the internal default approximately 30-minute precharge and 5-hour fast-charge safety timers |
| 15 | `TD` | GND | Enables normal charge termination; high disables termination, so do not leave it to an external control by default |
| 16 | `ISET` | 4.7 kOhm, 1%, to GND | Programs about 189 mA nominal fast-charge current |
| pad | exposed thermal pad | Ground plane, same potential as `VSS` | Use the TI land pattern and thermal vias; do not rely on the pad instead of connecting pin 8 |

The charge-current calculation is:

```text
ICHG = KISET / RISET
     = 890 A*Ohm / 4.7 kOhm
     = 189 mA nominal
```

The BQ24073 termination threshold is internally fixed at approximately 10% of
the programmed fast-charge current in USB500 mode, or about 19 mA nominal for
this design.

The optional-mode input-limit calculation is:

```text
IIN(typ) = KILIM(typ) / RILIM
         = 1525 A*Ohm / 3.6 kOhm
         = 424 mA typical

IIN(max-factor) = 1720 A*Ohm / 3.6 kOhm
                = 478 mA
```

This resistor does not set the limit while `EN2 = 0` and `EN1 = 1`; the
internal USB500 limit does. Fit it anyway so a future strap or assembly error
cannot select resistor-programmed mode with `ILIM` undefined.

## Required passive BOM

All resistors are 0603, 1%, at least 0.1 W unless an approved footprint calls
for another size. Capacitors are X5R or X7R; verify effective capacitance at
5 V DC bias.

| Qty | Value | Connection | Notes |
| ---: | --- | --- | --- |
| 1 | 4.7 kOhm | `ISET` to GND | Approximately 189 mA fast charge |
| 1 | 3.6 kOhm | `ILIM` to GND | Defined sub-500 mA fallback for resistor-programmed mode |
| 1 conditional | 10 kOhm | `TS` to GND | Fit only when no compatible external NTC is connected |
| 1 | 10 uF, 10 V | `IN` to GND | Place beside pin 13 and pin 8/ground return |
| 1 | 22 uF, 10 V | `OUT` to GND | May reuse the Rev A `VSYS` capacitor if placement is corrected for the new IC |
| 1 | 10 uF, 10 V | `BAT` to GND | Replaces the Rev A 1 uF battery capacitor |
| optional | 10 kOhm | `PGOOD` to `VCC` | Only if `PGOOD` is read by the MCU |
| optional | 10 kOhm | `CHG` to `VCC` | Only if `CHG` is read by the MCU |

Do not add a generic 100 nF in place of any of the three required power
capacitors. An additional 100 nF may be fitted in parallel at `IN` for
high-frequency decoupling, but it does not replace the 10 uF part.

### Optional charge-status LED

If a dedicated charge LED is wanted instead of an MCU input, connect it as an
active-low indicator:

```text
VCC -- resistor -- LED anode
                  LED cathode -- CHG pin 9
```

Use a high-efficiency LED and start with 4.7 kOhm at 3.0 V. Do not connect an
LED and a separate pull-up if the combined current or user-visible behavior
has not been checked. `CHG` is low while charging and high-impedance when
charging is complete or disabled.

## Battery temperature options

Preferred production configuration:

```text
three-wire approved battery:
BAT+  -> battery protection -> BAT_PROTECTED
BAT-  -> GND
NTC   -> TS, with documented 10 kOhm-at-25-C NTC from NTC to BAT-
```

For a two-wire, customer-supplied battery, fit the 10 kOhm `TS` fallback and
state explicitly that the charger is not measuring cell temperature. A
"protected" two-wire pack does not imply that it includes charge-temperature
qualification. Do not claim arbitrary JST-PH battery compatibility: connector
polarity, cell chemistry, maximum charge voltage, protection circuit, capacity,
discharge rating, physical fit, and lead quality must all match.

If both battery types must share one PCB, provide mutually exclusive assembly
options rather than an end-user jumper:

| Assembly | Fixed 10 kOhm `TS` resistor | External NTC link |
| --- | --- | --- |
| Approved two-wire pack | Fit | Do not fit |
| Approved three-wire 10 kOhm NTC pack | Do not fit | Fit |

For the nominal 189 mA charge current, specify a protected 1S 4.2 V pack whose
manufacturer permits at least 200 mA charging. A 500 mAh or larger pack keeps
the nominal rate at or below approximately 0.4 C; the approved pack data sheet,
not capacity alone, remains authoritative.

## Nets and downstream compatibility

Rev B power-domain definitions become:

| Net | Source | Expected behavior |
| --- | --- | --- |
| `USB_VBUS_RAW` | USB-C VBUS after PTC/protection | Real cable VBUS; feeds BQ24073 `IN` and HolyIOT VBUS detect |
| `BAT_PROTECTED` | Battery fault-protection output | Bidirectional battery charge/discharge node; connects to BQ24073 `BAT` |
| `VSYS` | BQ24073 `OUT` | Approximately 4.4 V with valid USB input; follows the battery through the power path without USB |
| `VSYS_SW` | Existing system P-MOSF output | Switched input to the 3.0 V LDO and 5 V boost converter |

Check every `VSYS` and `VSYS_SW` component for at least the BQ24073's maximum
regulated `OUT` voltage plus normal design margin. Do not power the HolyIOT
directly from `VSYS`; retain the 3.0 V regulator.

The BQ24073 is linear. At the intended charge current, estimate charger heat
using `(VIN - VBAT) * IBAT`, then add loss from system current through the
input-to-output path. Provide a ground copper area and thermal vias under the
exposed pad; do not place the charger beneath the battery pouch.

## Placement and routing

1. Place the BQ24073, its three power capacitors, and the power-ground returns
   as one compact group.
2. Route both `IN` and both `OUT`/`BAT` pin pairs with short, wide copper. Join
   paired pins at the package rather than feeding one through the other.
3. Keep `ISET`, `ILIM`, `TS`, and `TMR` away from USB D+/D-, the 5 V boost
   switch node, backlight PWM, and high-current edges.
4. Give the exposed pad several small thermal vias into a ground plane. Follow
   TI's paste-window and land-pattern recommendations to avoid floating the
   QFN during reflow.
5. Put labeled test points on `USB_VBUS_RAW`, `BAT_PROTECTED`, `VSYS`, and GND.
6. Preserve the existing TVS's short return to the connector-side ground; do
   not route surge current through the charger's signal-ground area.

## Bring-up and release tests

Do not describe the Rev B power path as complete until all of these tests pass:

1. With no battery, apply a current-limited 5 V input and verify `VSYS`, `VCC`,
   USB enumeration, and maximum available system load.
2. With USB absent, power from a battery emulator across the intended battery
   range and verify `VSYS`, the system latch, 3.0 V rail, 5 V boost, and cutoff
   behavior.
3. With battery and USB present, measure total VBUS current at idle, maximum
   backlight, and maximum backlight while charging. Confirm it never exceeds
   the USB500 limit.
4. Program a battery emulator below 3 V and verify precharge current and
   transition to fast charge. Then verify approximately 189 mA fast charge and
   approximately 19 mA termination under conditions where DPPM and thermal
   regulation are inactive.
5. Increase system load through the DPPM region. Verify charge current reduces
   before the input limit is exceeded, then verify battery supplement mode.
6. Insert and remove USB repeatedly at idle and maximum allowed load. Check for
   resets, rail overshoot, reverse current, and false VBUS detection.
7. Verify the `TS` cold/hot response with resistor substitution on the NTC
   assembly; verify the fixed-fallback assembly separately.
8. Run a sustained worst-case thermal test at maximum ambient temperature with
   the final enclosure and battery position.
9. Test battery reversal and short protection only with a current-limited
   battery emulator. Inspect and replace the sacrificial battery fuse after a
   fault test.
10. Re-run the complete power sequence with the keyboard physically off;
    verify that the battery still charges while `VSYS_SW`, `VCC`, and `+5V`
    remain off.

The fixed `EN1 = high`, `EN2 = low` configuration matches the design's fixed
500 mA input ceiling. Strict enumeration-dependent 100 mA/500 mA selection or
USB suspend control would require deliberate MCU control of both pins and is a
separate hardware/firmware feature; do not add it implicitly during layout.
