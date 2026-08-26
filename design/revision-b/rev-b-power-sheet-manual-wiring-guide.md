# Revision B POWER sheet manual wiring guide

Pin numbers below are physical package or connector-pad numbers. They were
checked against the linked manufacturer data sheets. Where a selected
low-cost part has only a supplier drawing, that limitation is stated. Always
compare the EasyEDA symbol pin numbers and footprint pads with this guide
before wiring.

## 0. Sheet-wide rail flow

```text
USB-C VBUS
    |
    +-- F_USB (PTC) -- USB_VBUS_RAW --+-- BQ24073 IN
                                      +-- Schottky -- TYPEC_VDD
                                      +-- HUSB320 VBUS_DET
                                      +-- HolyIOT VBUS sense

Battery -- F_BAT -- BAT_PROTECTED -- BQ24073 BAT
                                      |
BQ24073 OUT ------------------------- VSYS
                                      +-- TPS63031 -- VCC_REG -- LK_VCC -- VCC
                                      +-- SY7069 --- BOOST_5V -- LK_5V_SOURCE -- +5V
                                                                              +-- LK_TRACKPOINT_5V -- TRACKPOINT_5V
                                                                              +-- MX22917 -- shunt -- BL_5V

VSYS -- maintained switch -- SYS_EN --+-- TPS63031 EN
                                      +-- SY7069 EN
             100 kΩ from SYS_EN to GND
```

Never create `VSYS_SW`. `BAT_PROTECTED`, `VSYS`, `VCC_REG`, `VCC`,
`BOOST_5V`, `+5V`, `TRACKPOINT_5V`, `BL_SWITCH_OUT`, and `BL_5V` are distinct
nets.

## 0.1 PCB power-track widths and brownout budget

These are **minimum finished-copper routing rules**, not schematic wire
widths. They assume:

- 1 oz/ft² finished copper, approximately 35 µm;
- power routes on an outer layer over an uninterrupted ground plane;
- 25 °C ambient and no more than 10 °C conductor temperature rise;
- maximum 20 mm route length for the voltage-drop examples; and
- copper resistivity of 1.724e-8 Ω·m at 20 °C.

Thermal minima were cross-checked with the
[DigiKey IPC-2221 trace-width calculator](https://www.digikey.com/en/resources/conversion-calculators/conversion-calculator-pcb-trace-width).
IPC-2221 is conservative but older; for final layout verification use
[Saturn PCB Toolkit](https://saturnpcb.com/saturn-pcb-toolkit/) in
**IPC-2152 without modifiers** mode, which Saturn recommends for ordinary
designs. IPC identifies IPC-2152 as the standard for sizing finished-board
conductors by current and acceptable temperature rise: see the
[IPC-2152 scope](https://www.ipc.org/TOC/IPC-2152.pdf).

EasyEDA does not document an integrated current-to-track-width calculator.
Its PCB tools let you enter routing widths and enforce per-net minimum widths
through Design Rules, but those values must be calculated externally. After
checking a width in Saturn, create matching EasyEDA net classes so DRC enforces
it; changing Route Width alone does not prove current capacity.

The recommended widths below are practical minimums for this compact board.
They are wider than the thermal minimum where needed to control load-step
drop, but they do not assume that every power net is a 50 mm track. Prefer a
short polygon or filled region for the common trunks.

| Net or segment | Design current | IPC-2221 outer-layer thermal minimum, 1 oz/10 °C | Required width | 20 mm copper drop at design current | Routing requirement |
| --- | ---: | ---: | ---: | ---: | --- |
| `USB_CONN_VBUS`, F_USB, `USB_VBUS_RAW`, BQ IN | 1.5 A | 21 mil / 0.53 mm | **40 mil / 1.02 mm** | 14.5 mV | Use a pour where possible; no neck-down between receptacle, fuse, input capacitor, and BQ IN |
| J_BAT, F_BAT, `BAT_PROTECTED`, BQ BAT | 2.0 A | 31 mil / 0.78 mm | **40 mil / 1.02 mm** | 19.4 mV | Size to the 2 A fuse; short direct battery loop |
| BQ OUT and `VSYS` common trunk | 2.0 A | 31 mil / 0.78 mm | **60 mil / 1.52 mm** | 12.9 mV | Prefer a solid polygon from BQ OUT capacitor to both converters |
| `VSYS` branch into TPS63031 | 0.8 A | 9 mil / 0.22 mm | **30 mil / 0.76 mm** | 10.4 mV | Input capacitor must join at the IC end of the branch |
| `VCC_REG`, `LK_VCC`, `VCC` trunk | 0.8 A | 9 mil / 0.22 mm | **30 mil / 0.76 mm** | 10.4 mV | Keep TPS output capacitor and FB before `LK_VCC`; use wider pours at parallel connector contacts |
| `VSYS` branch into SY7069 and its inductor | 1.5 A | 21 mil / 0.53 mm | **40 mil / 1.02 mm** | 14.5 mV | The low-battery boost-input path is a primary brownout path |
| `BOOST_5V`, `LK_5V_SOURCE`, shared `+5V` | 1.0 A | 12 mil / 0.30 mm | **40 mil / 1.02 mm** | 9.7 mV | Prefer a pour; feedback senses before the link |
| `TRACKPOINT_5V` after its link | 0.25 A | 2 mil / 0.04 mm | **20 mil / 0.51 mm** | 4.8 mV | Widen immediately where it feeds parallel connector contacts |
| `BL_SWITCH_OUT`, shunt, and `BL_5V` | 0.75 A | 8 mil / 0.20 mm | **30 mil / 0.76 mm** | 9.7 mV | Use a pour; do not include Kelvin traces in the load-current path |
| `TYPEC_VDD` | 50 mA | below fabrication minimum | **12 mil / 0.30 mm** | negligible | Logic supply only; local bypass at each IC |
| `SYS_EN` | below 1 mA | below fabrication minimum | **8 mil / 0.20 mm** | negligible | Keep away from LX/L1/L2 switching copper |
| `BAT_SENSE`, FB, CC, status, and GPIO nets | signal current | below fabrication minimum | **8 mil / 0.20 mm** | not applicable | Increase to 10 mil / 0.25 mm where space permits; Kelvin/feedback rules take precedence |

The 20 mm drops use `R = ρL/(w·t)` and `Vdrop = I·R`. Scale drop linearly
with route length and inversely with actual finished width and copper
thickness. At 60–80 °C copper resistance is higher, so the table is not a
guaranteed worst-case drop.

### Width and voltage-drop acceptance rules

1. Keep each high-current copper-only segment below **25 mV** at its design
   current and the complete source-to-load **copper-plus-via** path below
   **75 mV**. This budget excludes intentional component drops in the fuse,
   diode, links, MX22917, and 50 mΩ shunt; validate those separately. If a
   route must exceed the table's 20 mm example, widen it proportionally.
2. Do not allow a footprint pad, thermal relief, zero-ohm link pad, fuse pad,
   or layer transition to create a long neck narrower than the required
   track. A component-pad neck may be narrower only for the shortest practical
   distance, normally no more than 0.5 mm.
3. Route primary power on outer layers. If an internal 1 oz trace is
   unavoidable, do not reuse the outer-layer number: recalculate it with
   IPC-2152 and still meet the voltage-drop limit.
4. Use uninterrupted ground-plane copper beneath every converter and power
   path. Ground return copper must have at least the same effective width as
   its outgoing rail. Do not force USB, battery, converter, or backlight
   current through a thin shared ground trace.
5. A plane split, thermal spoke, or connector escape that violates these
   requirements fails the rule even when the nominal net polygon is wide.

### Power-via requirements

For an ordinary plated through via with approximately 0.30 mm finished hole,
0.60 mm pad, and at least 20 µm barrel copper, use no fewer than:

| Current crossing layers | Required parallel vias |
| ---: | ---: |
| up to 0.5 A | 1 |
| 0.5–1.0 A | 2 |
| 1.0–1.5 A | 3 |
| 1.5–2.0 A | 4 |

Place the vias as a compact array at the transition, not at the far end of a
narrow neck. Use the same count for the associated ground return. These are
conservative layout rules, not a substitute for entering the board house's
actual finished hole, plating thickness, board thickness, and temperature
rise into Saturn's IPC-2152 via calculator.

### Brownout validation on assembled hardware

Measure at the local rail/ground pairs, not against a distant bench ground:

- `MP_USB_VBUS_RAW` to `MP_GND_USB` during cable insertion and maximum charge;
- `MP_VSYS` to `MP_GND_SYS` during simultaneous 3.3 V and 5 V load steps;
- `MP_VCC` to `MP_GND_3V3` during radio transmit and keyboard activity;
- `MP_5V` to `MP_GND_5V` during TrackPoint traffic and backlight startup; and
- `MP_BL_5V` to `MP_GND_BL` at maximum PWM duty.

Use an oscilloscope with the local loop or a ground spring. A DC multimeter
cannot reveal the startup dip that causes a reset. If the actual stack-up,
route length, current, or connector allocation exceeds these assumptions,
recalculate and widen the copper before ordering the PCB.

## 1. USB-C input, surge protection, and USB data

Build this whole block together. Use **40 mil / 1.02 mm** for every VBUS
segment from the receptacle through `F_USB` to the BQ24073 IN pins. Keep each
segment under 20 mm; use a filled region if the geometry permits. Signal
tracks are **8 mil / 0.20 mm** minimum. Place `MP_USB_CONN_VBUS`,
`MP_USB_VBUS_RAW`, and `MP_GND_USB` in this block. The two rail loops are
Keystone 5016, LCSC C238129; the ground loop is also 5016/C238129 and connects
independently to the local plane.

### 1.1 J_USB — TYPE-C-31-M-12, LCSC C165948

Manufacturer drawing: [TYPE-C-31-M-12](https://datasheet.lcsc.com/lcsc/1903211732_Korean-Hroparts-Elec-TYPE-C-31-M-12_C165948.pdf).

```text
       USB-C receptacle
 A4 ----+
 A9 ----+---- USB_CONN_VBUS ---- F_USB ---- USB_VBUS_RAW
 B4 ----+
 B9 ----+

 A6 ----+
 B6 ----+---- USB_DP_CONN ---- U_ESD pins 1 -> 6 ---- NRF_USB_DP

 A7 ----+
 B7 ----+---- USB_DN_CONN ---- U_ESD pins 3 -> 4 ---- NRF_USB_DN

 A5 -------------------------- CC1 ---- HUSB320 pin 1
 B5 -------------------------- CC2 ---- HUSB320 pin 2

 A8 SBU1 --------------------- NC
 B8 SBU2 --------------------- NC
 A1, A12, B1, B12 ------------ GND plane
 shell tabs S1-S4 ------------ chassis/ESD ground strategy; default GND plane
```

All four VBUS pads join immediately. All four ground contacts join the local
ground plane with nearby vias. SBU1 and SBU2 get solderable pads but no copper
beyond the pads. Do not fit 5.1 kΩ CC resistors; HUSB320 supplies the sink Rd.

### 1.2 F_USB — ASMD1206-150, LCSC C135341

```text
pin 1  USB_CONN_VBUS ----[ resettable PTC ]---- pin 2  USB_VBUS_RAW
```

The part is non-polar. Confirm its hot hold current against enclosure
temperature and check downstream effective capacitance under DC bias.

### 1.3 D_VBUS — PTVS5V0S1UR,115, LCSC C478011

Nexperia source: [PTVS5V0S1UR](https://www.nexperia.com/product/PTVS5V0S1UR).

```text
USB_VBUS_RAW ---- pin 1 cathode K |<| pin 2 anode A ---- local connector GND
```

Use the SOD123W polarity marking from the Nexperia drawing. Place it beside
the receptacle/fuse return, not beside the charger.

### 1.4 U_ESD — HXY USBLC6-2SC6, LCSC C5261088

Manufacturer data: [HXY USBLC6-2SC6](https://datasheet.lcsc.com/datasheet/pdf/cb5323745b089563df1735df61d8bc33.pdf?productCode=C5261088).

```text
USB_DP_CONN -- pin 1 [channel 1] pin 6 -- NRF_USB_DP
USB_DN_CONN -- pin 3 [channel 2] pin 4 -- NRF_USB_DN
GND ---------------------------------- pin 2
USB_VBUS_RAW ------------------------- pin 5 (positive clamp rail)
```

Route D+ and D- through the device; do not tee long stubs into it. Pin 2 gets
a very short ESD return. Pin 5 is a clamp reference only, not a power output.

### 1.5 D_TYPEC — Hottech 1N5819WS, LCSC C191023

Supplier/manufacturer data: [Hottech 1N5819WS](https://lcsc.com/datasheet/lcsc_datasheet_Guangdong-Hottech-1N5819WS_C191023.pdf).

```text
USB_VBUS_RAW ---- pin 2 anode A |>| pin 1 cathode K ---- TYPEC_VDD
```

Fit 100 nF from `TYPEC_VDD` to GND at HUSB320 and another 100 nF at the
SN74LVC1G14. Do not substitute the VBUS TVS for this diode.

Local passive selections:

- HUSB320 bypass: 100 nF, 50 V, X7R, LCSC C14663.
- SN74LVC1G14 bypass: 100 nF, 50 V, X7R, LCSC C14663.

## 2. Type-C advertised-current detection

Keep HUSB320, its strap, inverter, and Type-C measurement loops together.
Use **12 mil / 0.30 mm** for `TYPEC_VDD` and **8 mil / 0.20 mm** for CC and
logic tracks. Place Keystone 5015/C2906768 loops for `MP_TYPEC_VDD`,
`MP_CC1`, `MP_CC2`, `MP_TYPEC_OUT1`, `MP_TYPEC_OUT2`, `MP_BQ_EN1`,
`MP_BQ_EN2`, and `MP_HUSB_DEBUG_N` beside this block.

### 2.1 U_TYPEC — HUSB320-BA000-QN12R, LCSC C7471906

Manufacturer source: [Hynetek HUSB320 and data sheet](https://en.hynetek.com/2578.html).

```text
                    HUSB320-BA000-QN12R
 USB-C CC1 ---------------- pin 1  CC1
 USB-C CC2 ---------------- pin 2  CC2
 GND -- 900 kΩ, 1%, C407504 pin 3  PORT/DEBUG_N -- MP_HUSB_DEBUG_N
 USB_VBUS_RAW ------------- pin 4  VBUS_DET       (direct; BA000)
 NC ----------------------- pin 5  ADDR/ORIENT
 NC ----------------------- pin 6  INT_N/OUT3
 TYPEC_OUT1 --------------- pin 7  SDA/OUT1 -- 10 kΩ, C25804 -> VCC
 TYPEC_OUT2 --------------- pin 8  SCL/OUT2 -- 10 kΩ, C25804 -> VCC
 NC ----------------------- pin 9  ID
 GND ---------------------- pin 10 GND
 GND ---------------------- pin 11 EN_N
 TYPEC_VDD ---------------- pin 12 VDD -- 100 nF, C14663 -> GND pin 10
```

Pin 3 must not be hard-grounded: it becomes a push-pull debug output after
initialization. Pin 4 is direct to VBUS for the **BA000** suffix; do not use
the BA001-only 866 kΩ path.

`DEBUG_N` goes low only when the HUSB320 detects a USB-C Debug Accessory. It
does not report charging, cable quality, or ordinary Type-C faults. The local
test loop is the default diagnostic. An optional indicator may be fitted as
`TYPEC_VDD -> 4.7 kΩ C23162 -> LED anode; LED cathode -> DEBUG_N`, which
lights only for a debug accessory. Do not connect this 5 V push-pull output
directly to the nRF52840. An MCU connection would require a dedicated
5-V-tolerant buffer and is not justified for the current firmware.

### 2.2 U_EN_INV — TI SN74LVC1G14DBVR, LCSC C7835

TI source: [SN74LVC1G14 data sheet](https://www.ti.com/lit/ds/symlink/sn74lvc1g14.pdf).

```text
pin 1 NC     : pad present, no copper
pin 2 A      : TYPEC_OUT1
pin 3 GND    : GND
pin 4 Y      : BQ_EN2; 10 kΩ to GND
pin 5 VCC    : TYPEC_VDD; 100 nF, C14663 to pin 3
```

The required logic is `BQ_EN2 = NOT TYPEC_OUT1`. `TYPEC_OUT1` also connects
directly to BQ24073 EN1.

### 2.3 MCU visibility

No BSS138PS is required here. In GPIO mode, HUSB320 `OUT1` and `OUT2` assert
low or become high-impedance; the 10 kΩ pull-ups to 3.3 V `VCC` therefore
make both nets safe for the BQ24073 logic inputs and nRF52840 GPIO inputs.
Connect `TYPEC_OUT1` directly to the assigned P0.09 MCU input. Keep
`TYPEC_OUT2` at its local test loop unless a second free GPIO is deliberately
assigned later; adding it would let firmware distinguish Default, 1.5 A, and
3 A advertisements. The BQ24073 only needs `OUT1`.

## 3. BQ24073 charger and battery power path

Draw the charger, battery connector, NTC selection, battery ADC, status LED,
and all their test access as one subsystem. Use **40 mil / 1.02 mm** for BQ
IN, BAT, the battery fuse path, and their grounds. Use **60 mil / 1.52 mm** or
a solid filled region for the BQ OUT-to-`VSYS` trunk. Keep each primary path
under 20 mm. Use **8 mil / 0.20 mm** for status, TS, ADC, EN, ILIM, and ISET.

Place Keystone 5016/C238129 loops for `MP_BQ_IN`, `MP_BAT_PROTECTED`,
`MP_VSYS`, and `MP_GND_BQ` beside the charger. Place Keystone
5015/C2906768 loops for `MP_BQ_EN1`, `MP_BQ_EN2`, `MP_BQ_ILIM`,
`MP_PWR_GOOD_N`, and `MP_CHARGING_N` in the same block. `MP_GND_BQ` joins
the local plane independently with short copper and nearby vias.

### 3.1 U_BQ — TI BQ24073RGTR, LCSC C15220

TI source: [BQ24073 data sheet](https://www.ti.com/lit/ds/symlink/bq24073.pdf).

```text
                           BQ24073 RGT (top-view numbering)
 BAT_TS ---------------- pin 1  TS
 BAT_PROTECTED --------- pin 2  BAT --+
 BAT_PROTECTED --------- pin 3  BAT --+-- 10 µF, 10 V, X5R/X7R -> GND
 GND ------------------ pin 4  CE          charge enabled
 BQ_EN2 ---------------- pin 5  EN2
 TYPEC_OUT1 ------------ pin 6  EN1
 PWR_GOOD_N ------------ pin 7  PGOOD      open drain
 GND ------------------- pin 8  VSS
 CHARGING_N ------------ pin 9  CHG        open drain
 VSYS ------------------ pin 10 OUT --+
 VSYS ------------------ pin 11 OUT --+-- 22 µF, 10 V, X5R/X7R -> GND
 pin 12 ILIM -- 1.2 kΩ, 1%, C22765 -> GND
 USB_VBUS_RAW ---------- pin 13 IN --- 10 µF, 10 V, X7R, C15850 -> GND
 NC -------------------- pin 14 TMR         internal timer default
 GND ------------------- pin 15 TD          normal termination enabled
 pin 16 ISET -- 4.7 kΩ, 1%, C23162 -> GND
 exposed pad 17 -------- GND plane, TI paste window and thermal vias
```

TI permits 4.7–47 µF on BAT and OUT; the selected 10 µF BAT and 22 µF OUT
values are within that range. Keep ILIM and ISET traces short and quiet. EN1
and EN2 must never float. Do not connect `BAT_PROTECTED` to `VSYS` outside
the IC.

### 3.2 Input-current mode truth path

```text
HUSB320 OUT1 -------------------------- BQ pin 6 EN1
HUSB320 OUT1 -- SN74LVC1G14 inverter -- BQ pin 5 EN2
```

This selects USB500 or resistor-programmed ILIM mode. Verify the HUSB320 GPIO
truth table on hardware.

The BAT capacitor is 10 µF, 10 V, X7R, LCSC C15850. The OUT capacitor is
22 µF, 10 V, X7R, LCSC C45783. Place both at their corresponding IC pins,
not at the test loops.

### 3.3 J_BAT, direct-wire pads, F_BAT, and reverse-fault clamp

Retain the Revision A JST `S2B-PH-SM4-TB(LF)(SN)`, LCSC C295747,
2-position PH, 2 A. The protected-cell power connector carries BAT+ and BAT-
only. JST source: [PH-series drawing](https://www.jst-mfg.com/product/pdf/eng/ePH.pdf).

```text
J_BAT pin 2 BAT+ -- F_BAT pin 1 [0466002.NRHF] pin 2 -- BAT_PROTECTED
J_BAT pin 1 BAT- --------------------------------------- GND
J_BAT hold-down tabs 3,4 ------------------------------- GND/chassis copper

J_BAT_WIRE direct-solder alternative, one 3-pin no-BOM connector symbol:
  pin 1 WIRE_BAT+ -------------------------------------- same fused BAT+ input as J_BAT pin 2
  pin 2 WIRE_BAT- -------------------------------------- GND
  pin 3 WIRE_NTC --------------------------------------- SJ_TS_NTC

D_BAT_REV SS34:
  pin 1 cathode K -------------------------------------- BAT_PROTECTED
  pin 2 anode A ---------------------------------------- GND
```

`F_BAT` is Littelfuse `0466002.NRHF`, LCSC C3105, 2 A fast-acting, 1206,
non-polar. `D_BAT_REV` is MDD `SS34`, LCSC C8678. The diode orientation is a
crowbar clamp and therefore depends on the fuse and approved battery pack;
validate fault current and clearing energy.

The direct-wire footprint is preferred over reusing a Keystone test loop.
Create `BAT_WIRE_3P` as three plated multilayer pads: 1.2 mm finished hole,
110 mil / 2.8 mm copper pad, and 120 mil / 3.05 mm pitch. Add two 1.5 mm NPTH
strain-relief holes behind the pads so the three insulated wires can be
cable-tied or laced to the board. Mark `+`, `-`, and `NTC` on both
silkscreen and assembly layers. Set **Convert to PCB = Yes** and **Add into
BOM = No**. EasyEDA's public
[`WIRE SOLDER PAD (E.D.)`](https://easyeda.com/component/Wire_Solder_PAD_E_D_-111532645c764646aca79b6fb35aeb3a)
footprint is only a single 2.5 mm square pad with a 0.9 mm hole; cloning and
editing it, or creating `BAT_WIRE_3P` directly, is safer for unknown
battery-wire diameter.

Before attaching the AEC503759 wires, identify BAT+, BAT-, and the thermistor
lead with a multimeter. Never infer polarity from wire colour. Populate either
the JST or the direct wires for battery power; do not connect two packs.

### 3.4 Temperature-sense assembly variants

```text
                         SJ_TS_NTC (normal battery build)
J_BAT_WIRE pin 3 WIRE_NTC ----[0 Ω, C163836]----------- BAT_TS --- BQ pin 1
J_BAT_WIRE pin 2 WIRE_BAT-/GND ---------------------------- NTC return

                         SJ_TS_FIXED (fallback/test build)
BAT_TS ------------------------[0 Ω, C163836]-- 10 kΩ, 1%, C25804 -- GND
```

Never fit both selectors. The AEC503759 pack thermistor is the normal path;
the 10 kΩ fixed resistor is only an assembly/test fallback and bypasses real
cell-temperature protection. At room temperature, measure resistance from
the thermistor lead to BAT- and warm the cell gently by hand: a 10 kΩ NTC
should measure near 10 kΩ at 25 °C and decrease as it warms. Do not populate
`SJ_TS_NTC` until the pack is confirmed to use the 10 kΩ NTC characteristic
expected by the BQ24073.

### 3.5 Charger status LED — one multipurpose package

Place this circuit immediately beside the BQ24073, not in a separate status
section. Use `TZ-P4-1615RGBTCA1-0.55T`, LCSC C779813, common-anode RGB.

```text
LED_STATUS pin 1 common anode ----------------------------- VSYS
LED_STATUS pin 2 red cathode -- 2.7 kΩ, C13167 -- RED_SINK
LED_STATUS pin 3 green cathode -- 4.7 kΩ, C23162 -------- PWR_GOOD_N
LED_STATUS pin 4 blue cathode -- 1.5 kΩ, C22843 --------- CHARGING_N

Q_LOW_BAT — Yangjie BSS138, LCSC C400505, discrete SOT-23:
  pin 1 gate ---------------------------------------------- NRF_LOW_BAT_LED
  pin 1 gate -- 100 kΩ, C25803 --------------------------- GND
  pin 2 source -------------------------------------------- GND
  pin 3 drain --------------------------------------------- RED_SINK
```

Both BQ status pins are open-drain and the LED resistors provide their loads,
as shown in TI's status-LED guidance. Green means a valid external input;
blue means charging. Firmware uses the battery ADC to turn red on when the
battery needs charging and no qualified USB input is present; it suppresses
red once `PGOOD` reports valid input. Green plus blue shows valid power while
charging. Powering the common anode from `VSYS` keeps the red warning
available on battery power.

### 3.6 Battery ADC

```text
BAT_PROTECTED -- LK_BAT_ADC 0 Ω (C163836) -- BAT_ADC_RAW
BAT_ADC_RAW ---- 900 kΩ, 1%, C407504 -------+
                                             +-- BAT_SENSE -- P0.02/AIN0
GND ------------ 330 kΩ, 1%, C114618 -------+
GND ------------ 100 nF, C14663 -------------+
```

The nominal divider ratio is 330/(900+330) = 0.2683. At 4.2 V battery,
`BAT_SENSE` is approximately 1.127 V. Put the 100 nF and lower resistor beside
the MCU ADC input.

## 4. Maintained system-enable switch

This switch carries only an enable signal. Route `VSYS` to the switch and
`SYS_EN` at **8 mil / 0.20 mm**. Place `MP_SYS_EN` (Keystone 5015, LCSC
C2906768) beside the switch and pull `SYS_EN` down with 100 kΩ, LCSC C25803.

### 4.1 SW_SYS_EN — SHOU HAN MSK12C02, LCSC C431540

Supplier/manufacturer drawing: [MSK12C02](https://datasheet.lcsc.com/lcsc/2304140030_SHOU-HAN-MSK12C02_C431540.pdf).

```text
VSYS -------------------- pad 2 common
SYS_EN ------------------ pad 3 ON throw
NC ---------------------- pad 1 OFF throw
GND --------------------- frame/retention pads (EasyEDA grouped shell pad 4)

SYS_EN -- 100 kΩ -------- GND
SYS_EN ------------------ TPS63031 pin 6 EN
SYS_EN ------------------ SY7069 pin 6 EN
VSYS -- SJ_SYS_EN_FORCE -- SYS_EN      normally open / DNP
```

This is a 50 mA, 12 V signal switch. It must never carry `VSYS`, `VCC`, or
5 V load current. Continuity-test pad numbering and physical slider direction
on the first board because the supplier drawing is mechanical rather than a
full electrical data sheet.

## 5. 3.3 V buck-boost

Build the input capacitor, IC, inductor, output capacitor, feedback sense,
link, and local test loops as one tight block. Use **30 mil / 0.76 mm** for
`VSYS`, `VCC_REG`, and `VCC`; use **8 mil / 0.20 mm** for `SYS_EN` and FB.
L1/L2 are switching nodes: use short, broad copper confined to the IC and
inductor pads rather than routing them across the board.

Place `MP_VCC_REG` and `MP_GND_3V3` as Keystone 5016/C238129 and `MP_VCC` as
Keystone 5015/C2906768 beside this converter. `MP_GND_SYS` is a local
Keystone 5016/C238129 beside the `VSYS` entry and connects independently to
the plane. Keep the bare switching-node probe pad and its ground-spring pad
beside L1/L2; neither is a Keystone loop.

### 5.1 U_3V3 — TI TPS63031DSKR, LCSC C15516

TI source: [TPS63031 data sheet](https://www.ti.com/lit/ds/symlink/tps63031.pdf).

```text
                         TPS63031 DSK VSON
VCC_REG --------------- pin 1 VOUT -- 22 µF, 10 V, X7R, C45783 -> PGND
inductor terminal B ---- pin 2 L2
PGND ------------------- pin 3 PGND
inductor terminal A ---- pin 4 L1
VSYS ------------------- pin 5 VIN  -- 22 µF, 10 V, X7R, C45783 -> PGND
SYS_EN ----------------- pin 6 EN
GND -------------------- pin 7 PS/SYNC       power-save enabled
VSYS ------------------- pin 8 VINA -- 100 nF, C14663 -> pin 9 GND
GND -------------------- pin 9 GND
VCC_REG Kelvin sense --- pin 10 FB           fixed 3.3 V part: no divider
thermal pad ------------ PGND

pin 4 L1 -- L_3V3 1.5 µH -- pin 2 L2
VCC_REG -- LK_VCC 0 Ω, 0805, 1.5 A (C163947) -- VCC
```

`L_3V3` is FTC252012S1R5MBCA, LCSC C5832371, 1.5 µH; its published minimum
saturation current is 4.1 A and minimum heat-rating current is 3.2 A. Keep the
L1/L2 loop tiny. FB senses the VCC_REG output-capacitor terminal, never the
injectable `VCC` side of `LK_VCC`.

## 6. 5 V boost and branch links

Use **40 mil / 1.02 mm** for the `VSYS` input branch, `BOOST_5V`,
`LK_5V_SOURCE`, and `+5V`; keep each high-current segment under 20 mm. Use
**20 mil / 0.51 mm** after `LK_TRACKPOINT_5V` and **8 mil / 0.20 mm** for FB
and EN. Confine `BOOST_LX` to the IC/inductor loop.

Place `MP_BOOST_5V` and `MP_5V` as Keystone 5016/C238129, `MP_GND_5V` as a
local Keystone 5016/C238129, and `MP_TRACKPOINT_5V` as Keystone
5015/C2906768. Keep `INJ_5V`, `INJ_GND`, and the LX probe-tip/ground-spring
pads as dedicated bare pads.

### 6.1 U_5V — Silergy SY7069ADC, LCSC C207630

Sources: [Silergy product catalog](https://www.silergy.com/zh/list/177) and
[LCSC SY7069ADC listing/data sheet](https://www.lcsc.com/product-detail/Battery-Management-ICs_Silergy-Corp-SY7069ADC_C207630.html).

```text
BOOST_FB ---------------- pin 1 FB
VSYS -------------------- pin 2 IN  -- 22 µF, 10 V, X7R, C45783 -> GND
GND --------------------- pin 3 GND
BOOST_5V ---------------- pin 4 OUT -- 22 µF, C45783 || 22 µF, C45783 -> GND
BOOST_LX ---------------- pin 5 LX
SYS_EN ------------------ pin 6 EN

VSYS -- L_5V 1.5 µH ---------------- BOOST_LX
BOOST_5V -- 470 kΩ, 1%, C23178 --+
                           +-- BOOST_FB
GND ------- 150 kΩ, 1%, C22807 --+
```

The divider gives approximately 4.96 V using the nominal 1.2 V reference.
`L_5V` uses the same FTC252012S1R5MBCA as the 3.3 V converter. Confirm the
EasyEDA TSOT-23-6 symbol against this pin order before drawing; Silergy's
public web catalog is less complete than the TI documentation.

### 6.2 Isolation links and injection

```text
BOOST_5V -- LK_5V_SOURCE, 0 Ω 1206 C164015, 2 A -- +5V
+5V ------ LK_TRACKPOINT_5V, 0 Ω 0805 C163947 ---- TRACKPOINT_5V
+5V ------------------------------------------------ MX22917 VIN

INJ_5V  --------------------------------------------- +5V side
INJ_GND --------------------------------------------- GND
```

The boost feedback and output capacitors stay on `BOOST_5V`, before the link.
Injection pads stay on `+5V`, after the link.

## 7. Backlight load switch and current measurement

Use **40 mil / 1.02 mm** for `+5V` into the load switch and **30 mil /
0.76 mm** for `BL_SWITCH_OUT`, the shunt load path, and `BL_5V`. Use **8 mil /
0.20 mm** for enable, PWM, CT, and Kelvin traces. The two Kelvin traces must
leave the shunt pads separately and carry no load current.

Place `MP_BL_SWITCH_IN` and `MP_BL_5V` as Keystone 5016/C238129,
`MP_GND_BL` as a local Keystone 5016/C238129, and `MP_BL_ENABLE` plus
`MP_BL_PWM` as Keystone 5015/C2906768. Keep `KL_BL_CURRENT_P` and
`KL_BL_CURRENT_N` as bare pads at the shunt.

### 7.1 U_BL — Maxinmicro MX22917T23, LCSC C5359093

Manufacturer source: [MX22917T data sheet](https://www.maxinmicro.com/action/download?file=84).

```text
+5V --------------------- pin 1 VIN -- 1 µF, C95843 -> GND
GND --------------------- pin 2 GND
NRF_BL_ENABLE ----------- pin 3 ON
pin 4 CT -- 1 nF, C100042 pin 1 VIN       (CT goes to VIN, never GND)
NC ---------------------- pin 5 QOD       disables output discharge
BL_SWITCH_OUT ----------- pin 6 VOUT

BL_SWITCH_OUT --[ WSLP1206R0500FEA, C844912, 50 mΩ, 1%, 1 W ]-- BL_5V
                 | KL_BL_CURRENT_P       KL_BL_CURRENT_N |

BL_5V -- 22 µF, 10 V, X7R, C86295 -- GND
BL_5V -- 100 nF, C1591 ---------------- GND
```

`ON` is active high and has an internal smart pull-down, but firmware must
still drive it to a defined state. The manufacturer's pin table explicitly
allows CT to float for fastest startup and defines QOD as optional; Revision B
uses 1 nF CT-to-VIN and leaves QOD NC. Validate rise time, voltage drop,
temperature, current limit, QOD-off behavior, and reverse current with the
real backlight.

## 8. Auxiliary TrackPoint and indicator interfaces

Keep these interfaces together at the downstream edge of the sheet, not mixed
into the charger. All tracks in this block are **8 mil / 0.20 mm** except
`TRACKPOINT_5V`, which is **20 mil / 0.51 mm**. Place Keystone
5015/C2906768 loops for `MP_TP4_RESET`, `MP_TP4_CLOCK`, and `MP_TP4_DATA`
beside the TrackPoint translator.

### 8.1 BSS138PS allocation: eight LED and TrackPoint channels

Use four UMW `BSS138PS`, LCSC C5271194, dual N-channel SOT-23-6 packages.
For every package, channel 1 is `S1=1, G1=2, D1=6`; channel 2 is
`S2=4, G2=5, D2=3`.

| Package | Pin 1 S1 | Pin 2 G1 | Pin 6 D1 | Pin 4 S2 | Pin 5 G2 | Pin 3 D2 |
| --- | --- | --- | --- | --- | --- | --- |
| Q_TP_A | `NRF_TP4_RESET` + 10 kΩ to VCC | VCC | `TP4_RESET` + 10 kΩ to TRACKPOINT_5V | `NRF_TP4_CLOCK` + 10 kΩ to VCC | VCC | `TP4_CLOCK` + 10 kΩ to TRACKPOINT_5V |
| Q_TP_LED | `NRF_TP4_DATA` + 10 kΩ to VCC | VCC | `TP4_DATA` + 10 kΩ to TRACKPOINT_5V | GND | T430 power-LED GPIO + 100 kΩ to GND | through 1 kΩ to `-LEDPWR` |
| Q_LED_A | GND | T470 Fn-lock GPIO + 100 kΩ to GND | through 1 kΩ to `-LED_FNLOCK` | GND | speaker-mute GPIO + 100 kΩ to GND | through 1 kΩ to `-LED_MUTE` |
| Q_LED_B | GND | mic-mute GPIO + 100 kΩ to GND | through 1 kΩ to `-LEDMICMUTE` | GND | T470 Caps-lock GPIO + 100 kΩ to GND | through 1 kΩ to `-LED_CAPSLOCK` |

These are exactly eight non-power channels in four dual packages. The
power-status circuit uses the separate discrete Yangjie `Q_LOW_BAT`; it is
not part of this allocation.

### 8.2 Local resistor arrays

For each isolated four-resistor 8-pin array, the manufacturer circuit is four
independent opposite-pad pairs: `1-8`, `2-7`, `3-6`, and `4-5`. Verify that
pair order in the selected EasyEDA footprint before assigning nets.

| Part | Selection | Elements |
| --- | --- | --- |
| TrackPoint 3.3 V pulls | Panasonic EXB28V103JX, C178757, 10 kΩ ×4 | RESET/CLOCK/DATA nRF sides -> VCC; fourth pair NC at both ends |
| TrackPoint 5 V pulls | Panasonic EXB28V103JX, C178757, 10 kΩ ×4 | RESET/CLOCK/DATA keyboard sides -> TRACKPOINT_5V; fourth pair NC at both ends |
| LED gate pulls | Bourns CAY10-104J4LF, C1731039, 100 kΩ ×4 | first four keyboard LED gates -> GND |
| LED series array | Panasonic EXB28V102JX, C192510, 1 kΩ ×4 | first four keyboard LED drains -> respective active-low LED nets |
| BLE LED array | CTS 741X083472JP, C1731191, 4.7 kΩ ×4 | VCC to three BLE LED anodes; fourth pair NC |

All pull-array LCSC numbers are part of this auxiliary subsystem; do not move
them to a sheet-wide passive bank. Place each array beside the MOSFET package
or connector it serves.

## 9. Final manual cross-check

Before ERC, confirm all of these statements:

1. Every physical pin above is wired or explicitly marked NC.
2. HUSB320 pin 11 and BQ pins 4, 8, 15, and exposed pad are grounded.
3. TPS63031 FB connects directly to `VCC_REG`; no divider is fitted.
4. MX22917 CT connects to VIN, not GND; QOD is NC.
5. `SYS_EN` contains only the switch, 100 kΩ pull-down, force jumper, two EN
   pins, and its measurement loop.
6. No `VSYS_SW` exists.
7. The NTC and fixed-TS selectors cannot both be populated.
8. No Keystone replaces Kelvin, injection, or switch-node bare pads.
9. All four dual BSS138PS packages have both channels accounted for, while
   the power-status sink is one separate Yangjie BSS138/C400505.
10. `POWER_old` remains untouched until this sheet passes ERC and visual
    review.

## Primary data-sheet index

- [BQ24073, Texas Instruments](https://www.ti.com/lit/ds/symlink/bq24073.pdf)
- [TPS63031, Texas Instruments](https://www.ti.com/lit/ds/symlink/tps63031.pdf)
- [SN74LVC1G14, Texas Instruments](https://www.ti.com/lit/ds/symlink/sn74lvc1g14.pdf)
- [HUSB320, Hynetek](https://en.hynetek.com/2578.html)
- [MX22917T, Maxinmicro](https://www.maxinmicro.com/action/download?file=84)
- [BSS138PS, UMW](https://www.umw-ic.com/static/pdf/d86e29681007282405144f12b47b01e6.pdf)
- [BSS138, Yangjie](../../docs/reference/datasheets/yangjie-bss138.pdf)
- [USBLC6-2SC6, HXY](https://datasheet.lcsc.com/datasheet/pdf/cb5323745b089563df1735df61d8bc33.pdf?productCode=C5261088)
- [TYPE-C-31-M-12 connector drawing](https://datasheet.lcsc.com/lcsc/1903211732_Korean-Hroparts-Elec-TYPE-C-31-M-12_C165948.pdf)
- [MSK12C02 switch drawing](https://datasheet.lcsc.com/lcsc/2304140030_SHOU-HAN-MSK12C02_C431540.pdf)
- [Nexperia PTVS5V0S1UR](https://www.nexperia.com/product/PTVS5V0S1UR)
