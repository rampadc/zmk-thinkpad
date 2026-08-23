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

Deferred Revision B work is tracked separately in
[rev-b-deferred-changes.md](rev-b-deferred-changes.md). Revision A retains the
parts and quantities in this BOM.

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
- `SY7069ADC` provides the 5 V rail. It is a synchronous boost converter with
  a minimum 3 A valley-current limit, shutdown load disconnect, and automatic
  bypass. Its advertised current is switch current, not guaranteed 5 V output
  current; this design must pass a 600 mA thermal/load test at minimum battery
  voltage.
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
0, 220, 1k, 3k, 3.9k, 4.7k, 5.1k, 10k, 15k, 100k,
150k, and 470k ohms
```

All are E12 except `3k` and `5.1k`, which are E24. `5.1k` is the correct
nominal USB-C sink CC value. The boost divider uses `470k` above the FB node
and `150k` below it. It gives approximately 4.96 V from the SY7069's nominal
1.2 V reference; confirm the assembled rail before connecting the keyboard.
Do not replace it with a trimmer.

Resistors are 0603, 1%, >=0.1 W unless the table says otherwise. Capacitors are
X7R or X5R; never use Y5V/Z5U. Account for DC-bias derating on 10 uF and 22 uF
ceramics.

## Core semiconductors and electromechanical parts

`Buy` includes useful spares for one board.

This document intentionally does not assign schematic reference designators.
The eventual schematic/PCB annotation owns those identifiers. Manufacturer
part numbers and LCSC catalogue numbers remain because they identify what to
purchase.

| Function | Fit | Buy | Suggested part / LCSC | Package | Reason or constraint |
| --- | ---: | ---: | --- | --- | --- |
| Wireless module | 1 | 2 | Holyiot [18010 V1.0](datasheets/holyiot-18010-module.pdf) | LGA module | User-supplied; verify the 18010-A top-view pad numbering |
| Charger/power path | 1 | 5 | etasolution [`ETA6002E8A`](datasheets/eta6002e8a.pdf), `C7436031` | ESOP-8-EP | 3 MHz switching charger with dynamic power path; about US$0.52 at qty 1 when checked |
| 3.0 V regulator | 1 | 10 | MICRONE [`ME6211C30M5G-N`](datasheets/me6211c30m5g-n.pdf), `C403651` | SOT-23-5 | 3.0 V, 500 mA LDO with enable; about 30 uA Iq and <100 mV dropout at 100 mA |
| 5 V boost | 1 | 5 | Silergy [`SY7069ADC`](datasheets/silergy-sy7069adc.pdf), `C207630` | TSOT-23-6 | 1 MHz synchronous boost, minimum 3 A valley-current limit, shutdown load disconnect and automatic bypass |
| USB current limiter | 1 | 5 | Silergy [`SY6280AAC`](datasheets/sy6280aac.pdf), `C55136` | SOT-23-5 | Programmable VBUS current limiter with reverse blocking and output discharge |
| USB data ESD protector | 1 | 10 | TECH PUBLIC [`USBLC6-2SC6`](datasheets/tech-public-usblc6-2sc6.pdf), `C2827654` | SOT-23-6 | Two-channel 5 V USB 2.0 ESD protection; verify footprint against its drawing |
| VBUS TVS | 1 | 5 | Nexperia [`PTVS5V0S1UR,115`](datasheets/nexperia-ptvs5v0s1ur-115.pdf), `C478011` | SOD-123W | 5 V unidirectional, 6.4 V minimum breakdown, 9.2 V maximum clamp at 43.5 A, 400 W; low leakage and stocked cut tape |
| BLE profile LEDs | 3 | 20 | NATIONSTAR [`NCD0805O1`](datasheets/nationstar-ncd0805o1.pdf), `C84262` | 0805, top-view | Orange LED, 1.5-2.6 V and 30-130 mcd at 20 mA, 130-degree viewing angle; start with 4.7 kOhm and verify visibility around 0.2 mA through the intended enclosure or light pipe |
| Reverse-battery clamp | 1 | 10 | MDD [`SS34`](datasheets/mdd-ss34.pdf), `C8678` | SMA | 3 A/40 V; anode to GND and cathode to protected battery positive |
| Small-signal N-MOSFETs | 7 | 20 | Yangjie [`BSS138`](datasheets/yangjie-bss138.pdf), `C400505` | SOT-23 | Level shifting, LED/reset sinks, and backlight P-MOS gate pull-down |
| High-side P-MOSFETs | 2 | 10 | Yangjie [`YJL3401A`](datasheets/yangjie-yjl3401a.pdf), `C393520` | SOT-23 | One carries switched system power; one switches the 5 V backlight supply |
| USB VBUS PTC | 1 | 10 | LUTE [`1206L100/16NR`](datasheets/lute-1206l100-16nr.pdf), `C7542956` | 1206 | 16 V, 1 A hold/1.8 A trip, approximately 50 mOhm initial resistance; verify the actual trip curve |
| Battery fault fuse | 1 | 10 | Littelfuse [`0466002.NRHF`](datasheets/littelfuse-0466002-nrhf.pdf), `C3105` | low-profile 1206 | Very-fast 2 A/63 V fuse; 31 mOhm nominal cold resistance, 0.2326 A-squared-second nominal melting I-squared-t, and 50 A interrupt rating for sacrificial reverse-battery/short protection |
| Charger inductor | 1 | 5 | cjiang [`FXL0420-2R2-M`](datasheets/cjiang-fxl0420-2r2-m.pdf), `C167206` | molded SMD, 4.4 x 4.2 mm | 2.2 uH +/-20%, 4.5 A rated, 5 A saturation and 58 mOhm DCR for ETA6002 |
| Boost inductor | 1 | 5 | cjiang [`FXL0420-1R5-M`](datasheets/cjiang-fxl0420-1r5-m.pdf), `C167205` | molded SMD, 4.4 x 4.2 mm | SY7069 reference value: 1.5 uH +/-20%, 5 A rated, 6 A saturation and 46 mOhm DCR; same footprint as charger inductor |
| USB-C receptacle | 1 | 5 | Korean Hroparts [`TYPE-C-31-M-12`](datasheets/hroparts-type-c-31-m-12.pdf), `C165948` | right-angle 16-pin SMD with through-hole shell stakes | Very high-volume USB 2.0 receptacle; exposed signal pads and shell stakes suit hand soldering, but copy the exact drawing footprint |
| T430 flex connector | 1 | user-supplied | JAE `AA01B-S040VA1`, JLCPCB assembly listing [`C9900009426`](https://jlcpcb.com/partdetail/JLCPCBAssembly-AA01BS040VA1/C9900009426) | SMD | User-supplied; still require an authentic mechanical drawing and physical mate/orientation test before PCB release |
| Battery connector | 1 | 3 | JST [`S2B-PH-SM4-TB(LF)(SN)`](datasheets/jst-ph-series.pdf), `C295747` | right-angle SMD, 2.0 mm | Genuine 2-pin JST-PH, 2 A; wire for Adafruit/Feather polarity and mark red `BAT+`/black `GND` |
| Reset switch | 1 | 20 | SHOU HAN [`TS24CA`](datasheets/shou-han-ts24ca.pdf), `C393942` | right-angle SMD, about 4.7 x 1.9 mm | High-volume side-actuated momentary SPST-NO; 50 mA/12 V and 20,000 cycles are ample for occasional reset |
| Main power latch button | 1 | 5 | XKB Connection [`XKB5858-Z-E`](datasheets/xkb-xkb5858-z-e.pdf), `C780038` | top-actuated, six-pin through-hole | DPDT latching button, 100 mA/30 V; use one pole to control a YJL3401A gate and leave the other pole unconnected |
| Programming footprint | 1 footprint | 0 | Tag-Connect [TC2050-IDC-NL footprint](datasheets/tag-connect-tc2050-idc-nl-footprint.pdf) | pads only | No fitted connector |

The three apparent power/reset controls are not interchangeable. The
`TS24CA` is a momentary service-reset button, the `XKB5858-Z-E` is the
latching physical system-power control, and the original T430 power button is
a momentary keyboard input interpreted by firmware. Do not connect the T430
button to the system P-MOSFET gate or use the reset button as the power latch.

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

| Function | Fit | Value | Elements used | Suggested family | Purpose |
| --- | ---: | ---: | ---: | --- | --- |
| TrackPoint pull-ups | 1 | 4 x 4.7 kOhm | 2/4 | Yageo `YC164-FR-074K7L`, `C700514`; Bourns `C1733626` alternate | DATA/CLOCK 5 V pull-ups |
| Profile LED limiters | 1 | 4 x 4.7 kOhm | 3/4 | same footprint/value as TrackPoint array | BLE profile LED current limiting |
| MOSFET bias resistors | 2 | 4 x 100 kOhm | up to 8/8 | `0603x4`, isolated, 1% | MOSFET gates and safe control defaults |

Buy the MOQ only after confirming both values share the exact same
footprint. Arrays save placements and routing, but unused elements must be
unconnected at both ends. A 5% array is electrically adequate for pull-ups
and LEDs if the 1% strip is unavailable; do not relax divider or CC tolerances.

## Discrete resistors

| Function | Fit | Buy | Value | Purpose |
| --- | ---: | ---: | ---: | --- |
| USB-C CC sink | 2 | 20 | 5.1 kOhm | Individual and directly inspectable |
| TrackPoint RESET pull-up | 1 | 20 | 10 kOhm | Pull-up to 5 V |
| Backlight-enable gate series | 1 | 20 | 1 kOhm | Between the Holyiot GPIO and BSS138 gate; retained for gate-current/edge control |
| System-switch gate series | 1 | 20 | 10 kOhm | Between the system P-MOSFET gate and latching button; limits contact pulse and sets turn-on slew |
| T430 power/connectivity LED limiter | 1 | 20 | 220 Ohm | Series current limiting |
| T430 mute LED limiters | 2 | 20 | 3.9 kOhm | Series current limiting |
| Charger current-set | 1 | 10 | 5.1 kOhm | ETA6002 equation `I_BAT = 1000/R_ISET`; nominal about 196 mA |
| Fixed NTC fallback divider | 2 optional | 10 | 10 kOhm | 50% VIN fallback; prefer the protected cell's real thermistor network |
| USB current-limit set | 1 | 10 | 15 kOhm | SY6280 nominal limit about 453 mA from `I_LIM = 6800/R_SET` |
| Boost feedback upper leg | 1 | 10 | 470 kOhm | SY7069 output-to-FB resistor |
| Boost feedback lower leg | 1 | 10 | 150 kOhm | SY7069 FB-to-ground resistor; gives approximately 4.96 V nominal |
| Branch measurement links | 3 | 10 | 0 Ohm, 0805 | Removable PPK2 links for TrackPoint, backlight, and 3.0 V branches |
| Battery measurement link | 1 | 5 | 0 Ohm, 1206, >=1.5 A | Whole-board battery-current measurement |
| Configuration links | as routed | 20 | 0 Ohm, 0603 | Configuration and debug isolation only |

The ETA6002 data sheet specifies 0.5 A at 2 kOhm and gives
`I_BAT = 1000/R_ISET`; 5.1 kOhm is the nearest E24 choice for approximately
196 mA. Verify it on the first board because the data sheet does not publish a
tolerance point this low. Its NTC hot/cold thresholds are 35% and 76.5% of VIN;
an equal-resistor divider is an in-range diagnostic fallback, not temperature
protection. Prefer a real cell thermistor. Pulling NTC below 100 mV disables
NTC monitoring and should only be a marked bring-up option.

ETA6002 does not expose a programmable USB input-current pin despite LCSC's
generic “input current limiting” description; its internal switch limit is far
above a USB 2.0 unit load. The SY6280 therefore sits between protected VBUS
and the ETA6002 input.
Tie its active-high EN to VBUS so it cannot float. A 15 kOhm E24 resistor gives
about 453 mA nominal, deliberately below 500 mA; confirm tolerance and trip
behavior on the first board. With the backlight active, ETA6002 power-path
supplement lets the battery cover demand beyond the USB limit.

The battery connector deliberately follows the Adafruit/Feather two-wire
battery convention, but the JST-PH series itself does not define polarity.
Route its positive pin through the battery fault fuse to `BAT_PROTECTED`;
connect the SS34 clamp with its cathode at `BAT_PROTECTED` and anode at ground.
With the correct battery the clamp is reverse-biased, so the only added series
loss is the fuse. With a reversed battery, the clamp conducts while the fuse
or the battery pack's own protection opens the fault current. The fuse is
sacrificial:
inspect and replace it after a reverse-connection event. This circuit must be
tested with both USB absent and USB present because a conventional single
P-MOS “ideal diode” is not sufficient in a bidirectional charge/discharge path.

Use only a protected 1S 3.7 V nominal/4.2 V maximum LiPo. Recommend 1000-2000
mAh and at least 1.5 A documented continuous discharge. Do not claim that every
JST-PH battery is compatible. Put `1S LiPo ONLY`, `4.2 V MAX`, `RED +`, and
`BLACK -` beside the connector, and provide large `BAT_RAW+`, `BAT_PROTECTED`,
and GND test pads. Also provide large plated `BAT+`/GND solder holes for an
expert-wired battery; they are an alternative to the connector, never a second
simultaneous battery.

## Capacitors

| Function | Fit | Buy | Value/rating | Size | Placement / note |
| --- | ---: | ---: | --- | --- | --- |
| Local bypass | 10 | 50 | 100 nF, 25 V, X7R | 0603 | At every local IC/module supply pin |
| System-switch gate slew | 1 | 10 | 100 nF, 10 V, X7R | 0603 | Gate-to-source on the system P-MOSFET; with 10 kOhm/100 kOhm gives roughly 1 ms on and 10 ms off time constants |
| LDO input/output | 2 | 10 | 1 uF, 10 V, X7R | 0603 or 0805 | Immediately beside ME6211; follow MICRONE layout |
| USB limiter input/output | 2 | 10 | 1 uF, 10 V, X7R | 0805 | One at each side of SY6280; do not share through a long trace |
| Wireless-module bulk | 1 | 10 | 4.7 uF, 10 V, X7R | 0805 | At the Holyiot `VCC` entry, alongside 100 nF |
| Charger input | 1 | 10 | 10 uF, 10 V, X5R/X7R | 1206 | ETA6002 IN-to-PGND bypass |
| Charger system output | 1 | 10 | 22 uF, 10 V, X5R | 1206 | ETA6002 SYS output filter beside its inductor |
| Charger battery bypass | 1 | 10 | 1 uF, 10 V, X7R | 0805 | ETA6002 BATT bypass |
| Boost input | 1 | 10 | 22 uF, 10 V, X5R | 1206 | Beside SY7069 input switching loop |
| Boost output | 2 | 10 | 22 uF, 10 V, X5R | 1206 | Two in parallel at SY7069 output; verify effective capacitance, stability, and ripple |
| Backlight local bulk | 1 | 10 | 22 uF, 10 V, X5R | 1206 | After the high-side P-MOSF, close to the T430 connector |
| TrackPoint local bulk | 1 | 10 | 22 uF, 10 V, X5R | 1206 | Close to the T430 connector |
| Backlight gate slew | 1 | 10 | 10 nF, 25 V, X7R | 0603 | P-MOSF gate-to-source capacitor; tune after inrush test |

For 22 uF parts, select a reputable high-volume MLCC maker such as Samsung,
Murata, Taiyo Yuden, Walsin, or Yageo. A cheap capacitor that measures 22 uF at
zero bias may deliver only a small fraction of that at 5 V. The 1206 allowance
is intentional.

## Designator-free reference circuits

These sketches define connectivity for schematic capture. They use functional
names and IC pin names, not provisional reference designators. Confirm the
physical pin mapping and exposed-pad requirements against the data sheet for
the exact ordered suffix before assigning footprints. Every ground shown joins
the same uninterrupted PCB ground plane.

### USB-C input, protection, and data

```text
USB-C VBUS pins ── 1 A hold PTC ──+── USB_VBUS_RAW
                                  |
                         PTVS5V0S1UR
                         cathode  |  anode
                                  +── GND

USB-C CC1 ── 5.1 kOhm ── GND
USB-C CC2 ── 5.1 kOhm ── GND

USB-C D+ ───── USBLC6 channel ───── Holyiot USB D+
USB-C D- ───── USBLC6 channel ───── Holyiot USB D-
             USBLC6 VBUS ────────── USB_VBUS_RAW
                   USBLC6 GND ───── GND
```

Join all receptacle VBUS pins before the PTC. Join all ground pins and shell
stakes to ground with short paths. Place the PTVS5V0S1UR after the PTC, with
its cathode on `USB_VBUS_RAW` and anode returned to the ground plane through a
very short, wide path. It does not need low capacitance because it is not on a
USB data line. Its 5 V standoff is close to USB's 5.5 V permitted upper range,
so measure leakage and temperature at 5.5 V during first-board validation.
Place the USBLC6 beside the receptacle. Route each USBLC6 channel according to its own data
sheet; do not infer package pin numbers from the ST part. No USB-data series
resistors are specified initially.

### SY6280 USB current limiter

```text
                         SY6280AAC
USB_VBUS_RAW ────────+── IN       OUT ──+──────── USB_5V_LIM
                     |                  |
                   1 uF               1 uF
                     |                  |
                    GND                GND

USB_VBUS_RAW ─────────── EN
                         ISET ── 15 kOhm, 1% ── GND
                         GND  ───────────────── GND
```

The resistor sets approximately 453 mA nominal. Put both capacitors at the IC
pins, keep ISET away from switching nodes, and tie EN directly to IN. Never
join `USB_5V_LIM` to the locally boosted 5 V rail.

### Battery connector and reverse-fault protection

```text
JST-PH red/+ ── BAT_RAW+ ── 2 A fast fuse ──+── BAT_PROTECTED
                                             |
                                      SS34 cathode
                                      SS34 anode
                                             |
JST-PH black/- ───────────────────────────── GND
```

The SS34 is normally reverse-biased. Reversed polarity forward-biases it so
the protected pack or sacrificial fuse opens. Put both parts directly beside
the connector with wide copper. Do not replace this bidirectional path with a
single-P-MOS load-only reverse-polarity circuit.

### ETA6002 charger and dynamic power path

```text
USB_5V_LIM ─────────────── IN       SYS ──+──────── VSYS
                         ETA6002           |
USB_5V_LIM ── 10 uF ── GND                +── 22 uF ── GND

                         SW ── 2.2 uH ─────+

BAT_PROTECTED ── 0 Ohm link ── BATT ── 1 uF ── GND
                              ISET ── 5.1 kOhm, 1% ── GND
                              STAT ── test pad or no-connect

USB_5V_LIM ── 10 kOhm ──+── NTC
                         +── 10 kOhm ── GND

ETA6002 GND + exposed pad ───────────────────── GND
```

The equal NTC divider is only the fallback for a two-wire battery; use the
cell thermistor network when available. Route SYS—not BATT—through the main
switch. Keep the inductor, IN/SYS capacitors, IC, and exposed-pad return in a
compact 3 MHz loop. Treat any IC pin not shown exactly as the selected data
sheet specifies. STAT is an optional open-drain charge-status
output and may safely be left unconnected; no indicator LED is included in
this preliminary BOM.

### Main power latch and ME6211 VCC regulator

```text
VSYS ─────────────── YJL3401A source
 |                         drain ───────── VSYS_SW
 |
 +── 100 kOhm ──+── YJL3401A gate
 +──── 100 nF ──+          |
                           10 kOhm
                            |
                  one XKB5858 pole, verified by continuity test
                       common
                       ON throw ── GND
                       OFF throw ── no-connect
                  all three contacts of other pole ── no-connect

                         ME6211C30
VSYS_SW ─────────────+── VIN       VOUT ──+── 0 Ohm link ── VCC
                     |                   |
                   1 uF                1 uF
                     |                   |
                    GND                 GND

VSYS_SW ── 100 kOhm ───── EN
                         GND ─────────── GND
```

The 100 kOhm gate-to-source resistor defaults system power off if the switch is
missing or between contacts. The 100 nF gate-to-source capacitor and 10 kOhm
series resistor suppress contact-bounce edges and provide approximately 1 ms
turn-on and 10 ms turn-off time constants; they are slew control, not a precise
inrush limiter. The top-actuated latch carries only MOSFET gate current; its
100 mA contact rating is not a system-current rating. The
YJL3401A source must face VSYS and its drain must face VSYS_SW. The EN pull-up
makes the LDO follow VSYS_SW. Put both capacitors at their pins. The output link
is retained for logic-rail current measurement.

The switch drawing identifies a six-terminal DPDT device, but do not infer the
PCB pin mapping or latched state from this functional sketch. Before PCB
release, use a meter to identify one pole's common and the throw closed in the
desired latched-ON position. Leave the complete unused pole unconnected.

This latch is independent of the original T430 power-button contact. Moving
the latch changes `VSYS_SW`; pressing the T430 button only produces a firmware
key event. The separate `TS24CA` reset button also does not control this gate.

### SY7069 synchronous 5 V boost

```text
VSYS_SW ──+────────────────────── IN      OUT ───────────── +5V
          |                      SY7069                         |
          +── 1.5 uH ─────────── LX                         22 uF || 22 uF
          |                                                    |
        22 uF                                                 GND
          |
         GND

VSYS_SW ──────────────────────── EN

                         FB ──+── 150 kOhm ── GND
                              |
                              +── 470 kOhm ── OUT

                         GND ───────────────────────────── GND
```

Keep FB away from LX. SY7069 is synchronous, so it needs no external
rectifier. Tie EN to `VSYS_SW`.

### Holyiot module, TrackPoint, indicators, and backlight

```text
VCC ── 0 Ohm link ──+── Holyiot VDD
                     +── 4.7 uF ── GND
                     +── 100 nF ── GND

Holyiot reset pad ── normally-open pushbutton ── GND
USB_VBUS_RAW ─────── Holyiot VBUS-detect input only
USB D+/D- ────────── Holyiot USB D+/D-
```

Do not power the module from USB VBUS. Connect the Tag-Connect SWDIO, SWDCLK,
RESET, VCC target sense, and GND pads to the matching module signals. Follow
Holyiot's ground-pad and antenna keep-out drawing.

Fit this bidirectional circuit once for TrackPoint DATA and once for CLOCK:

```text
                              4.7 kOhm
                                 |
                                +5V
                                 |
Holyiot GPIO ── source  BSS138  drain ── TrackPoint DATA or CLOCK
 internal pull-up        gate
                          |
                         VCC
```

There is no external nRF-side pull-up. A SparkFun translator module already
has 10 kOhm on both sides; do not combine that module with these PCB pull-ups.

TrackPoint RESET uses a different, unidirectional inverting circuit. TrackPoint
IV RESET itself is active high:

```text
+5V ── 10 kOhm ──+── TrackPoint RESET
                     +── BSS138 drain
                         BSS138 source ── GND
Holyiot GPIO ─────────── BSS138 gate
                              |
                            100 kOhm
                              |
                             GND
```

GPIO low leaves the MOSFET off, so the 10 kOhm resistor asserts RESET high at
5 V. After 600 ms firmware must drive GPIO high, turning the MOSFET on and
releasing RESET low. This is a Revision A-specific inverted firmware waveform.
The three T430 indicator sinks each use the following circuit:

```text
VCC ── T430 internal LED ── series resistor ── BSS138 drain
                                                    source ── GND
Holyiot GPIO ────────────────────────────────────── gate
                                                     |
                                                   100 kOhm
                                                     |
                                                    GND

Series value: power LED 220 Ohm; each mute LED 3.9 kOhm.
```

Each optional BLE profile LED is direct and active-low:

```text
VCC ── 4.7 kOhm ── LED anode |>| LED cathode ── Holyiot GPIO
```

The backlight PWM and high-side supply switch are separate paths:

```text
Holyiot PWM GPIO ───────────────── T430 KBD_BL_PWM

Holyiot enable GPIO ── 1 kOhm ── BSS138 gate
                                      +── 100 kOhm ── GND

+5V ─────────────── YJL3401A source
   |                         drain ──+── BL_5V ── T430 backlight supply
   |                                +── 22 uF ── GND
   |                                +── 100 nF ── GND
   |
   +── 100 kOhm ──+── YJL3401A gate
   +──── 10 nF ───+
                   +── BSS138 drain
                       BSS138 source ── GND
```

The YJL3401A source must be at `+5V`. Its 100 kOhm and 10 nF are both
gate-to-source. The BSS138 only pulls the gate down; it does not carry
backlight current. PWM remains a separate 0-3.0 V signal and must not be
pulled up to 5 V.

## Circuit and layout constraints

- Route `ETA6002 SYS`, not `BATT`, through the system high-side P-MOSFET to the
  LDO and boost inputs. Use the latching button only to control its gate.
  This leaves charging active while the keyboard is switched off.
- Give the ETA6002 exposed pad a solid analog/power-ground landing and short
  return paths. Place its inductor and 10 uF input/22 uF SYS capacitors exactly as a
  compact 3 MHz buck power stage.
- Keep the SY7069 hot loop (input capacitor, IN/LX/OUT pins, inductor, and
  output capacitors) compact. Keep its switch node away from the Holyiot antenna and
  TrackPoint DATA/CLOCK, and route feedback separately from the switch node.
- Put the high-side P-MOSFET after the 5 V boost and use it only for the
  backlight branch. Connect
  its source to 5 V and drain to the backlight supply. A 100 kOhm
  gate-to-source pull-up defaults it off; a BSS138 pulls its gate low to turn
  it on.
  Drive that BSS138 through a discrete 1 kOhm resistor and give it a 100 kOhm gate
  pull-down. The TrackPoint 5 V branch stays on while the controller is awake.
  Keep the direct T430 `KBD_BL_PWM` logic connection: the P-MOSFET gates supply power,
  while the keyboard's existing PWM input controls brightness.
- Place the USB ESD protector at the USB-C receptacle. Route D+/D- as a short
  differential pair directly to the
  Holyiot; the charger sees VBUS and ground only.
- Put the TrackPoint translators and their 4.7 kOhm array beside the T430 connector.
- Do not add external pull-ups to the 3.0 V keyboard SENSE, Fn, power-button,
  or TrackPoint MCU-side nets; firmware uses nRF52840 internal pull-ups.
- Keep 0 Ohm measurement links accessible to an iron after assembly.
- Follow the ETA6002 and SY7069 reference layouts before optimizing for visual
  neatness. Power-loop geometry is part of the circuit.

## LCSC cart strategy

1. Buy five each of the SY7069 boost and SY6280 limiter, ten ME6211 LDOs,
   three ETA6002 chargers, twenty Yangjie BSS138s, ten YJL3401As, ten SS34s,
   and five battery fuses. Small
   cut-tape quantities cost little and spares prevent one damaged part from
   stopping the build.
2. Buy 20-50 each of the discrete 0603 resistor values. Buy the smallest useful
   MOQ of the two resistor-array values only after footprint confirmation.
3. Buy 50 x 100 nF, 10 x each smaller capacitor, and 10-20 x the selected
   22 uF/10 V 1206 capacitor.
4. Buy three inductors and three of every mechanically uncertain connector or
   switch. Confirm both connector dimensions against seller drawings before layout.
5. Buy one protected Adafruit/Feather-polarity 1-cell LiPo, preferably
   1000-2000 mAh and rated for at least 1.5 A continuous discharge. Its allowed
   charge rate must be at least the configured approximately 190 mA. A
   two-wire Feather battery has no thermistor connection, so use the marked
   fixed-NTC fallback unless fitting a separately wired thermistor pack.

Do not purchase an E24 assortment merely to obtain these few values unless it
is useful for the workshop. Exact-value cut tape is cheaper, has traceable
ratings, and avoids mystery dielectric/tolerance parts. An assortment remains
useful only for bring-up substitutions.

## Bring-up acceptance tests

1. Populate and test the charger/LDO first with a current-limited supply and a
   protected cell. Confirm approximately 196 mA charge current, termination,
   NTC behavior, thermal behavior, the upstream 500 mA VBUS limit, and seamless
   system power while inserting/removing USB.
2. Before using a real cell, emulate a reversed battery with a current-limited
   bench supply. Test with USB absent and present; verify the SS34 clamps the
   protected node, no downstream rail goes negative, and the battery fuse opens
   under the qualified fault current. Replace the fuse after a destructive trip
   test.
3. Scope `VSYS_SW` while operating the main latch at minimum and maximum input
   voltage, first unloaded and then with both regulators populated. Confirm
   there is no repeated restart, excessive system-P-MOSFET heating, or rail
   overshoot; adjust the gate capacitor only from measured results.
4. Populate the boost with the Holyiot and T430 connector disconnected. Test at 3.2, 3.7,
   and 4.2 V input and at 50, 300, and 600 mA output. Record voltage, input
   current, ripple, inductor/IC temperature, startup, and no-load current.
5. Disable both SY7069 and the backlight high-side switch and verify the backlight connector
   is truly unpowered. Confirm SY7069 output disconnect prevents an
   objectionable partial rail.
6. Test the 3.0 V rail and USB data, then fit the TrackPoint translators and
   connector. Use the PPK2 links to measure 3.0 V logic, TrackPoint, backlight,
   and whole-board battery current independently.
7. Confirm every LED color and brightness, remove any unintended always-on
   indicator, and run a long battery/USB transition test before installing the
   board in the laptop.

If the SY7069 cannot hold 5 V at 600 mA from 3.2 V without excessive ripple or
temperature, do not enlarge traces and hope for the best. Replace that stage
with a better-characterized boost design; the connector, measurement, and
load-switch portions of this BOM remain valid.
