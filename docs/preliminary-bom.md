# Preliminary LCSC / home-assembly BOM

This is the one-board purchasing and schematic-capture BOM for the Holyiot
18010 V1.0 T430 controller in
[preliminary-pcb-wiring.md](preliminary-pcb-wiring.md). Revision A is intended
for LCSC cut-tape purchasing and assembly at home with an iron, hot air, or a
small hot plate. It deliberately uses commodity parts seen in inexpensive
Asian modules and Feather-class boards, but does not copy their optimistic
power ratings or omit protection needed by a finished keyboard.

Stock and prices were checked on **2026-08-10**. LCSC stock is not a design
guarantee: re-check the MPN, package, data sheet, lifecycle, and stock before
ordering. An LCSC number identifies the suggested source, not an electrical
substitute class.

## Design choices

- Buy components on cut tape; there are no charger or boost daughter boards.
- Use 0603 imperial as the minimum passive size. Use 0805 for removable links
  and 1206 for power ceramics and the polyfuse.
- Use E12 values first and E24 only when E12 gives a materially worse result.
- Combine repeated equal-value resistors in isolated `0603x4`, eight-terminal
  arrays. Keep USB-C CC resistors, feedback dividers, timing/current-setting
  resistors, and measurement links discrete so they are easy to inspect and
  change.
- `ETA6002` provides a switching charger and genuine dynamic power path in a
  hand-solderable ESOP-8 package. This is preferable to the cheap `TP4056`
  topology used on many marketplace boards, which has no separate system
  output.
- `LP6252B6F` provides the 5 V rail. It is an inexpensive Asian synchronous
  boost converter with a 2.7 A typical switch-current limit, soft start, and
  output disconnect in shutdown. Its advertised current is switch current,
  not guaranteed 5 V output current; this design must pass a 600 mA
  thermal/load test at minimum battery voltage.
- `ME6211C30` provides the 3.0 V logic rail. It is a high-volume Asian
  SOT-23-5 alternative to the TLV75530 with the same useful 500 mA/enable
  feature set at much lower cut-tape cost. The familiar Feather-era
  `AP2112K`/`MCP73831` combination is evidence of broad availability, but an
  MCP73831 charger is not suitable here because it lacks load sharing.
- The keyboard assembly already contains its backlight LEDs and drive
  circuitry. A `YJL3401A` P-channel MOSFET plus one BSS138 switches its 5 V
  supply; no dedicated LED-driver or load-switch IC is required.
- `SY6280AAC` limits USB VBUS current before the ETA6002. This is necessary
  because ETA6002 has no externally programmable USB input limit.

The ETA6002 has an exposed ground pad. Extend its ground landing beyond the
body and include a large plated via so it can be soldered or touched up from
the back. Paste and hot air/hot plate remain easiest, but unlike a QFN its eight
signal leads are directly visible and accessible to an iron.

## Value policy

Preferred resistor values are:

```text
0, 220, 1k, 3k, 3.9k, 4.7k, 5.1k, 10k, 15k, 56k,
100k, and 470k ohms
```

All are E12 except `5.1k`, which is E24. `5.1k` is the correct nominal USB-C
sink CC value. The boost divider uses `470k + 56k` in series above the FB node
and `100k` below it. It gives approximately 5.01 V from the LP6252's nominal
0.8 V reference; confirm the assembled rail before connecting the keyboard.
Do not replace it with a trimmer.

Resistors are 0603, 1%, >=0.1 W unless the table says otherwise. Capacitors are
X7R or X5R; never use Y5V/Z5U. Account for DC-bias derating on 10 uF and 22 uF
ceramics.

## Core semiconductors and electromechanical parts

`Buy` includes useful spares for one board.

| Ref. | Fit | Buy | Suggested part / LCSC | Package | Reason or constraint |
| --- | ---: | ---: | --- | --- | --- |
| MOD1 | 1 | 2 | Holyiot 18010 V1.0 | LGA module | User-supplied; verify the 18010-A top-view pad numbering |
| U1 | 1 | 5 | etasolution `ETA6002E8A`, `C7436031` | ESOP-8-EP | 3 MHz switching charger with dynamic power path; about US$0.52 at qty 1 when checked |
| U2 | 1 | 10 | MICRONE `ME6211C30M5G-N`, `C403651` | SOT-23-5 | 3.0 V, 500 mA LDO with enable; about 30 uA Iq and <100 mV dropout at 100 mA |
| U3 | 1 | 5 | LOWPOWER `LP6252B6F`, `C387748` | SOT-23-6 | 1 MHz synchronous boost with soft start and output disconnect; inexpensive but re-check stock before ordering |
| U4 | 1 | 5 | Silergy `SY6280AAC`, `C55136` | SOT-23-5 | Programmable VBUS current limiter with reverse blocking and output discharge |
| D1 | 1 | 10 | TECH PUBLIC `USBLC6-2SC6`, `C2827654` | SOT-23-6 | Two-channel 5 V USB 2.0 ESD protection; verify footprint against its drawing |
| D2 | 1 | 10 | `SMBJ5.0A` is too large; select a low-capacitance 5 V unidirectional TVS | SOD-123 | VBUS surge clamp; choose VRWM >=5 V and low leakage |
| D3-D5 | 3 | 10 | amber or red high-efficiency LED | 0603 | BLE profile indicators; avoid green/blue/white |
| Q1-Q7 | 7 | 20 | Yangjie `BSS138`, `C400505` | SOT-23 | Level shifting, LED/reset sinks, and backlight P-MOS gate pull-down |
| Q8 | 1 | 10 | Yangjie `YJL3401A`, `C393520` | SOT-23 | P-channel high-side switch for the 5 V backlight supply |
| F1 | 1 | 5 | 1 A hold PTC, >=6 V | 1206 | USB VBUS protection; verify trip curve, not just headline current |
| L1-L2 | 2 | 5 | shielded 2.2 uH, Isat >=4 A, low DCR | about 4x4 or 5x5 mm | One common SKU for ETA6002 and LP6252; sized for the charger's higher current limit and within LP6252's recommended 1-4.7 uH range |
| J1 | 1 | 3 | USB-C USB 2.0 16-pin receptacle with through-board shell stakes | hybrid SMD | Signal pins are SMD; shell stakes give needed mechanical strength |
| J2 | 1 | 3 | JAE `AA01B-S040VA1-R3000` or physically proven T430 mate | SMD | Mate with the real flex before PCB release |
| J3 | 1 | 3 | battery connector matching the protected cell | SMD | Verify polarity; connector families do not guarantee it |
| SW1 | 1 | 5 | normally-open side- or top-actuated reset switch | SMD | Select after enclosure check |
| SW2 | 1 | 3 | latching main power switch, >=1.5 A DC | SMD | Mechanically selected |
| P1 | 1 footprint | 0 | Tag-Connect TC2050-NL footprint | pads only | No fitted connector |

Do not buy a random “USBLC6” or BSS138 solely by search title. Check the data
sheet, marking, pinout, and seller/manufacturer. The selected TECH PUBLIC
USBLC6 has two protected channels, 5 V stand-off, 6 V breakdown, 12 V clamp,
low leakage, and the same functional SOT-23-6 class as the ST part. Its PCB pin
mapping must still be checked against its own drawing rather than inferred
from the shared part name. Asian-brand equivalents are appropriate where the
ratings are explicit. Keep the exact ETA6002 charger
MPN until a candidate substitute's power-path and low-current charge behavior
have been bench-tested. Do not assume every nominal ME6211 or XC6206 clone is
equivalent; use the listed MICRONE MPN and verify its pinout.

## Resistor arrays

Use isolated four-resistor/eight-terminal arrays only. `0603x4` describes four
0603 elements in a roughly 1206-size body; it is not a single 0603 resistor.
Do not substitute a bussed five-pin network.

| Ref. | Fit | Value | Elements used | Suggested family | Purpose |
| --- | ---: | ---: | ---: | --- | --- |
| RA1 | 1 | 4 x 4.7 kOhm | 2/4 | Yageo `YC164-FR-074K7L`, `C700514`; Bourns `C1733626` alternate | TrackPoint DATA/CLOCK 5 V pull-ups |
| RA2 | 1 | 4 x 4.7 kOhm | 3/4 | same footprint/value as RA1 | BLE profile LED current limiting |
| RA3 | 1 | 4 x 1 kOhm | 2/4 | `0603x4`, isolated, 1% | Backlight PWM and P-MOS gate-driver GPIO series resistors |
| RA4-RA5 | 2 | 4 x 100 kOhm | up to 8/8 | `0603x4`, isolated, 1% | MOSFET gates and safe control defaults |

Buy the MOQ only after confirming all three values share the exact same
footprint. Arrays save placements and routing, but unused elements must be
unconnected at both ends. A 5% array is electrically adequate for pull-ups
and LEDs if the 1% strip is unavailable; do not relax divider or CC tolerances.

## Discrete resistors

| Logical ref. | Fit | Buy | Value | Purpose |
| --- | ---: | ---: | ---: | --- |
| R_CC1, R_CC2 | 2 | 20 | 5.1 kOhm | USB-C CC sink resistors; individual and directly inspectable |
| R_TP_RESET | 1 | 20 | 10 kOhm | TrackPoint RESET pull-up to 5 V |
| R_LED_PWR | 1 | 20 | 220 Ohm | T430 power/connectivity LED |
| R_LED_MUTE, R_LED_MIC | 2 | 20 | 3.9 kOhm | T430 mute LEDs |
| R_CHG_ISET | 1 | 10 | 5.1 kOhm | ETA6002 equation `I_BAT = 1000/R_ISET`; nominal about 196 mA |
| R_NTC_TOP, R_NTC_BOTTOM | 2 optional | 10 | 10 kOhm | 50% VIN fixed NTC fallback; prefer the protected cell's real thermistor network |
| R_VBUS_ISET | 1 | 10 | 15 kOhm | SY6280 nominal limit about 453 mA from `I_LIM = 6800/R_SET` |
| R_FB_TOP_A, R_FB_TOP_B | 2 | 10 each | 470 kOhm + 56 kOhm | LP6252 feedback upper leg; series connection gives 526 kOhm |
| R_FB_BOTTOM | 1 | 10 | 100 kOhm | LP6252 feedback lower leg, as recommended by its data sheet |
| LNK_TP, LNK_BL, LNK_3V0 | 3 | 10 | 0 Ohm, 0805 | Removable PPK2 branch measurement links |
| LNK_BAT | 1 | 5 | 0 Ohm, 1206, >=1.5 A | Whole-board battery measurement link |
| CFG links | as routed | 20 | 0 Ohm, 0603 | Configuration and debug isolation only |

The ETA6002 data sheet specifies 0.5 A at 2 kOhm and gives
`I_BAT = 1000/R_ISET`; 5.1 kOhm is the nearest E24 choice for approximately
196 mA. Verify it on the first board because the data sheet does not publish a
tolerance point this low. Its NTC hot/cold thresholds are 35% and 76.5% of VIN;
an equal-resistor divider is an in-range diagnostic fallback, not temperature
protection. Prefer a real cell thermistor. Pulling NTC below 100 mV disables
NTC monitoring and should only be a marked bring-up option.

ETA6002 does not expose a programmable USB input-current pin despite LCSC's
generic “input current limiting” description; its internal switch limit is far
above a USB 2.0 unit load. U4 therefore sits between protected VBUS and U1 IN.
Tie its active-high EN to VBUS so it cannot float. A 15 kOhm E24 resistor gives
about 453 mA nominal, deliberately below 500 mA; confirm tolerance and trip
behavior on the first board. With the backlight active, ETA6002 power-path
supplement lets the battery cover demand beyond the USB limit.

## Capacitors

| Ref. group | Fit | Buy | Value/rating | Size | Placement / note |
| --- | ---: | ---: | --- | --- | --- |
| C_BYPASS | 10 | 50 | 100 nF, 25 V, X7R | 0603 | At every local IC/module supply pin |
| C_LDO_IN, C_LDO_OUT | 2 | 10 | 1 uF, 10 V, X7R | 0603 or 0805 | Immediately beside ME6211; follow MICRONE layout |
| C_CHG_IN | 1 | 10 | 10 uF, 10 V, X5R/X7R | 1206 | ETA6002 IN-to-PGND bypass |
| C_CHG_SYS | 1 | 10 | 22 uF, 10 V, X5R | 1206 | ETA6002 SYS output filter beside L1 |
| C_CHG_BAT | 1 | 10 | 1 uF, 10 V, X7R | 0805 | ETA6002 BATT bypass |
| C_BOOST_IN | 1 | 10 | 22 uF, 10 V, X5R | 1206 | Beside L2/U3 input loop |
| C_BOOST_OUT1-2 | 2 | 10 | 22 uF, 10 V, X5R | 1206 | Two in parallel at U3 OUT; verify effective capacitance, stability, and ripple |
| C_BL_LOCAL | 1 | 10 | 22 uF, 10 V, X5R | 1206 | After Q8, close to J2 backlight pins |
| C_TP_LOCAL | 1 | 10 | 22 uF, 10 V, X5R | 1206 | Close to J2 TrackPoint supply |
| C_BL_GATE | 1 | 10 | 10 nF, 25 V, X7R | 0603 | Q8 gate-to-source slew capacitor; tune after inrush test |

For 22 uF parts, select a reputable high-volume MLCC maker such as Samsung,
Murata, Taiyo Yuden, Walsin, or Yageo. A cheap capacitor that measures 22 uF at
zero bias may deliver only a small fraction of that at 5 V. The 1206 allowance
is intentional.

## Circuit and layout constraints

- Route `ETA6002 SYS`, not `BATT`, through SW2 to the LDO and boost inputs.
  This leaves charging active while the keyboard is switched off.
- Give the ETA6002 exposed pad a solid analog/power-ground landing and short
  return paths. Place L1 and its 10 uF input/22 uF SYS capacitors exactly as a
  compact 3 MHz buck power stage.
- Keep the LP6252 hot loop (`C_BOOST_IN`, U3 VIN/SW/OUT, L2, and output
  capacitors) compact. Keep its switch node away from the Holyiot antenna and
  TrackPoint DATA/CLOCK, and route feedback separately from the switch node.
- Put Q8 after the 5 V boost and use it only for the backlight branch. Connect
  its source to 5 V and drain to the backlight supply. A 100 kOhm
  gate-to-source pull-up defaults it off; Q7 pulls its gate low to turn it on.
  Drive Q7 through a 1 kOhm array element and give Q7 a 100 kOhm gate
  pull-down. The TrackPoint 5 V branch stays on while the controller is awake.
  Keep the separate T430 `KBD_BL_PWM` logic connection: Q8 gates supply power,
  while the keyboard's existing PWM input controls brightness.
- Place D1 at J1. Route USB D+/D- as a short differential pair directly to the
  Holyiot; the charger sees VBUS and ground only.
- Put the TrackPoint translators and their 4.7 kOhm array beside J2.
- Do not add external pull-ups to the 3.0 V keyboard SENSE, Fn, power-button,
  or TrackPoint MCU-side nets; firmware uses nRF52840 internal pull-ups.
- Keep 0 Ohm measurement links accessible to an iron after assembly.
- Follow the ETA6002 and LP6252 reference layouts before optimizing for visual
  neatness. Power-loop geometry is part of the circuit.

## LCSC cart strategy

1. Buy five each of U3/U4, ten U2s, three U1s, twenty Yangjie BSS138s, and ten
   YJL3401As. Small
   cut-tape quantities cost little and spares prevent one damaged part from
   stopping the build.
2. Buy 20-50 each of the discrete 0603 resistor values. Buy the smallest useful
   MOQ of the three resistor-array values only after footprint confirmation.
3. Buy 50 x 100 nF, 10 x each smaller capacitor, and 10-20 x the selected
   22 uF/10 V 1206 capacitor.
4. Buy three inductors and three of every mechanically uncertain connector or
   switch. Confirm J1 and J2 dimensions against seller drawings before layout.
5. Buy one protected 1-cell LiPo with a thermistor if possible. Its documented
   continuous discharge must cover converter input current and its allowed
   charge rate must be at least the configured approximately 190 mA.

Do not purchase an E24 assortment merely to obtain these few values unless it
is useful for the workshop. Exact-value cut tape is cheaper, has traceable
ratings, and avoids mystery dielectric/tolerance parts. An assortment remains
useful only for bring-up substitutions.

## Bring-up acceptance tests

1. Populate and test the charger/LDO first with a current-limited supply and a
   protected cell. Confirm approximately 196 mA charge current, termination,
   NTC behavior, thermal behavior, the upstream 500 mA VBUS limit, and seamless
   system power while inserting/removing USB.
2. Populate the boost with the Holyiot and J2 disconnected. Test at 3.2, 3.7,
   and 4.2 V input and at 50, 300, and 600 mA output. Record voltage, input
   current, ripple, inductor/IC temperature, startup, and no-load current.
3. Disable both LP6252 and the Q7/Q8 switch and verify the backlight connector
   is truly unpowered. Confirm LP6252 output disconnect prevents an
   objectionable partial rail.
4. Test the 3.0 V rail and USB data, then fit the TrackPoint translators and
   connector. Use the PPK2 links to measure 3.0 V logic, TrackPoint, backlight,
   and whole-board battery current independently.
5. Confirm every LED color and brightness, remove any unintended always-on
   indicator, and run a long battery/USB transition test before installing the
   board in the laptop.

If the LP6252 cannot hold 5 V at 600 mA from 3.2 V without excessive ripple or
temperature, do not enlarge traces and hope for the best. Replace that stage
with a better-characterized boost design; the connector, measurement, and
load-switch portions of this BOM remain valid.
