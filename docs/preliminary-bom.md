# Preliminary production PCB BOM

This is a one-board preliminary BOM for the Holyiot 18010 V1.0 T430 keyboard
controller described in [production-pcb-wiring.md](production-pcb-wiring.md).
It is intended for schematic capture and early purchasing, not yet as a
JLCPCB assembly upload.

> **Stock snapshot:** 9 August 2026. LCSC stock and pricing change frequently.
> Recheck every LCSC number, manufacturer datasheet, package, pinout, and stock
> level before ordering or assigning a PCB footprint.

## Assumptions

- One Holyiot 18010 V1.0 module per board.
- One-cell protected Li-ion/LiPo battery for wireless use.
- USB-C provides USB 2.0 data and 5 V input; no USB Power Delivery negotiation.
- Revision A boots in the BQ24074 USB100 input mode and permits firmware to
  select USB500 after enumeration. Battery charging is set to approximately
  200 mA; a protected battery must permit that charge rate.
- Revision A uses one external power-stage inductor: a 3.0 V LDO supplies the
  Holyiot/keyboard logic, and a TPS61023 boost supplies the legacy 5 V loads.
- Measured TrackPoint consumption is 5.94 mA idle, 6.47 mA during movement,
  and 37.1 mA peak at 5 V. Revision A retains a 50 mA peak allowance.
- The unmeasured backlight is allocated 500 mA at 5 V. Its load switch and
  replaceable passives allow later tuning without changing the PCB.
- The T430 keyboard and TrackPoint use one 40-contact-plus-ground JAE keyboard
  connector assembly.
- The PCB uses 0603 imperial resistors and high-frequency capacitors. Larger
  capacitors, fuses, and inductors use 0805, 1206, or larger packages.
- DATA and CLOCK use discrete BSS138 level translators on the production PCB.
  The SparkFun breakout is only for the bench prototype.
- Quantities headed **Fit** are per PCB. **Buy** is a sensible quantity for a
  small hand-built prototype run, not a production reel quantity.

## Parts that can be selected now

### Modules, connectors, protection, and switches

| Ref. | Fit | Buy | Description | Candidate / LCSC | Package | Status and notes |
| --- | ---: | ---: | --- | --- | --- | --- |
| MOD1 | 1 | 2 | nRF52840 BLE module | Holyiot 18010 V1.0 | Module | User-supplied; verify purchased pad numbering against the 18010-A top-view manual |
| J1 | 1 | 5 | USB-C receptacle, USB 2.0, 16 contact | [TYPE-C-31-M-12 / C165948](https://www.lcsc.com/product-detail/C165948.html) | SMD + shell stakes | Widely stocked candidate; use its exact recommended footprint |
| J2 | 1 | 3-5 | ThinkPad keyboard receptacle | JAE `AA01B-S040VA1-R3000` or verified compatible clone | 0.5 mm, 40 contact | Not an LCSC commodity part; likely donor, AliExpress, or specialist sourcing. Do not substitute AA01A without mechanical testing |
| Q1-Q6 | 6 | 20 | N-channel MOSFET for TrackPoint translation/reset and LED sinks | [onsemi BSS138 / C52895](https://www.lcsc.com/product-detail/C52895.html) | SOT-23 | Six fitted; check source/drain orientation against the selected symbol and footprint |
| D1 | 1 | 5 | USB D+/D- ESD protector | [ST USBLC6-2SC6 / C7519](https://www.lcsc.com/product-detail/C7519.html) | SOT-23-6 | Place directly beside J1 with a short ground return |
| D2 | 1 | 20 | USB VBUS ESD/TVS protection | [ESD5Z5V0 / C85708](https://www.lcsc.com/product-detail/C85708.html) | SOD-523 | Preliminary; coordinate with charger input protection and connector placement |
| D3-D5 | 3 | 10 | BLE-profile indicator, emerald green | [ROHM CSL0901PT1C / C6490365](https://www.lcsc.com/product-detail/C6490365.html) | 0603 | Profiles 1-3; active-low direct GPIO drive, labeled on silkscreen; amber or red may be substituted |
| F1 | 1 | 10 | USB VBUS resettable fuse, 1 A hold | [TLC-NSMD100 / C261954](https://www.lcsc.com/product-detail/C261954.html) | 1206 | Revision-A value; BQ24074 starts at USB100 and is limited to USB500 after enumeration |
| SW1 | 1 | 10 | nRF hardware-reset pushbutton | [TS-1088-AR02016 / C720477](https://www.lcsc.com/product-detail/C720477.html) | SMD, 3.2 x 4.2 mm | P0.18/nRESET to ground; substitute mechanically if case access requires side actuation |
| P1 | 1 footprint | 0 | Tag-Connect programming interface | TC2050-NL target footprint | PCB pads only | No connector is populated on the PCB; add locating holes and pad-1 marking |

The `TC2050-IDC-NL-050-ALL` cable, nRF52840 DK, keyboard, battery, enclosure,
and USB cable are off-board items and are not included in the fitted count.

### Fixed resistors

Use 0603 imperial, 1%, at least 0.1 W unless the final schematic explicitly
requires otherwise. Ordering 100 of each common value is usually cheaper and
more useful than ordering exactly the fitted count.

| Ref. group | Fit | Buy | Value | Candidate MPN | LCSC | Purpose |
| --- | ---: | ---: | ---: | --- | --- | --- |
| R_4K7 group | 5 | 100 | 4.7 kΩ | `0603WAF4701T5E` | [C23162](https://www.lcsc.com/product-detail/C23162.html) | Two TrackPoint DATA/CLOCK 5 V pull-ups and three BLE-profile LED resistors |
| R_TP_RESET, R_TS_FIXED | 2 | 100 | 10 kΩ | `0603WAF1002T5E` | [C25804](https://www.lcsc.com/product-detail/C25804.html) | TrackPoint RESET pull-up; default BQ24074 TS fallback when the battery has no NTC |
| R_CONTROL group | 3 | 100 | 1 kΩ | `0603WAF1001T5E` | [C21190](https://www.lcsc.com/product-detail/C21190.html) | `KBD_BL_PWM`, `BL_5V_EN`, and `CHG_USB500_EN` series protection |
| R_100K group | 9 | 100 | 100 kΩ | `0603WAF1003T5E` | [C25803](https://www.lcsc.com/product-detail/C25803.html) | RESET/LED gates, PWM/load-switch/charger defaults, TLV75530P enable, TPS61023 feedback bottom |
| R_LED_PWR | 1 | 100 | 220 Ω | `0603WAF2200T5E` | [C22962](https://www.lcsc.com/product-detail/C22962.html) | Power/connectivity LED current limiting |
| R_LED_MUTE group | 2 | 100 | 3.9 kΩ | `0603WAF3901T5E` | [C23018](https://www.lcsc.com/product-detail/C23018.html) | Speaker-mute and microphone-mute LED current limiting |
| R_CC1, R_CC2 | 2 | 100 | 5.1 kΩ | `0603WAF5101T5E` | [C23186](https://www.lcsc.com/product-detail/C23186.html) | USB-C CC1/CC2 pull-downs (`Rd`) |
| R_ISET | 1 | 100 | 4.42 kΩ | `0603WAF4421T5E` | [C23043](https://www.lcsc.com/product-detail/C23043.html) | BQ24074 approximately 200 mA fast-charge setting |
| R_ILIM | 1 | 100 | 3.09 kΩ | `0603WAF3091T5E` | [C22992](https://www.lcsc.com/product-detail/C22992.html) | BQ24074 approximately 500 mA resistor-mode limit; retained for a future EN strap configuration |
| R_ITERM | 1 | 100 | 2.94 kΩ | `0603WAF2941T5E` | [C22909](https://www.lcsc.com/product-detail/C22909.html) | BQ24074 approximately 20 mA termination current |
| R_TMR | 1 | 100 | 46.4 kΩ | `0603WAF4642T5E` | Verify stock | BQ24074 approximately 6.25-hour safety timer |
| R_FB_TOP | 1 | 100 | 732 kΩ | 0603, 1%, MPN TBD | Verify stock | TPS61023 5 V feedback top resistor |
| LNK_TP, LNK_BL, LNK_3V0 | 3 | 20 | 0 Ω | TBD after PCB current/footprint check | TBD | Removable 0805 links for PPK2 branch measurements; provide test pads on both sides |
| CFG_BQ and BOOST_EN straps | 4 | 20 | 0 Ω | Stocked generic 0603 jumper | TBD | BQ24074 EN1/EN2/CE configuration and default-always-on TPS61023 enable |

The schematic reference designators must be renumbered during capture; the
logical names such as `R_ISET` and `R_FB_TOP` are more important than the
temporary `R1`-style numbers in this planning table.

Do not add external pull-ups to the 3.0 V sides of the TrackPoint DATA/CLOCK
lines, the keyboard SENSE lines, Fn, or the power button. Those inputs use the
nRF52840's internal pull-ups. The 4.7 kΩ resistors above are on the separate
5 V TrackPoint side and remain necessary.

### Revision-A capacitors

Use X7R or X5R; do not substitute Y5V for power-stage capacitors.

| Ref. group | Fit now | Buy | Value/rating | Candidate / LCSC | Package | Intended placement |
| --- | ---: | ---: | --- | --- | --- | --- |
| C_BYPASS group | 10 | 100 | 100 nF, 50 V, X7R | [Samsung CL10B104KB8NNNC / C1591](https://www.lcsc.com/product-detail/C1591.html) | 0603 | Holyiot, J7, converters, USB protection, and local rail bypass |
| C_1U group | 4 | 20 | 1 µF, 10 V or higher, X7R | Select stocked 0603/0805 | 0603 or 0805 | BQ24074 `IN`; TPS22918 `VIN`; TLV75530P input and output |
| C_BQ_BAT | 1 | 20 | 4.7 µF, 10 V, X7R | [CCTC TCC0805X7R475K100FTM / C51912533](https://www.lcsc.com/product-detail/C51912533.html) | 0805 | BQ24074 `BAT` pins |
| C_10U group | 2 | 20 | 10 µF, 10 V, X5R | [CCTC TCC0805X5R106K100FT / C380331](https://www.lcsc.com/product-detail/C380331.html) | 0805 | BQ24074 `OUT`; TPS61023 input |
| C_22U group | 4 | 20 | 22 µF, 10 V, X5R | [FH 1206X226M100NT / C108717](https://www.lcsc.com/product-detail/C108717.html) | 1206 | Two TPS61023 outputs; local TrackPoint and switched-backlight bulk |
| C_BL_CT | 1 | 20 | 10 nF, 25 V or higher, X7R | Select stocked 0603 | 0603 | TPS22918 slew-rate control; roughly 20-30 ms 5 V rise |

Check capacitance under DC bias, especially the 10 µF and 22 µF MLCCs. Leave
room for an additional larger electrolytic or polymer capacitor on the 5 V
rail if logic-analyzer or oscilloscope testing shows backlight inrush droop.

## Revision-A battery and power-stage selection

These selections are suitable for fabricating the first PCB. They are not a
substitute for checking each manufacturer's reference layout and the exact
footprint before ordering boards.

| Ref. | Fit | Candidate / LCSC | Package | Revision-A role and population |
| --- | ---: | --- | --- | --- |
| U1 | 1 | [TI BQ24074RGTR / C54313](https://www.lcsc.com/product-detail/C54313.html) | QFN-16-EP, 3 x 3 mm | 1-cell charger/power path; GPIO-selectable USB100/500, 200 mA charge, 20 mA termination, 6.25-hour timer |
| U2 | 1 | [TI TLV75530PDBVR / C507268](https://www.lcsc.com/product-detail/C507268.html) | SOT-23-5 | Fixed 3.0 V, 500 mA LDO for Holyiot and keyboard logic/LEDs; `EN` pulled to input through 100 kΩ |
| U3 | 1 | [TI TPS61023DRLR / C919459](https://www.lcsc.com/product-detail/C919459.html) | SOT-563 | 5 V boost; 732 kΩ/100 kΩ feedback, 10 µF input, two 22 µF output |
| U4 | 1 | [TI TPS22918DBVR / C131941](https://www.lcsc.com/product-detail/C131941.html) | SOT-23-6 | Backlight 5 V load switch; 10 nF `CT`, `QOD` initially open, active-high from Holyiot P1.12 |
| L1 | 1 | [XR XRNR4030-1uH/N / C5289359](https://www.lcsc.com/product-detail/C5289359.html) | Shielded 4 x 4 mm | TPS61023, 1 µH, 4.14 A rated, 5.26 A saturation, 16 mΩ DCR |
| J3 | 1 | TBD JST-PH-compatible 2- or 3-pin battery connector | Through-hole or large SMD | Match the actual protected battery and optional NTC; never assume connector polarity |
| SW2 | 1 | TBD latching power switch | Mechanically selected | Shipping/off control; position and actuator depend on the enclosure |
| LNK_BAT | 1 | Removable high-current link or solder bridge, TBD | PCB feature or suitably rated part | At least 1.5 A path rating, accessible PPK2 pads, and nearby ground test point |

## Deliberately tunable or mechanically unresolved items

Keep footprints or configuration access for:

- BQ24074 EN2/CE straps, EN1 force-high override, and alternate NTC/fixed-10
  kOhm TS selection; never populate both TS paths at once;
- an unpopulated TPS22918 `QOD`-to-`BL_5V` resistor and an interchangeable
  0603 `CT` capacitor;
- TPS61023 enable selection: default always-on and alternate future GPIO;
- battery connector, battery fuse/protection arrangement, and optional fuel gauge;
- any ferrite bead or extra bulk capacitance justified by measured TrackPoint
  or backlight noise.

Use 0603 for these resistors and small capacitors where their electrical
requirements permit. Use 0805/1206 or larger for high-capacitance MLCCs and
any component carrying the 5 V load current.

## Mechanical and sourcing risks

1. **J2 is the largest sourcing risk.** Obtain and physically mate the exact
   keyboard connector before finalizing its footprint. Ideally buy several
   identical parts or recover connectors from donor T430-family boards.
2. **Holyiot manuals disagree about some pad numbering.** Verify GPIO names
   against the actual module and continuity-test uncertain P1.10/P1.11 pads.
3. **USB connector footprints are not interchangeable.** Lock J1 to the exact
   TYPE-C-31-M-12 drawing or intentionally select another connector before PCB
   layout.
4. **Battery connector polarity is not standardized.** Match the actual cell,
   clearly mark polarity on silkscreen, and use a protected cell.
5. **TC2050-NL is only a footprint.** Buy the `TC2050-IDC-NL-050-ALL` cable
   separately for direct use with the nRF52840 DK P19 debug-out header.

## What to measure before freezing the BOM

- T430 keyboard/Holyiot current through `LNK_3V0`, idle and during heavy key
  activity, with both debug and release logging configurations.
- TrackPoint has measured 5.94 mA idle, 6.47 mA during movement, and 37.1 mA
  peak at 5 V. Recheck on the assembled PCB and retain the 50 mA peak limit.
- Backlight current through `LNK_BL` at 5 V at 50% and 100% PWM, including
  startup inrush. Begin with a 500 mA continuous design allowance.
- Total battery current during BLE connected idle, BLE activity, advertising,
  wake, TrackPoint use, and every power/profile LED pattern. Use `LNK_BAT` with
  USB disconnected.
- 5 V ripple and droop during TrackPoint plus full-backlight operation.
- Temperature rise of the charger, LDO, boost converter/inductor, load switch, and USB
  connector during charging and maximum load.

After those measurements, tune the populated values if necessary, choose the
final battery capacity and connector, and export a machine-readable BOM from
the actual schematic rather than copying this planning table.

The [Nordic PPK2 documentation](https://docs.nordicsemi.com/r/bundle/ug_ppk2/page/ug/ppk/ppk_user_guide_intro.html)
specifies up to 1 A in ampere-meter mode and 600 mA continuous in source mode.
Use source mode at 5.0 V for the isolated TrackPoint branch. Use a separate
current-limited supply and suitable shunt/current probe if maximum backlight or
whole-board current can exceed those limits.
