# Revision B consolidated hardware specification

> **Status: authoritative design draft; not yet built or validated.** This file
> consolidates the current Revision B power, wiring, connector, passive,
> configuration-link, measurement-access, and bring-up decisions. Where it
> conflicts with the older Revision B notes, use this file for the next
> schematic revision. Do not release a PCB until every item marked **release
> blocker** has been resolved against the completed schematic and layout.

## 1. Scope and fixed decisions

Revision B is one shared HolyIOT/nRF52840 core board connected through a
60-contact FFC to one passive T430 or T470 keyboard adapter. Only one adapter
may be connected at a time.

The design shall:

- support USB-C default current safely, and use higher current only after a
  valid 1.5 A or 3 A Type-C CC advertisement;
- allow an approved protected 1S Li-ion/LiPo battery to supplement the USB
  source through the BQ24073 power path;
- generate regulated 3.3 V with a buck-boost converter and generate local 5 V
  for TrackPoint and keyboard backlight;
- keep the TrackPoint and backlight 5 V branches separately isolatable;
- use a current-limited backlight load switch and an on-board Kelvin shunt;
- consolidate repeated low-power resistors into isolated four-resistor arrays;
- use regulator shutdown for the maintained system power switch; do not fit a
  discrete system P-channel MOSFET;
- use no removable 2.54 mm jumper headers or plastic shunts; and
- expose flat measurement, isolation and injection access sufficient to debug
  every power domain without lifting an IC.

The preliminary schematic and BOM are inputs, not the final design. They still
contain the old Q2/Q3 backlight switch, the ME6211 3.0 V LDO, two discrete USB-C
CC resistors, a 3.6 kOhm BQ24073 ILIM resistor and a 1 A USB PTC. Those items
must change as specified below.

## 2. Naming convention

`TP` is reserved for **TrackPoint** signal names such as `TP4_DATA`. Do not use
`TP` as a test-point prefix on Revision B.

| Prefix | Meaning | Physical implementation |
| --- | --- | --- |
| `MP_` | Measurement point | Flat exposed copper pad for probe or pogo pin |
| `KL_` | Kelvin measurement point | Matched pair of pads routed independently to a shunt terminal |
| `LK_` | Normally fitted isolation link | Zero-ohm resistor footprint; removable for fault isolation or injection |
| `SJ_` | Assembly configuration selector | Two-pad open/closed or three-pad solder selector; no header |
| `INJ_` | External power injection | Large flat pads and optional unpopulated plated holes; no fitted connector in production |
| `DBG_` | Programming/debug access | Tag-Connect or compact pogo-pad footprint |

Silkscreen may abbreviate long names, but the schematic net and reference name
must remain unambiguous. For example, use `MP_TP4_DATA` for a measurement pad
on the TrackPoint data signal, never `TP_TP4_DATA`.

## 3. System power architecture

```text
USB-C CC1/CC2 ---> HUSB320 current detector ---> BQ24073 mode logic

USB-C VBUS -- F1 -- USB_VBUS_RAW ----+---- BQ24073 IN
                                     +---- VBUS detect to HolyIOT
                                     +---- Schottky ---- TYPEC_VDD

protected 1S battery --------------------- BQ24073 BAT

BQ24073 OUT -- VSYS ------------------------------+-- TPS63031 --> VCC_REG -- LK_VCC --> VCC 3.3 V
                                                  |
maintained POWER switch --> SYS_EN ---------------+-- converter EN pins
                                                  |
                                                  +-- SY7069 --> BOOST_5V -- LK_5V_SOURCE --> +5V
                                                                                                  |
                                                                                                  +-- LK_TRACKPOINT_5V --> TrackPoint 5 V
                                                                                                  |
                                                                                                  +-- SGM2562 --> 50 mOhm shunt --> BL_5V
```

`USB_VBUS_RAW`, `BOOST_5V`, `+5V`, `BL_5V`, `VSYS`, `VCC_REG` and
`VCC` are different nets. Do not merge them in the schematic or by copper.

### 3.1 USB-C input and advertised-current detection

Use Hynetek `HUSB320-BA000-QN12R` (LCSC `C7471906`) in sink-only/UFP
GPIO mode:

- CC1 and CC2 connect directly to the USB-C receptacle;
- remove the preliminary R22/R23 5.1 kOhm pull-downs;
- `PORT/DEBUG_N` connects to GND through 900 kOhm, 1%, `ADDR/ORIENT` is
  unconnected and `EN_N` is low; do not tie `PORT/DEBUG_N` directly to GND
  because it becomes a push-pull output after initialization;
- `VBUS_DET` connects directly to `USB_VBUS_RAW`; the stocked BA000 ordering
  variant is factory-configured for a 0 Ohm external path;
- `TYPEC_VDD` is derived from `USB_VBUS_RAW` through a qualified low-drop
  Schottky diode and has a local 100 nF bypass;
- `OUT1` and `OUT2` each have 10 kOhm pull-ups to `TYPEC_VDD`;
- `OUT1` drives BQ24073 `EN1` directly;
- `SN74LVC1G14DBVR`, powered from `TYPEC_VDD`, generates
  `EN2 = NOT OUT1`; and
- `EN2` has a 10 kOhm fail-safe pull-down.

| Source state | OUT1 | OUT2 | BQ EN1 | BQ EN2 | Input policy |
| --- | ---: | ---: | ---: | ---: | --- |
| Unattached | H | H | H | L | Safe USB500 state |
| Default-current source or A-to-C cable | H | L | H | L | USB500 |
| 1.5 A advertised | L | H | L | H | Resistor-programmed limit |
| 3 A advertised | L | L | L | H | Same resistor-programmed limit |

Level-shift `OUT1` to 3.3 V with a BSS138 channel and a 10 kOhm pull-up. The
HolyIOT input is `TYPEC_HIGH_CURRENT_N` on `P0.09`; low means a valid 1.5 A or
3 A advertisement. Firmware must immediately disable batteryless backlight
power if this input becomes high.

F1 must have at least 1.5 A **hold current after maximum-temperature
derating**, sufficiently low series resistance, and adequate fault rating.
The preliminary `1206L100/16NR` is 1 A hold and is rejected. A 1.5 A 1206 PTC
such as `ASMD1206-150`, LCSC `C135341`, is a candidate only; approve it after
checking its hot hold-current curve, resistance and trip behavior.

### 3.2 BQ24073 charger and supplement path

Use `BQ24073RGTR`, LCSC `C15220`, in the TI RGT exposed-pad footprint.

| Pin/function | Required connection |
| --- | --- |
| `IN` | `USB_VBUS_RAW`; local 10 uF, 10 V X5R/X7R to GND |
| `BAT` pins | `BAT_PROTECTED`; local 10 uF, 10 V X5R/X7R to GND |
| `OUT` pins | `VSYS`; local 22 uF, 10 V X5R/X7R to GND |
| `VSS` and pad | Ground plane and thermal vias |
| `CE` | GND |
| `EN1` | Type-C `OUT1` |
| `EN2` | Inverted Type-C `OUT1`, with 10 kOhm pull-down |
| `ILIM` | 1.2 kOhm, 1%, discrete, to GND |
| `ISET` | 4.7 kOhm, 1%, discrete, to GND |
| `TS` | Approved 10 kOhm pack NTC, or conditional fixed 10 kOhm fallback |
| `TD` | GND |
| `TMR` | No-connect for internal timers |
| `PGOOD`, `CHG` | Optional; open-drain, never pulled above receiver rail |

The 1.2 kOhm ILIM target gives approximately 1.342 A typical and 1.433 A at
the data-sheet maximum factor. Do not fit the preliminary 3.6 kOhm value and
do not use 1.1 kOhm. The 4.7 kOhm ISET target gives approximately 189 mA
nominal fast charge.

The battery must be an approved protected 1S 4.2 V pack. It must permit at
least 200 mA charging, at least 1.5 A continuous discharge and at least 2 A
transient discharge, subject to higher first-article measurements. Prefer a
three-wire pack with a documented 10 kOhm-at-25-degrees-C NTC. Fixed-TS and
external-NTC builds are mutually exclusive assembly variants, selected with
fitted/DNP zero-ohm links or resistors, not an end-user jumper.

### 3.3 Regulated 3.3 V rail

Delete `ME6211C30M5G-N`, its bias resistor and its old local passives. Use
`TPS63031DSKR`, LCSC `C15516`, fixed at 3.3 V:

- `VIN` and `VINA` to `VSYS`;
- 22 uF at `VIN`, 100 nF at `VINA`;
- 1.5 uH `FTC252012S1R5MBCA`, LCSC `C5832371`, between `L1` and `L2`;
- `VOUT` and Kelvin `FB` to `VCC_REG`;
- 22 uF at `VCC_REG` before its isolation link;
- `PS/SYNC` to GND for power-save mode; and
- `EN` to `SYS_EN`.

Use `LK_VCC`, normally fitted, between `VCC_REG` and `VCC`. It is a flat
0603/0805 zero-ohm service link, not a pin-header jumper. Removing it isolates
the regulator output while leaving the regulator output capacitor and FB
connection intact. External 3.3 V injection goes to `INJ_VCC`, on the `VCC`
side, only while system power is off and `LK_VCC` is removed.

Do **not** use preliminary `MSK12C02` as the VCC isolation device: its listed
50 mA contact rating is too small to approve for the complete logic rail. A
frequently switched engineering build may substitute a separately qualified,
low-profile, top-actuated SMD switch rated for the measured current, but the
production design uses the resistor link.

### 3.4 Five-volt boost and branches

Retain `SY7069ADC` with:

- 1.5 uH `FTC252012S1R5MBCA`;
- 22 uF at the input;
- two parallel 22 uF capacitors at `BOOST_5V`; and
- the validated 470 kOhm / 150 kOhm feedback network for 5.0 V.

`LK_5V_SOURCE`, a normally fitted 1206 zero-ohm link rated for the complete
branch current, connects `BOOST_5V` to `+5V`. Remove it before applying an
external 5 V supply to `INJ_5V`. This is a low-profile resistor, not a header.

`LK_TRACKPOINT_5V`, normally fitted, isolates the TrackPoint supply from the
shared `+5V` bus. Use an 0805 or 1206 zero-ohm part with a documented current
rating. Each adapter also has `LK_BL_ADAPTER`, normally fitted, between its
incoming `BL_5V` bus and keyboard connector. These links are intended for
first-article fault isolation and current-path diagnosis; they are not
precision shunts.

### 3.5 Backlight branch

Delete preliminary Q2, Q3, R21, R24, R25 and C1. Use SGMicro
`SGM2562XN6G/TR`:

- `VIN` to `+5V`, with local 1 uF;
- `ON` to `NRF_BL_ENABLE` (`P1.12`);
- begin with 1 nF from `SS` to GND and validate startup;
- leave `QOD` open initially;
- place a 50 mOhm, 1%, 1206 Kelvin shunt after the switch;
- name the post-shunt rail `BL_5V`; and
- fit at least 22 uF plus 100 nF at `BL_5V` near the universal connector.

`KBD_BL_PWM` comes directly from `P1.04`, is push-pull 0-3.3 V and must idle
low. The rail enable and PWM are separate signals. Release firmware shall not
assume that any arbitrary duty is safe. The known T470 keyboard drew about
595 mA at 80% and 620 mA at 85% with 50 kHz PWM, and the test supply shut down
at 90%. Determine the final limit from Rev B measurements; during bring-up,
cap firmware at 70% or lower.

With no valid battery and only default USB current, both the backlight load
switch and PWM remain off. Batteryless operation is permitted only after a
valid 1.5 A/3 A advertisement and successful load validation. With a battery,
the BQ24073 reduces charging current before permitting battery supplement.

## 4. Passive consolidation and actual placement count

### 4.1 Array requirements

Use only four-element **isolated** arrays with eight terminals. Never
substitute bussed/common-pin networks. The initial package is 0402x4
(approximately 2.0 mm x 1.0 mm). Verify each manufacturer's pin numbering and
land pattern; similar package descriptions do not automatically prove
footprint interchangeability.

| Value | Candidate | LCSC | Use |
| ---: | --- | --- | --- |
| 100 kOhm x4 | `CAY10-104J4LF` | `C1731039` | Four of five keyboard-LED MOSFET gate-bias channels |
| 1 kOhm x4 | `EXB28V102JX` | `C192510` | Four of five keyboard-LED output-series channels |
| 10 kOhm x4 | `EXB28V103JX` | `C178757` | TrackPoint pull-ups and Type-C logic |
| 4.7 kOhm x4 | `741X083472JP` | `C1731191` | Three BLE profile LEDs after brightness validation |

### 4.2 Why the BOM no longer has nine 100 kOhm placements

The preliminary nine are:

| Preliminary group | Electrical elements | Revised disposition |
| --- | ---: | --- |
| Five keyboard-LED sink biases | 5 | One 100 kOhm x4 array plus one discrete: **2 placements** |
| System power latch | 1 | Removed; maintained switch drives `SYS_EN` |
| Old backlight Q2/Q3 network | 2 | Removed with Q2/Q3/SGM2562 conversion |
| Old LDO bias | 1 | Removed with ME6211/TPS63031 conversion |
| **Total** | **9 preliminary** | **5 remaining electrical resistors, 2 placements** |

The final grouped BOM must list the array and discrete lines separately. It
must not retain a generic `9 x 100 kOhm` line.

### 4.3 Other repeated-value consolidation

| Function | Electrical resistors | Revised placements |
| --- | ---: | ---: |
| Five 1 kOhm keyboard-LED series channels | 5 | One 1 kOhm x4 array + one discrete = 2 |
| TrackPoint 3.3 V RESET/CLOCK/DATA pull-ups | 3 | One 10 kOhm x4 array = 1 |
| TrackPoint 5 V RESET/CLOCK/DATA pull-ups | 3 | A separate 10 kOhm x4 array = 1 |
| Type-C OUT1, OUT2, EN2 and MCU-side pull resistors | 4 | One 10 kOhm x4 array = 1 |
| Three BLE profile LED resistors | 3 | One 4.7 kOhm x4 array = 1 |

The repeated signal groups therefore use six array placements total:

- one 100 kOhm array;
- one 1 kOhm array;
- three 10 kOhm arrays; and
- one 4.7 kOhm array.

Retain the following as individual, locally placed parts:

- the `SYS_EN` pulldown and any series resistors required after enable-pin
  threshold and absolute-maximum review;
- keyboard backlight-detect series resistor;
- BQ24073 ISET, ILIM, TS and optional status resistors;
- 900 kOhm HUSB320 PORT/DEBUG_N sink-role strap;
- 900 kOhm / 330 kOhm battery ADC divider and 100 nF filter;
- SY7069 feedback divider;
- SGM2562 SS capacitor;
- 50 mOhm backlight shunt;
- every zero-ohm isolation link; and
- every input, output, bypass and bulk capacitor.

Capacitors may share purchasing values but not physical packages. Keep local
decoupling at the relevant IC pins. Qualify the effective capacitance of every
22 uF 0603 part under DC bias before layout release.

The consolidation above is sufficient for Revision B. Do not replace further
passives merely to change manufacturer or force another array: do so only when
it reduces fitted placements without compromising local decoupling, tolerance,
power rating, voltage rating or debug isolation.

For ICs, an Asian-brand substitute is permitted only after its complete data
sheet has been compared against the selected part for function, pinout,
package/land pattern, absolute maximums, operating range, thresholds, current,
losses, startup/shutdown behavior, protection features and stability
requirements. Similar names and LCSC category matches are not qualification.

### 4.4 Completed semiconductor substitution review

| Candidate | Data-sheet result | Revision B disposition |
| --- | --- | --- |
| Hynetek `HUSB320-BA000-QN12R` vs TI `TUSB320LAI` | Same 12 functional pin positions and required sink/current-advertisement GPIO behavior. Hynetek supports 2.85-5.5 V VDD, -40 to +85 degC ambient, dead-battery Rd and 28 V-tolerant USB pins. Its QFN land pattern is not assumed identical. BA000 specifically selects direct/0 Ohm VBUS detection. | Approved with the Hynetek land pattern and direct `VBUS_DET`; do not fit the 866 kOhm resistor used by BA001. |
| HANSCHIP `74LVC1G14DBVRG` vs TI `SN74LVC1G14DBVR` | Function and SOT-23-5 pins match, but the reviewed HANSCHIP sheet specifies only -25 to +70 degC operation and provides typical rather than equivalent guaranteed propagation limits. | Rejected; retain TI. |
| SGMicro `SGM2562XN6G/TR` vs TI `TPS22918DBVR` | Pin-for-pin SOT-23-6 match: IN, GND, ON, SS/CT, QOD, OUT. Both support 2 A, 5.5 V, adjustable slew and QOD. At 5 V SGM specifies 89 mOhm maximum versus TI 79 mOhm; at the measured 620 mA load this difference is about 6.2 mV and 3.8 mW. A 1 nF SS capacitor gives about 2.4 ms typical rise time, close to the TI implementation. | Approved electrically; validate first-article temperature, startup and JLCPCB sourcing. |
| HXY `USBLC6-2SC6`, `C5261088`, vs ST `USBLC6-2SC6` | Same two-line rail-clamp topology and SOT-23-6 routing, 5 V working voltage, 6 A 8/20 us pulse rating and IEC +/-15 kV air, +/-8 kV contact rating. HXY specifies no more than 1 pF I/O-to-GND capacitance, below ST's 3.5 pF maximum, but permits 1 uA leakage versus ST's 150 nA maximum. | Approved for USB 2.0 with the HXY land pattern; verify enumeration and D+/D- signal quality on first articles. |

## 5. Core part list

This is the revised design list, not a purchase-ready BOM. Quantities marked
`DNP option` depend on the selected battery/status configuration.

### 5.1 ICs, semiconductors and magnetics

| Qty | Function | Part / target |
| ---: | --- | --- |
| 1 | MCU/module | `HOLYIOT-18010-NRF52840` |
| 1 | Charger/power path | `BQ24073RGTR`, LCSC `C15220` |
| 1 | Type-C current detector | Hynetek `HUSB320-BA000-QN12R`, LCSC `C7471906`, QFN-12 1.6 mm x 1.6 mm |
| 1 | Type-C mode inverter | TI `SN74LVC1G14DBVR`; retain TI after comparison because the reviewed HANSCHIP data sheet has only a -25 to +70 degC operating range and lacks equivalent guaranteed timing limits |
| 1 | 3.3 V buck-boost | `TPS63031DSKR`, LCSC `C15516` |
| 1 | 5 V boost | `SY7069ADC`, LCSC `C207630` |
| 1 | Backlight load switch | SGMicro `SGM2562XN6G/TR`; pin-compatible, data-sheet-qualified replacement for `TPS22918DBVR`; confirm JLCPCB sourcing before release |
| 5 | TrackPoint, LED and Type-C level-shift channels | UMW `BSS138PS`, LCSC `C5271194`, dual N-channel MOSFET in SOT-23-6; nine channels are used and one remains unconnected. Preliminary U6-U9 placeholder `44` is invalid |
| 2 | 1.5 uH inductors | `FTC252012S1R5MBCA`, LCSC `C5832371` |
| 1 | USB ESD | HXY `USBLC6-2SC6`, LCSC `C5261088` |
| 1 | VBUS TVS | `PTVS5V0S1UR,115` or qualified equivalent |
| 1 | Battery reverse/fault clamp | Qualified part retained from battery protection review |

### 5.2 Resistor arrays and precision/discrete resistors

| Qty | Value/type | Function |
| ---: | --- | --- |
| 1 | 100 kOhm x4 isolated array | Four LED sink gate biases |
| 1 | 100 kOhm discrete | Fifth LED sink bias |
| 1 | 1 kOhm x4 isolated array | Four LED series paths |
| 1 | 1 kOhm discrete | Fifth LED series path |
| 3 | 10 kOhm x4 isolated arrays | Two TrackPoint domains and one Type-C logic group |
| 1 base | 10 kOhm discrete | `-KBD_BL_DTCT` series path |
| 1 DNP option | 10 kOhm, 1% | Fixed TS fallback for approved two-wire battery |
| 1 | 4.7 kOhm x4 isolated array | Three profile LEDs; fourth element unused |
| 1 | 4.7 kOhm, 1% | BQ24073 ISET |
| 1 DNP option | 4.7 kOhm | Charge-status LED |
| 1 | 1.2 kOhm, 1% | BQ24073 ILIM |
| 2 | 900 kOhm, 1% | HUSB320 PORT/DEBUG_N sink-role strap and battery-divider upper leg |
| 1 | 330 kOhm, 1% | Battery-divider lower leg |
| 1 each | 470 kOhm, 150 kOhm, 1% | SY7069 feedback; confirm final output calculation |
| 1 | 50 mOhm, 1%, 1206 Kelvin-capable | Backlight current shunt |
| As listed below | Zero-ohm links | Flat service isolation; select current-rated parts |

### 5.3 Main capacitors

| Minimum qty | Value/rating | Function |
| ---: | --- | --- |
| 2 | 10 uF, 10 V, X5R/X7R | BQ24073 IN and BAT |
| 1 | 22 uF, 10 V, X5R/X7R | BQ24073 OUT |
| 2 | 22 uF, 10 V, X5R/X7R | TPS63031 input and output |
| 3 | 22 uF, 10 V, X5R/X7R | SY7069 input and two output capacitors |
| 1 | 1 uF | SGM2562 input |
| 1 | 22 uF + 1 x 100 nF | Backlight post-switch output |
| As local | 100 nF | HolyIOT, HUSB320, TPS63031 VINA, logic ICs and connector entries |
| 1 | Initial 1 nF | SGM2562 SS; adjust only after startup measurement |
| 1 | 100 nF | Battery ADC filter |

Do not derive the final capacitor quantity by simply grouping nominal values.
Complete the schematic, then recount every local bypass and bulk location.

### 5.4 Connectors and protection

| Qty | Function | Part / requirement |
| ---: | --- | --- |
| 1 | USB-C receptacle | `TYPE-C-31-M-12`, LCSC `C165948`, if footprint and current rating pass review |
| 2 | Core and adapter universal FFC connectors | Molex `5051106091`, LCSC/JLCPCB `C493444`; 60 positions, 0.5 mm pitch, bottom contact, right-angle SMT, ZIF/hinged lid, 1.9 mm height, 0.30 mm FFC/FPC, gold-plated phosphor-bronze contacts, 0.5 A per contact, 50 V, -40 to +105 degC. Use the manufacturer land pattern; do not assume footprint compatibility with HCTL `C2906127` or Hirose `C224193` |
| 1 | Universal FFC cable | Molex Premo-Flex `15018-0561` / `0150180561`; 60 circuits, 0.5 mm pitch, 76 mm long, Type A same-side contacts, 0.30 mm ends, gold plating, 0.5 A per conductor, 60 V AC, -40 to +105 degC. Confirm 76 mm length in the enclosure before release; use another Type-A 60-circuit Series 15018 length if required |
| 1 | Battery | Keyed connector with verified polarity and current rating |
| 1 | USB input PTC | At least 1.5 A hold after hot derating |
| 1 | Battery fuse | Retain/resize after approved-pack fault-current review |
| 1 | Debug | Tag-Connect or compact pogo pads; no fitted header |

## 6. Schematic wiring guide

This section is the drawing guide for the next Revision B schematic. Pin
numbers are physical package pins viewed according to the cited manufacturer
top-view drawings. Every library symbol and footprint must be checked against
the current manufacturer data sheet before routing. A net named here must keep
that name through the schematic; do not replace distinct rails with generic
power symbols.

### 6.1 USB-C receptacle, protection and USB data

| Receptacle function | Connection |
| --- | --- |
| All VBUS contacts | Join at the connector with wide copper -> `MP_USB_CONN_VBUS` -> F1 -> `USB_VBUS_RAW` |
| CC1 | HUSB320 pin 1 `CC1`; optional `MP_CC1` pad without a long stub |
| CC2 | HUSB320 pin 2 `CC2`; optional `MP_CC2` pad without a long stub |
| D+ contacts | Join at connector -> USB ESD array -> HolyIOT USB D+ pad |
| D- contacts | Join at connector -> USB ESD array -> HolyIOT USB D- pad |
| Shield/shell | Chassis/ground treatment selected at schematic review; provide short ESD return and do not send surge current through signal ground traces |
| GND contacts | Ground plane with multiple vias at connector |

Place the VBUS TVS from `USB_VBUS_RAW` to the connector-side ground return.
Place the USB ESD device beside the receptacle, before the D+/D- pair travels
across the board. The HolyIOT VBUS-detect input connects to
`USB_VBUS_RAW`; it is a sense input only and must not feed another rail.

Do not fit discrete 5.1 kOhm CC resistors when HUSB320 is installed.

### 6.2 HUSB320-BA000-QN12R Type-C detector

Use the Hynetek 12-pin QFN land pattern. The package is only 1.6 mm x 1.6 mm;
verify that JLCPCB accepts its land pattern and solder-mask rules. Although its
functional pin order matches TUSB320, verify the Hynetek package drawing rather
than copying a TI footprint without comparison.

| Pin | Name | Revision B connection |
| ---: | --- | --- |
| 1 | `CC1` | USB-C CC1 and `MP_CC1` |
| 2 | `CC2` | USB-C CC2 and `MP_CC2` |
| 3 | `PORT/DEBUG_N` | GND through discrete 900 kOhm, 1%, to sample UFP/sink-only mode; never hard-ground this pin because it changes to a push-pull output after initialization |
| 4 | `VBUS_DET` | Directly to `USB_VBUS_RAW`; BA000 requires the 0 Ohm external-path configuration, not the BA001-only 866 kOhm path |
| 5 | `ADDR/ORIENT` | No-connect for GPIO mode; no copper pull-up/down |
| 6 | `INT_N/OUT3` | No-connect unless later accessory detection is explicitly required |
| 7 | `SDA/OUT1` | `TYPEC_OUT1`; 10 kOhm pull-up element to `TYPEC_VDD`; also BQ24073 EN1, inverter input and 5 V side of MCU level shifter |
| 8 | `SCL/OUT2` | `TYPEC_OUT2`; 10 kOhm pull-up element to `TYPEC_VDD`; measurement only |
| 9 | `ID` | No-connect in sink-only use |
| 10 | `GND` | Ground plane; local return for pin 12 bypass |
| 11 | `EN_N` | GND; active-low enable |
| 12 | `VDD` | `TYPEC_VDD`; 100 nF to GND at pins 12/10 |

`TYPEC_VDD` is `USB_VBUS_RAW` after a low-drop Schottky diode. Place
`MP_TYPEC_VDD`, `MP_TYPEC_OUT1` and `MP_TYPEC_OUT2` beside this block.

### 6.3 SN74LVC1G14DBVR Type-C mode inverter

Use the five-pin DBV/SOT-23 top-view pinout; this part does **not** use the
common one-gate convention with input on pin 1.

| Pin | Name | Revision B connection |
| ---: | --- | --- |
| 1 | NC | No functional connection; retain a solderable pad |
| 2 | `A` | `TYPEC_OUT1` |
| 3 | GND | Ground plane |
| 4 | `Y` | `BQ_EN2`; 10 kOhm pull-down element to GND and `MP_BQ_EN2` |
| 5 | VCC | `TYPEC_VDD`; 100 nF to GND beside pins 5/3 |

The hardware result must be `BQ_EN2 = NOT TYPEC_OUT1`. Connect
`TYPEC_OUT1` directly to BQ EN1, so EN1 and EN2 always select either USB500
or resistor-programmed mode; they must not float during startup.

### 6.4 BQ24073RGTR charger and power path

Use the RGT 16-pin VQFN top-view footprint with exposed thermal pad.

| Pin | Name | Revision B connection |
| ---: | --- | --- |
| 1 | `TS` | `BAT_TS`; selected by mutually exclusive `SJ_TS_NTC` or fixed 10 kOhm TS assembly |
| 2 | `BAT` | Join directly to pin 3 and `BAT_PROTECTED`; local 10 uF to GND |
| 3 | `BAT` | Join directly to pin 2 and `BAT_PROTECTED` |
| 4 | `CE` | GND for charge enabled |
| 5 | `EN2` | `BQ_EN2` from inverter pin 4; `MP_BQ_EN2` |
| 6 | `EN1` | `TYPEC_OUT1`; `MP_BQ_EN1` |
| 7 | `PGOOD` | DNP option: 10 kOhm pull-up to `VCC` and MCU/status destination; otherwise NC |
| 8 | `VSS` | Ground plane and direct local capacitor returns |
| 9 | `CHG` | DNP option: LED plus discrete resistor to `VCC`, or 10 kOhm pull-up to an MCU input; otherwise NC |
| 10 | `OUT` | Join directly to pin 11 and `VSYS`; local 22 uF to GND |
| 11 | `OUT` | Join directly to pin 10 and `VSYS` |
| 12 | `ILIM` | Discrete 1.2 kOhm, 1%, to GND; short quiet route and `MP_BQ_ILIM` |
| 13 | `IN` | `USB_VBUS_RAW`; local 10 uF to GND and `MP_BQ_IN` |
| 14 | `TMR` | No-connect for internal timer defaults |
| 15 | `TD` | GND to enable normal termination |
| 16 | `ISET` | Discrete 4.7 kOhm, 1%, to GND; short quiet route |
| pad | thermal pad | Ground plane with TI paste window and thermal vias |

Place `MP_BAT_PROTECTED` at the battery capacitor and `MP_VSYS` at the OUT
capacitor. Do not join `BAT_PROTECTED` and `VSYS` anywhere outside the IC.

### 6.5 Battery connector, protection, ADC and TS variants

```text
JBAT BAT+ -> battery fuse -> reverse/fault protection -> BAT_PROTECTED
JBAT BAT- -----------------------------------------------------> GND
JBAT NTC (three-wire variant) -> SJ_TS_NTC -> BAT_TS/BQ TS

BAT_PROTECTED -> LK_BAT_ADC -> 900k ->+-> P0.02/AIN0
                                       +-> 330k -> GND
                                       +-> 100nF -> GND
```

For a fixed-temperature two-wire build, do not fit `SJ_TS_NTC`; fit the
approved fixed 10 kOhm TS resistor/selector specified by the final BQ24073 TS
network. For a three-wire NTC build, do not fit the fixed fallback. The final
schematic shall show two explicit BOM variants and an assembly warning that
both variants must never be populated simultaneously.

### 6.6 System enable switch

Do not fit a system P-channel MOSFET. `VSYS` feeds both converter inputs
directly, while the maintained POWER switch controls their enable pins through
the common `SYS_EN` net.

| Item | Connection |
| --- | --- |
| Maintained POWER switch common | Qualified enable-high source |
| Maintained POWER switch ON | `SYS_EN` asserted |
| Maintained POWER switch OFF | Open; pulldown forces `SYS_EN` low |
| `SYS_EN` pulldown | Value selected to guarantee both converters are disabled while limiting ON-state loss |
| TPS63031 `EN` | `SYS_EN`, directly or through a small series resistor if required by final review |
| SY7069 `EN` | `SYS_EN`, directly or through a small series resistor if required by final review |
| Measurement | `MP_VSYS` and `MP_SYS_EN` |

Before schematic release, compare both converter data sheets and select an
enable-high source that remains within both EN-pin absolute maximum ratings at
USB-plus-battery and battery-only extremes. Verify VIH/VIL margins, shutdown
current and absence of back-powering in both switch positions. Add a normally
open two-pad `SJ_SYS_EN_FORCE` from the qualified enable-high source to
`SYS_EN` for bring-up; do not fit a header or production jumper.

### 6.7 TPS63031DSKR 3.3 V buck-boost

Use the ten-pin DSK VSON top-view pinout.

| Pin | Name | Revision B connection |
| ---: | --- | --- |
| 1 | `VOUT` | `VCC_REG`; 22 uF to PGND; `FB` Kelvin sense; `MP_VCC_REG`; then `LK_VCC` to `VCC` |
| 2 | `L2` | One side of 1.5 uH inductor; switching node only |
| 3 | `PGND` | Ground plane and local input/output capacitor returns |
| 4 | `L1` | Other side of 1.5 uH inductor; switching node only |
| 5 | `VIN` | `VSYS`; local 22 uF to PGND |
| 6 | `EN` | `SYS_EN`; never leave floating |
| 7 | `PS/SYNC` | GND for power-save mode |
| 8 | `VINA` | `VSYS`; local 100 nF to signal GND |
| 9 | `GND` | Signal ground beside exposed pad |
| 10 | `FB` | Direct Kelvin connection to `VCC_REG` at the output capacitor; no divider for fixed 3.3 V part |
| pad | thermal pad | PGND/ground plane with TI land pattern |

`LK_VCC` separates `VCC_REG` from `VCC`. Put `MP_VCC` and `INJ_VCC` on the
`VCC` side. Do not place the feedback connection or required output capacitor
on the injectable side of the link.

### 6.8 SY7069ADC 5 V boost

Use the TSOT-23-6 top-view pinout. The preliminary schematic symbol and
footprint must be checked against this table.

| Pin | Name | Revision B connection |
| ---: | --- | --- |
| 1 | `FB` | Divider midpoint: 470 kOhm from `BOOST_5V`, 150 kOhm to GND |
| 2 | `IN` | `VSYS`; local 22 uF to GND; one side of 1.5 uH inductor |
| 3 | `GND` | Ground plane and local capacitor returns |
| 4 | `OUT` | `BOOST_5V`; two local 22 uF capacitors to GND; `MP_BOOST_5V` |
| 5 | `LX` | Other side of 1.5 uH inductor; switching node only |
| 6 | `EN` | `SYS_EN`; never leave floating |

`BOOST_5V` then passes through normally fitted `LK_5V_SOURCE` to `+5V`.
`MP_5V` and `INJ_5V` are on the `+5V` side. The 470 kOhm / 150 kOhm values
produce approximately 4.96 V from the nominal 1.2 V reference; confirm
tolerance against the final keyboard voltage requirement.

### 6.9 SGM2562XN6G/TR backlight load switch

Use the six-pin SOT-23 top-view pinout. The SGM2562 pin assignment matches the
TPS22918DBVR pin-for-pin, but the schematic symbol shall use the SGMicro pin
names and its footprint shall be checked against the SGMicro package drawing.

| Pin | Name | Revision B connection |
| ---: | --- | --- |
| 1 | `VIN` | `+5V`; local 1 uF to GND and `MP_BL_SWITCH_IN` |
| 2 | GND | Ground plane |
| 3 | `ON` | `NRF_BL_ENABLE` from HolyIOT `P1.12`; must not float; `MP_BL_ENABLE` |
| 4 | `SS` | Initial discrete 1 nF to GND; no other load |
| 5 | `QOD` | No-connect for initial build |
| 6 | `VOUT` | `BL_SWITCH_OUT` -> 50 mOhm Kelvin shunt -> `BL_5V` |

Connect `KL_BL_CURRENT_P` directly to the switch-side shunt terminal and
`KL_BL_CURRENT_N` directly to the `BL_5V` terminal. Place 22 uF plus 100 nF
from `BL_5V` to GND after the shunt. Put `MP_BL_5V` at those capacitors.

### 6.10 Five-volt branch isolation and injection

```text
BOOST_5V -- LK_5V_SOURCE -- +5V --+-- LK_TRACKPOINT_5V -- TRACKPOINT_5V
                                      |
                                      +-- SGM2562 IN

INJ_5V -------------------------------- +5V side of LK_5V_SOURCE
INJ_GND ------------------------------- GND
```

The universal connector's `+5V` pins connect to `TRACKPOINT_5V`, not directly
to `BOOST_5V`. Keep the branch name distinct so removing
`LK_TRACKPOINT_5V` really isolates the keyboard TrackPoint circuit.

### 6.11 TrackPoint level shifting

Implement nine BSS138 channels total using five UMW `BSS138PS` packages (LCSC
`C5271194`): three TrackPoint channels, five keyboard-LED sink channels and one
Type-C MCU level-shift channel. Five dual packages provide ten channels; leave
the unused channel unconnected at all signal terminals.

For each TrackPoint RESET, CLOCK and DATA channel:

| MOSFET terminal | Connection |
| --- | --- |
| Gate | `VCC` 3.3 V |
| Source | nRF-side `NRF_TP4_*` net with 10 kOhm pull-up element to `VCC` |
| Drain | keyboard-side `TP4_*` net with 10 kOhm pull-up element to `TRACKPOINT_5V` |

The two groups of three pull-ups use two different 10 kOhm x4 arrays. Do not
share an array element or supply rail across the two voltage domains. Keep the
fourth element of each array unconnected at both ends.

For the Type-C MCU level shift:

| MOSFET terminal | Connection |
| --- | --- |
| Gate | `VCC` |
| Drain | 5 V-domain `TYPEC_OUT1` |
| Source | `TYPEC_HIGH_CURRENT_N`, with the fourth element of the Type-C 10 kOhm array pulling to `VCC` |

Do not connect `TYPEC_OUT1` directly to the nRF52840.

The physical `BSS138PS` pin assignment is:

| Channel | Source | Gate | Drain |
| --- | ---: | ---: | ---: |
| 1 | 1 | 2 | 6 |
| 2 | 4 | 5 | 3 |

Use separate schematic units tied to a verified SOT-23-6 footprint with this
pad numbering. Do not use the ordinary three-pin BSS138 symbol or footprint.
The device is rated 60 V and 300 mA, with RDS(on) up to 2.2 Ohm at VGS = 4.5 V.
It is appropriate for these logic/LED-sink functions, not for the keyboard
backlight power path.

### 6.12 Keyboard LED sinks and profile LEDs

There are five independent keyboard LED sink outputs: T430 power, T470 Fn
Lock, speaker mute, microphone mute and T470 Caps Lock. For each channel:

```text
HolyIOT LED-drive GPIO (high = sink enabled) ----> BSS138 gate
                                      |
                                      +-- 100 kOhm -> GND

BSS138 source -> GND
BSS138 drain  -> 1 kOhm -> universal active-low LED net
```

This reproduces the preliminary channel topology explicitly: the 100 kOhm is
a gate pull-down, not a series GPIO resistor. Verify that the firmware
polarity and GPIO reset state keep every LED off during boot. The five
100 kOhm elements use one x4 array plus one discrete, and the five 1 kOhm
elements use one x4 array plus one discrete. Do not merge the five output
nets.

Each BLE profile indicator is wired `VCC -> one 4.7 kOhm array element -> LED
anode -> LED cathode -> HolyIOT GPIO`, so the GPIO sinks current and the LED is
active low. Leave the fourth array element unconnected. Confirm LED polarity
in the footprint rather than relying on symbol orientation.

### 6.13 Universal connector power and local adapter wiring

On the core connector, join parallel contacts at the pads:

- pins 42-43 -> `TRACKPOINT_5V`;
- pins 51-54 -> `BL_5V`;
- pins 56, 57 and 59 -> `VCC`; and
- all assigned ground contacts -> ground plane with local vias.

The four parallel `BL_5V` contacts have a 2.0 A nominal aggregate connector
and cable rating when the specified 0.5 A/contact Molex matched system is used. The observed
approximately 620 mA backlight load is therefore about 155 mA per contact if
current divides evenly. Merge the four contacts immediately into a common
pour on both boards, provide at least four low-impedance ground contacts, and
verify current sharing and temperature rise on the first article. Do not route
the backlight through only one of the contacts.

On each adapter:

- place at least 22 uF plus 100 nF from TrackPoint 5 V to GND beside the
  keyboard/TrackPoint connector;
- place 100 nF from `VCC` to GND beside each keyboard logic supply entry;
- route incoming `BL_5V` through normally fitted `LK_BL_ADAPTER` before the
  keyboard backlight pins; and
- place `MP_ADAPTER_5V`, `MP_ADAPTER_BL_5V`, `MP_ADAPTER_BL_PWM` and a ground
  pad beside the keyboard connector.

Do not duplicate the 50 mOhm current shunt on an adapter.

### 6.14 Schematic-sheet connection checklist

Before layout, print a net cross-reference and confirm:

1. `USB_VBUS_RAW` appears only after F1 and feeds BQ IN, Type-C supply diode,
   HUSB320 VBUS_DET resistor and HolyIOT VBUS sense.
2. `VSYS` connects BQ OUT directly to both converter inputs and their required
   local capacitors; it does not directly feed a keyboard load.
3. `SYS_EN` reaches both converter enable pins, its pulldown,
   `SJ_SYS_EN_FORCE`, the maintained switch and `MP_SYS_EN` only.
4. `BOOST_5V` reaches only the boost capacitors, feedback divider,
   `MP_BOOST_5V` and source side of `LK_5V_SOURCE`.
5. `+5V` reaches the injection pads, TrackPoint branch link and SGM2562 IN.
6. `TRACKPOINT_5V` reaches only the translator pull-ups and universal
   TrackPoint supply pins.
7. `BL_5V` begins after SGM2562 and the Kelvin shunt.
8. `VCC_REG` contains TPS63031 VOUT, FB and output capacitor before `LK_VCC`.
9. `VCC` contains the HolyIOT, 3.3 V logic, keyboard logic rail and external
   injection pad after `LK_VCC`.
10. Every `MP_`, `KL_`, `LK_`, `SJ_` and `INJ_` item in Section 10 exists in
    the schematic and has an intentional footprint.

## 7. HolyIOT wiring

### 7.1 Matrix

| Net | GPIO | Net | GPIO |
| --- | --- | --- | --- |
| `DRV0` | `P1.03` | `DRV8` | `P0.16` |
| `DRV1` | `P0.19` | `DRV9` | `P1.00` |
| `DRV2` | `P1.09` | `DRV10` | `P0.23` |
| `DRV3` | `P0.31` | `DRV11` | `P1.02` |
| `DRV4` | `P1.10` | `DRV12` | `P1.07` |
| `DRV5` | `P1.15` | `DRV13` | `P0.24` |
| `DRV6` | `P0.29` | `DRV14` | `P1.06` |
| `DRV7` | `P0.05` | `DRV15` | `P1.05` |
| `SENSE0` | `P0.03` | `SENSE4` | `P0.04` |
| `SENSE1` | `P0.07` | `SENSE5` | `P1.13` |
| `SENSE2` | `P0.30` | `SENSE6` | `P0.12` |
| `SENSE3` | `P0.28` | `SENSE7` | `P0.21` |

All drives are active-low open-drain outputs in exact `DRV0`-`DRV15` order.
All senses are active-low inputs with nRF internal pull-ups in exact
`SENSE0`-`SENSE7` order. Fit no external matrix pull-up arrays.

### 7.2 Direct and auxiliary functions

| Function | GPIO |
| --- | --- |
| Fn / `-HOTKEY` | `P1.11` |
| T430 `-PWRSWITCH` | `P0.22` |
| T470 left/right/middle buttons | `P0.08`, `P0.06`, `P0.26` |
| `-KBD_BL_DTCT` | `P0.10` through discrete 10 kOhm series resistor |
| `TYPEC_HIGH_CURRENT_N` | `P0.09` through 3.3 V level shift |
| Battery ADC | `P0.02/AIN0` through 900 kOhm / 330 kOhm divider |
| TrackPoint reset/clock/data | `P0.13`, `P0.14`, `P0.15` through 5 V translators |
| Speaker/microphone mute LED sinks | `P0.20`, `P0.17` |
| T470 Fn-lock/Caps-lock sinks | `P0.27`, `P1.14` |
| T430 power LED sink | `P1.01` |
| BLE profile LEDs 1/2/3 | `P1.08`, `P0.11`, `P0.25` |
| Backlight rail enable | `P1.12` |
| Backlight PWM | `P1.04` |

Set `CONFIG_NFCT_PINS_AS_GPIOS=y` for `P0.09` and `P0.10`.

## 8. Universal 60-contact adapter interface

The core and adapter use top-side bottom-contact connectors facing each other,
an untwisted Type-A FFC and a 180-degree rotated adapter connector. Therefore:

```text
core physical pad n -> adapter physical pad (61 - n)
```

| Core pins | Adapter pins | Signals |
| --- | --- | --- |
| 1 | 60 | GND |
| 2-17 | 59-44 | `DRV0`-`DRV15` in ascending core-pin order |
| 18 | 43 | GND |
| 19-26 | 42-35 | `SENSE0`-`SENSE7` |
| 27 | 34 | GND |
| 28 | 33 | `-HOTKEY` |
| 29 | 32 | `-PWRSWITCH` |
| 30-32 | 31-29 | `TP4LEFT`, `TP4RIGHT`, `TP4MIDDLE` |
| 33 | 28 | `KBD_ID` |
| 34 | 27 | Reserved, no-connect |
| 35 | 26 | `-KBD_BL_DTCT` |
| 36 | 25 | GND |
| 37 | 24 | `TP4_DATA` |
| 38 | 23 | GND guard |
| 39 | 22 | `TP4_CLOCK` |
| 40 | 21 | GND guard |
| 41 | 20 | `TP4_RESET`, active high |
| 42-43 | 19-18 | Parallel `+5V` TrackPoint supply |
| 44 | 17 | GND |
| 45 | 16 | T430 `-LEDPWR` |
| 46 | 15 | T470 `-LED_FNLOCK` |
| 47 | 14 | `-LED_MUTE` |
| 48 | 13 | `-LEDMICMUTE` |
| 49 | 12 | T470 `-LED_CAPSLOCK` |
| 50 | 11 | `KBD_BL_PWM` |
| 51-54 | 10-7 | Four parallel `BL_5V` contacts |
| 55 | 6 | GND |
| 56-57 | 5-4 | Parallel `VCC` contacts |
| 58 | 3 | GND |
| 59 | 2 | Third `VCC` contact |
| 60 | 1 | GND |

The connector's undocumented per-contact current rating remains a **release
blocker**. At 620 mA, four backlight contacts carry about 155 mA each before
imbalance. Obtain a written rating or select another connector, then validate
temperature and voltage drop in the final cable assembly.

TrackPoint RESET, CLOCK and DATA each use a BSS138 bidirectional level-shifter
channel with a 10 kOhm pull-up on both voltage domains. `TP4_RESET` is active
high: drive the nRF side high for about 600 ms to assert reset, then low to
release it before PS/2 communication.

## 9. Adapter requirements

Both adapters are passive. They contain connectors, local decoupling,
isolation links and measurement pads only. Do not duplicate TrackPoint level
shifters, LED sinks or the backlight load switch.

### 9.1 T430 critical mapping

- T430 J7 pin 6 is `DRV8` and reaches HolyIOT `P0.16`, not `P0.02`.
- J7 pin 21 is `-KBD_BL_DTCT`.
- J7 pins 29 and 31 are `BL_5V`.
- J7 pins 37, 39 and 40 are `TP4_DATA`, `TP4_CLOCK` and `TP4_RESET`.
- J7 pin 38 is TrackPoint `+5V`.
- J7 pin 25 is `KBD_BL_PWM`.
- J7 pins 35 and the required parallel supply contacts receive `VCC`.

The complete matrix connector order remains the known T430 mapping; only the
HolyIOT destination for `DRV8` changes from Revision A.

#### T430 J7 matrix mapping

| Signal | J7 pin | Signal | J7 pin |
| --- | ---: | --- | ---: |
| `DRV0` | 22 | `DRV8` | 6 |
| `DRV1` | 18 | `DRV9` | 20 |
| `DRV2` | 14 | `DRV10` | 16 |
| `DRV3` | 10 | `DRV11` | 24 |
| `DRV4` | 2 | `DRV12` | 28 |
| `DRV5` | 4 | `DRV13` | 32 |
| `DRV6` | 8 | `DRV14` | 26 |
| `DRV7` | 12 | `DRV15` | 30 |
| `SENSE0` | 5 | `SENSE4` | 11 |
| `SENSE1` | 13 | `SENSE5` | 3 |
| `SENSE2` | 9 | `SENSE6` | 15 |
| `SENSE3` | 7 | `SENSE7` | 17 |

#### T430 J7 auxiliary mapping

| J7 pin | Signal | J7 pin | Signal |
| ---: | --- | ---: | --- |
| 1 | `-HOTKEY` | 19 | `-PWRSWITCH` |
| 21 | `-KBD_BL_DTCT` | 23 | `-LEDPWR` |
| 25 | `KBD_BL_PWM` | 27 | GND |
| 29 | `BL_5V` | 31 | `BL_5V` |
| 33 | `-LED_MUTE` | 34 | GND |
| 35 | `VCC` | 36 | `-LEDMICMUTE` |
| 37 | `TP4_DATA` | 38 | `+5V` |
| 39 | `TP4_CLOCK` | 40 | `TP4_RESET` |
| 41 | GND | 42 | GND |
| 43 | GND | 44 | GND |

### 9.2 T470 critical mapping

The straight 40-pin keyboard connector maps FPC pin to keyboard contact plus
two. Important pins are:

| FPC pin | Signal | FPC pin | Signal |
| ---: | --- | ---: | --- |
| 27 | `VCC` | 28 | `-LED_FNLOCK` |
| 29 | `-LED_MUTE` | 30 | `-LEDMICMUTE` |
| 31 | `-HOTKEY` | 32 | GND |
| 33 | `-LED_CAPSLOCK` | 34 | GND |
| 35 | `TP4LEFT` | 36 | `TP4RIGHT` |
| 37 | `TP4MIDDLE` | 38 | `KBD_ID`, unimplemented on core |

The complete T470 matrix portion is:

| FPC pin | Signal | FPC pin | Signal |
| ---: | --- | ---: | --- |
| 3 | `SENSE3` | 4 | `SENSE7` |
| 5 | `SENSE6` | 6 | `DRV14` |
| 7 | `SENSE4` | 8 | `SENSE1` |
| 9 | `DRV0` | 10 | `SENSE2` |
| 11 | `SENSE0` | 12 | `DRV4` |
| 13 | `DRV2` | 14 | `SENSE5` |
| 15 | `DRV1` | 16 | `DRV3` |
| 17 | `DRV6` | 18 | `DRV7` |
| 19 | `DRV5` | 20 | `DRV15` |
| 21 | `DRV13` | 22 | `DRV9` |
| 23 | `DRV12` | 24 | `DRV10` |
| 25 | `DRV8` | 26 | `DRV11` |

FPC pins 1-2 and 39-40 are no-connect. Shield tabs 41-42 connect to the ground
plane.

T470 J37 is:

| J37 pin | Signal | J37 pin | Signal |
| ---: | --- | ---: | --- |
| 1 | `TP4_DATA` | 2 | `+5V` |
| 3 | GND | 4 | `TP4_RESET` |
| 5 | `TP4MIDDLE` | 6 | `TP4RIGHT` |
| 7 | `TP4LEFT` | 8 | `+5V` |
| 9 | `TP4_CLOCK` | 10 | `BL_5V` |
| 11 | `KBD_BL_PWM` | 12 | `-KBD_BL_DTCT` |

## 10. Measurement, isolation and injection access

### 10.1 Pad construction

- Ordinary `MP_` pads: 1.0-1.5 mm round or rounded-rectangle exposed ENIG
  copper, no paste, with at least 0.5 mm solder-mask clearance where possible.
- Ground probe pads: at least 2 mm, distributed beside each power block.
- `KL_` pairs: equal geometry and independently routed sense traces directly
  to the two shunt terminations; do not tap the high-current pours remotely.
- `INJ_5V` and `INJ_GND`: at least two parallel 2.5 mm x 4 mm flat pads per
  polarity at a board edge, plus optional unpopulated 1.0 mm plated holes for
  temporary soldered wires. Rate the combined copper path for 1.5 A.
- `INJ_VCC`: one 2 mm pad and adjacent ground pad; current-limited debugging
  only.
- Keep all pads unpopulated in production. This adds no component height.

### 10.2 Mandatory core measurement pads

| Measurement pad | Net/location | Diagnostic purpose |
| --- | --- | --- |
| `MP_USB_CONN_VBUS` | Connector side of F1 | Cable/source voltage |
| `MP_USB_VBUS_RAW` | Charger side of F1 | F1 drop and protected VBUS |
| `MP_TYPEC_VDD` | HUSB320 supply | Detector supply and diode drop |
| `MP_CC1`, `MP_CC2` | CC nets near detector | Attach/current-mode diagnosis; keep stubs short |
| `MP_TYPEC_OUT1`, `MP_TYPEC_OUT2` | Detector outputs | Verify advertisement truth table |
| `MP_BQ_EN1`, `MP_BQ_EN2` | Charger mode pins | Verify actual input-limit selection |
| `MP_BQ_ILIM` | ILIM pin/resistor node | Check soldering/value; high-impedance probing only |
| `MP_BQ_IN` | Charger IN pin | Local input droop |
| `MP_BAT_PROTECTED` | Battery after protection | Pack/protection behavior |
| `MP_VSYS` | BQ OUT | Charger system output |
| `MP_SYS_EN` | Common converter-enable net | Maintained-switch and shutdown diagnosis |
| `MP_VCC_REG` | TPS63031 output before `LK_VCC` | Regulator-only diagnosis |
| `MP_VCC` | Logic rail after `LK_VCC` | Delivered 3.3 V and injection check |
| `MP_BOOST_5V` | Before `LK_5V_SOURCE` | Converter-only diagnosis |
| `MP_5V` | After `LK_5V_SOURCE` | Shared 5 V bus and injection check |
| `MP_TRACKPOINT_5V` | After `LK_TRACKPOINT_5V` | TrackPoint branch voltage |
| `MP_BL_SWITCH_IN` | SGM2562 IN | Load-switch input loss |
| `KL_BL_CURRENT_P`, `KL_BL_CURRENT_N` | Shunt terminals | Backlight current: `(VP - VN) / 0.05` |
| `MP_BL_5V` | Post-shunt output | Voltage delivered to adapter |
| `MP_BL_ENABLE` | SGM2562 ON | Rail sequencing |
| `MP_BL_PWM` | `KBD_BL_PWM` | Frequency, duty and idle state |
| `MP_TP4_RESET` | Keyboard side reset | Active-high reset waveform |
| `MP_TP4_CLOCK`, `MP_TP4_DATA` | Keyboard-side PS/2 | TrackPoint communication |
| `MP_GND_USB`, `MP_GND_BQ`, `MP_GND_3V3`, `MP_GND_5V` | Local grounds | Short probe returns |

### 10.3 Mandatory isolation/configuration links

| Link | Default | Size | Open/remove when |
| --- | --- | --- | --- |
| `LK_VCC` | Fitted 0 Ohm | 0603/0805, current-qualified | Injecting external 3.3 V or isolating logic load |
| `LK_5V_SOURCE` | Fitted 0 Ohm | 1206, current-qualified | Injecting external 5 V or isolating boost |
| `LK_TRACKPOINT_5V` | Fitted 0 Ohm | 0805/1206 | Isolating a suspected TrackPoint/keyboard fault |
| `LK_BL_ADAPTER` | Fitted 0 Ohm on adapter | 1206 | Isolating keyboard backlight/cable fault |

`SJ_SYS_EN_FORCE` is a normally open assembly/debug selector rather than an
isolation link. Bridge it with solder only during bring-up when both converter
data sheets confirm the selected enable-high source is safe.
| `LK_BAT_ADC` | Fitted 0 Ohm | 0603 | Separating ADC divider leakage or a shorted ADC input |
| `SJ_TS_FIXED` | Variant-dependent | 0603 0 Ohm/DNP | Select only fixed-TS build |
| `SJ_TS_NTC` | Variant-dependent | 0603 0 Ohm/DNP | Select only external-NTC build |

`SJ_TS_FIXED` and `SJ_TS_NTC` must never both be fitted. Use an assembly note
and BOM variants; they are not user controls.

### 10.4 Injection rules

For 5 V injection:

1. Remove `LK_5V_SOURCE`.
2. Confirm no continuity from `INJ_5V` to `BOOST_5V`.
3. Start the external supply at 5.0 V and 100 mA current limit.
4. Connect ground first, then 5 V.
5. Raise the limit only after checking `MP_5V`, `MP_TRACKPOINT_5V` and
   `MP_BL_5V`.

For 3.3 V injection:

1. Turn system power off and remove `LK_VCC`.
2. Confirm `VCC_REG` and `VCC` are isolated.
3. Inject at `INJ_VCC` with a 100 mA initial limit.
4. Do not turn system power on until the external source is removed and
   `LK_VCC` is restored.

Never inject power through an ordinary `MP_` pad, a SWD reference pin or a
zero-ohm resistor that remains connected to another active source.

## 11. Layout requirements

### 11.1 JLCPCB stack-up and routing rules

The baseline fabrication assumption is a four-layer rigid FR-4 board with
1 oz outer copper, JLCPCB's standard inner copper, and a continuous ground
plane immediately below the component-side routing layer. Confirm the chosen
stack-up in the order before final USB routing. Do not silently change to 2 oz:
JLCPCB's minimum width/spacing rules become larger and every fine-pitch area
must be rechecked.

JLCPCB can manufacture much narrower tracks than this design should use. Its
published regular 1 oz capability reaches 0.10/0.10 mm on one- and two-layer
boards and 0.09/0.09 mm on multilayer boards, with approximately +/-20% trace
width tolerance. Those are manufacturing limits, not Rev B routing targets.

Use these project net classes on 1 oz outer copper:

| Net class | Minimum width | Preferred width/pour | Clearance | Notes |
| --- | ---: | ---: | ---: | --- |
| General GPIO, matrix, LED control | 0.15 mm | 0.20 mm | 0.15 mm | Use 0.15 mm only for package/connector escape |
| TrackPoint DATA/CLOCK/RESET | 0.20 mm | 0.25 mm | 0.20 mm | Continuous ground reference; no long PWM-parallel runs |
| CC1, CC2 and Type-C logic | 0.20 mm | 0.25 mm | 0.20 mm | Keep CC test-pad stubs extremely short |
| USB D+/D- | From JLC impedance calculator | From selected stack-up | From calculator | Route as 90 Ohm differential, same layer/reference, minimal vias and stubs |
| `VCC` / `VCC_REG` trunk | 0.50 mm | 0.75 mm | 0.20 mm | Branches to individual logic pins may neck to 0.20-0.25 mm |
| `USB_VBUS_RAW`, BQ IN/OUT, `VSYS`, `BAT_PROTECTED` | 1.50 mm | 2.00 mm or solid pour | 0.20 mm | Designed around approximately 1.43 A USB limit and battery transient margin |
| `VSYS` branches to either converter | 1.50 mm | 2.00 mm or solid pour | 0.20 mm | Keep each converter input loop short |
| `BOOST_5V`, `+5V`, TrackPoint 5 V | 1.00 mm | 1.50 mm or solid pour | 0.20 mm | Includes source and isolation-link necks |
| `BL_5V` and backlight-switch path | 1.00 mm | 1.50 mm or solid pour | 0.20 mm | Known load is at least 620 mA before final margin |
| `INJ_5V` path | 2.00 mm | Broad pour | 0.25 mm | Rate pads, links and copper for at least 1.5 A |
| Kelvin sense traces | 0.15 mm | 0.15-0.20 mm matched pair | 0.15 mm | No load current; connect directly to shunt terminals |

At 35 um copper, a 1 mm-wide trace is approximately 0.49 mOhm per millimetre
before plating and temperature effects. A 50 mm, 1 mm-wide path is therefore
about 25 mOhm and loses about 16 mV at 620 mA or 35 mV at 1.43 A. Width must be
selected from the complete round-trip length and allowed drop, not current
temperature-rise alone. Prefer pours because connector, link, MOSFET and cable
losses already consume the voltage budget.

For vias:

- use 0.30 mm finished drill / at least 0.60 mm pad as the normal robust via;
- never carry USB input, battery, VSYS or converter input current through one
  via: use at least three in parallel whenever those nets change layers;
- use at least two parallel vias for `+5V` or `BL_5V` layer changes, and more
  where space permits;
- place at least two ground vias immediately beside every high-current bypass
  capacitor; and
- use each IC manufacturer's thermal-via pattern under exposed pads rather
  than treating ordinary signal-via rules as a thermal design.

The universal FFC's 0.35 mm pads necessarily create short neck-downs. Keep
each `BL_5V`, `VCC`, `+5V` and ground pad escape at pad width for no more than
approximately 1 mm, then merge parallel contacts directly into the relevant
wide pour. Do not route one contact through another contact's pad.

Use at least 0.30 mm copper clearance to routed/milled board edges and slots.
Use 0.20 mm or greater around ordinary vias and 0.35 mm from PTH edges to
tracks where possible. These project rules intentionally exceed JLCPCB's
absolute minima for yield and tolerance margin.

1. Place HUSB320 beside USB-C; keep CC traces short and symmetric.
2. Place BQ24073, its IN/BAT/OUT capacitors and ground returns as one block.
3. Place TPS63031 and SY7069 switching loops compactly; keep measurement pads
   outside the switch-node copper.
4. Route `BOOST_5V`, `+5V` and `BL_5V` as pours with no single-via bottleneck.
5. Route all four universal `BL_5V` contacts into one wide bus immediately at
   the connector; similarly join parallel `VCC`, `+5V` and ground contacts.
6. Keep TrackPoint DATA/CLOCK away from boost switch nodes and backlight PWM.
7. Put the resistor arrays beside their complete functional groups. Do not
   create long stubs merely to fill an array element.
8. Keep precision and safety resistors beside their IC pins and away from
   switching edges.
9. Put `LK_5V_SOURCE`, injection pads and their ground pads at the board edge.
10. Keep all mandatory `MP_`, `KL_`, `LK_` and `SJ_` labels visible after the
    HolyIOT module and keyboard connector are installed.

## 12. First-article and release sequence

1. Assemble one core without keyboard and with `LK_TRACKPOINT_5V` and
   `LK_BL_ADAPTER` open. Validate USB default/1.5 A/3 A detection and charger
   EN truth table.
2. Verify F1 voltage drop and temperature at the maximum permitted input
   current.
3. Validate BQ24073 charging, DPPM and battery supplement with a battery
   emulator before attaching a real pack.
4. Validate `VCC_REG`, `VCC`, `BOOST_5V` and `+5V` unloaded and under electronic
   load. Exercise both injection procedures.
5. Close only `LK_TRACKPOINT_5V`; attach a known-good TrackPoint keyboard and
   verify reset, clock, data, pointer and every button.
6. Close `LK_BL_ADAPTER`; begin with the backlight rail disabled and PWM low.
   Measure startup at low duty, calibrate the shunt and increase only within
   the tested current budget.
7. Repeat with USB default current, advertised 1.5 A, advertised 3 A, battery
   only, USB plus battery and no battery.
8. Validate every T430 and T470 key, TrackPoint function, indicator LED and
   backlight using the final cables and adapters.
9. Perform enclosure thermal testing at maximum ambient and maximum permitted
   load.
10. Export the completed schematic BOM and verify that it no longer contains
    preliminary Q2/Q3 backlight parts, ME6211, discrete CC resistors, 3.6 kOhm
    ILIM, 1 A F1 or a `9 x 100 kOhm` placement line.

## 13. Release blockers

- Complete schematic and ERC using this architecture.
- Verify the `BSS138PS` symbol units and SOT-23-6 footprint pin assignment in
  the completed schematic and PCB.
- Qualify the USB-C receptacle and F1 current ratings.
- Verify the Molex `5051106091` (`C493444`) manufacturer land pattern and
  connector orientation on both boards; do not reuse the HCTL or Hirose
  footprint without a dimensional comparison.
- Confirm the Molex `15018-0561` 76 mm Type-A cable length, bend path and
  retention-ear clearance in the final enclosure. If the length changes, use
  only a 60-circuit, Type-A (same-side) Series 15018 variant and record its
  exact MPN in the production BOM.
- Verify 22 uF effective capacitance under bias and regulator stability.
- Confirm JLCPCB can source `SGM2562XN6G/TR`, or retain the footprint-compatible
  TI TPS22918 as the production fallback without changing the PCB.
- Verify the HUSB320 BOM is exactly BA000 and that no 866 kOhm VBUS_DET
  resistor from the BA001 reference circuit remains in the schematic.
- Verify both converter EN absolute maximums, thresholds, shutdown currents
  and back-power behavior before finalizing `SYS_EN` and its switch source.
- Validate SGM2562 SS value, startup and backlight current limit.
- Validate the approved battery, NTC option, charge/discharge current and
  enclosure thermal behavior.
- Confirm all array candidates are isolated, available and footprint-correct.
- Recount fitted placements and BOM variants from the completed schematic.
- Confirm every mandatory measurement pad, Kelvin pair, isolation link and
  injection pad remains physically accessible in the final assembly.
