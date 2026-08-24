# Universal keyboard-adapter FFC connector

> **Status: design only.** Revision B has not been built or tested.

Revision B uses a 60-contact FFC between the shared core and one passive
keyboard adapter. The adapter contains only local decoupling, an ID strap, and
optional zero-ohm test links. TrackPoint translation, LED drivers, and
backlight switching stay on the core.

This interface is proprietary to this project. It carries 3.0 V logic, 5 V
TrackPoint signals and switched 5 V backlight power on the same cable. Do not
connect it to a laptop keyboard cable or an unrelated 60-contact FFC.

## Connector and footprint

Use HCTL `XW05202-600R-00`, LCSC part
[`C2906127`](https://www.lcsc.com/product-detail/C2906127.html), on both boards:

| Property | Requirement |
| --- | --- |
| Contacts | 60 |
| Pitch | 0.50 mm |
| Mounting | Surface-mount, horizontal/right-angle |
| Contact position | Bottom contact |
| Lock | Hinged lid |
| FFC thickness | 0.30 mm |
| Contact material/finish | Copper alloy, tin plated |
| Rated voltage | 50 V AC/DC |
| Operating temperature | -25 degrees C to +85 degrees C |
| Rated current | Not specified by the manufacturer drawing or LCSC listing |
| Mating cycles | Not specified by the manufacturer drawing or LCSC listing |

Create the footprint from the [HCTL manufacturer
drawing](https://www.lcsc.com/datasheet/C2906127.pdf), including its `PCB
LAYOUT` view. Do not reuse the old Hirose footprint or derive pad dimensions
from pitch alone.

An alternate connector is acceptable only if its drawing confirms:

- identical signal-pad pitch, pad width, row position and pin-1 convention;
- compatible hold-down-tab locations and solder-pad dimensions;
- the same 0.30 mm cable thickness and bottom-contact orientation;
- compatible insertion depth, actuator sweep and cable centerline;
- an explicit per-contact current rating adequate for the assigned power pins;
- temperature and mating-durability ratings adequate for the product.

If any land or mechanical dimension differs, create another footprint.

For the 60-position `XW05202-600R-00`, the drawing gives these controlling
dimensions. They are review aids, not a replacement for checking the drawing:

| Drawing item | Dimension |
| --- | ---: |
| Contact pitch | 0.50 +/- 0.05 mm |
| `A`, first-to-last contact center span | 29.50 mm |
| `B`, connector reference width | 30.60 mm |
| `C`, overall connector width | 34.40 mm |
| Signal-pad width | 0.35 +/- 0.05 mm |
| Signal-pad length | 1.20 mm |
| Nominal connector height | 2.00 +/- 0.15 mm |
| Nominal connector depth | 6.00 +/- 0.15 mm |
| Applicable FFC thickness | 0.30 +/- 0.03 mm |

Use the drawing for tab pads and the actuator and cable envelopes. The summary
dimensions above are review aids, not a footprint definition. No drop-in
replacement has been approved.

The PCB footprint must include:

- numbered signal pads 1 through 60;
- both manufacturer-specified hold-down tabs as mechanical pads;
- a pin-1 triangle on copper and silkscreen that remains visible after
  assembly;
- an `FFC CONTACTS DOWN` silkscreen note on both boards;
- a courtyard covering the open actuator and cable insertion area;
- no components or vias that obstruct the actuator or the straight cable exit;
- two strain-relief holes or chassis tie points beyond the connector courtyard.

## Cable orientation and pin reversal

The cable arrangement is fixed:

1. Mount `J_UFC_CORE`, an HCTL `XW05202-600R-00`, on the top side of the core
   PCB.
2. Mount `J_UFC_ADAPTER`, another HCTL `XW05202-600R-00`, on the top side of
   the adapter PCB.
3. Place the boards coplanar with the two cable openings facing each other.
   Relative to `J_UFC_CORE`, `J_UFC_ADAPTER` is rotated exactly 180 degrees in
   the PCB layout.
4. Use an untwisted 60-contact, 0.50 mm-pitch, 0.30 mm-thick FFC whose exposed
   contacts are on the same face at both ends, commonly called a Type A cable.
5. Insert both cable ends with the exposed conductors facing down toward their
   PCBs, as required by the bottom-contact connectors.

The conductors run straight, but the adapter's local pad numbers reverse:

```text
core physical pad n -> adapter physical pad (61 - n)
```

Core pad 1 reaches adapter pad 60; core pad 60 reaches adapter pad 1. Signal
`UFC_n` therefore uses core pad `n` and adapter pad `61 - n`.

Use the real pad numbers on both footprints. Apply the reversal with net names
in the adapter schematic. Do not draw crossed conductors, renumber the cable,
or use a Type B cable.

Label the two sides this way:

- **Core schematic:** connector symbol pin `n` uses physical footprint pad `n`
  and carries universal signal `UFC_n` from the signal table below.
- **Core PCB:** mark physical pad 1 as `PAD 1 / UFC_1` and physical pad 60 as
  `PAD 60 / UFC_60`.
- **Adapter schematic:** connector symbol physical pin `p` carries universal
  signal `UFC_(61-p)`. For example, adapter physical pin 59 carries `UFC_2 /
  DRV0`, and adapter physical pin 10 carries `UFC_51 / BL_5V`.
- **Adapter PCB:** mark physical pad 1 as `PAD 1 / UFC_60` and physical pad 60
  as `PAD 60 / UFC_1`. Rotate the complete connector footprint 180 degrees;
  do not mirror it.
- **Adapter routing:** route by signal name, not by the apparent left-to-right
  position of the pads.

The pin-assignment table below lists both physical pad numbers. Before applying
power, check these sample points:

| Core pad | Expected adapter pad | Core signal |
| ---: | ---: | --- |
| 1 | 60 | GND |
| 2 | 59 | `DRV0` |
| 17 | 44 | `DRV15` |
| 28 | 33 | `-HOTKEY` |
| 37 | 24 | `TP4_DATA` |
| 41 | 20 | `TP4_RESET` |
| 51 | 10 | `BL_5V` |
| 59 | 2 | `VCC` |
| 60 | 1 | GND |

Also verify that core pad 2 does **not** reach adapter pad 2. Ground checks at
the cable ends cannot detect a missing reversal because both end pins are
ground.

## Pin assignment

Signal names follow the Revision A schematic where possible. T470 adds
`-LED_FNLOCK` and `-LED_CAPSLOCK`. TrackPoint and LED signals in the table are
on the keyboard side of the core translators or sink resistors. Do not connect
them directly to an nRF52840 GPIO.

The core needs five separate LED sink channels: T430 power, T470 Fn Lock,
speaker mute, microphone mute, and T470 Caps Lock. Do not merge T430 power with
T470 Fn Lock; their firmware signals and meanings differ.

`Universal/core pin` is also the physical core pad. `Daughter-board pad` is the
physical pad on the rotated adapter connector. The two numbers always add to
61.

| Universal/core pin | Daughter-board pad | Signal | Domain and core-board treatment |
| ---: | ---: | --- | --- |
| 1 | 60 | GND | Ground |
| 2 | 59 | `DRV0` | 3.0 V active-low open-drain matrix drive |
| 3 | 58 | `DRV1` | 3.0 V active-low open-drain matrix drive |
| 4 | 57 | `DRV2` | 3.0 V active-low open-drain matrix drive |
| 5 | 56 | `DRV3` | 3.0 V active-low open-drain matrix drive |
| 6 | 55 | `DRV4` | 3.0 V active-low open-drain matrix drive |
| 7 | 54 | `DRV5` | 3.0 V active-low open-drain matrix drive |
| 8 | 53 | `DRV6` | 3.0 V active-low open-drain matrix drive |
| 9 | 52 | `DRV7` | 3.0 V active-low open-drain matrix drive |
| 10 | 51 | `DRV8` | 3.0 V active-low open-drain matrix drive |
| 11 | 50 | `DRV9` | 3.0 V active-low open-drain matrix drive |
| 12 | 49 | `DRV10` | 3.0 V active-low open-drain matrix drive |
| 13 | 48 | `DRV11` | 3.0 V active-low open-drain matrix drive |
| 14 | 47 | `DRV12` | 3.0 V active-low open-drain matrix drive |
| 15 | 46 | `DRV13` | 3.0 V active-low open-drain matrix drive |
| 16 | 45 | `DRV14` | 3.0 V active-low open-drain matrix drive |
| 17 | 44 | `DRV15` | 3.0 V active-low open-drain matrix drive |
| 18 | 43 | GND | Ground |
| 19 | 42 | `SENSE0` | 3.0 V active-low matrix input; core internal pull-up |
| 20 | 41 | `SENSE1` | 3.0 V active-low matrix input; core internal pull-up |
| 21 | 40 | `SENSE2` | 3.0 V active-low matrix input; core internal pull-up |
| 22 | 39 | `SENSE3` | 3.0 V active-low matrix input; core internal pull-up |
| 23 | 38 | `SENSE4` | 3.0 V active-low matrix input; core internal pull-up |
| 24 | 37 | `SENSE5` | 3.0 V active-low matrix input; core internal pull-up |
| 25 | 36 | `SENSE6` | 3.0 V active-low matrix input; core internal pull-up |
| 26 | 35 | `SENSE7` | 3.0 V active-low matrix input; core internal pull-up |
| 27 | 34 | GND | Ground |
| 28 | 33 | `-HOTKEY` | 3.0 V active-low Fn input; core internal pull-up |
| 29 | 32 | `-PWRSWITCH` | 3.0 V active-low T430 power input; NC on adapters without it |
| 30 | 31 | `TP4LEFT` | 3.0 V active-low direct input; core internal pull-up |
| 31 | 30 | `TP4RIGHT` | 3.0 V active-low direct input; core internal pull-up |
| 32 | 29 | `TP4MIDDLE` | 3.0 V active-low direct input; core internal pull-up |
| 33 | 28 | `KBD_ID` | T470-only raw keyboard ID from J38 pin 36; T430 adapter leaves this universal pin unconnected |
| 34 | 27 | `ADAPTER_ID` | Digital identification strap to core `P0.09`; T430 leaves open and T470 ties to GND |
| 35 | 26 | `-KBD_BL_DTCT` | Keyboard backlight-detect input to core `P0.10`; T430 J7 pin 21 or T470 J37 pin 12 |
| 36 | 25 | GND | Ground and ID-strap return |
| 37 | 24 | `TP4_DATA` | 5 V open-collector side of core DATA translator |
| 38 | 23 | GND | TrackPoint signal return/guard |
| 39 | 22 | `TP4_CLOCK` | 5 V open-collector side of core CLOCK translator |
| 40 | 21 | GND | TrackPoint signal return/guard |
| 41 | 20 | `TP4_RESET` | 5 V active-high TrackPoint IV hardware reset; driven high to assert and low to release by the core-board level shifter |
| 42 | 19 | `+5V` | TrackPoint supply; parallel with pin 43 |
| 43 | 18 | `+5V` | TrackPoint supply; parallel with pin 42 |
| 44 | 17 | GND | TrackPoint power return |
| 45 | 16 | `-LEDPWR` | After core sink resistor; T430 power LED; NC on T470 |
| 46 | 15 | `-LED_FNLOCK` | After core sink resistor; T470 Fn-lock LED; NC on T430 |
| 47 | 14 | `-LED_MUTE` | After core sink resistor; speaker-mute LED |
| 48 | 13 | `-LEDMICMUTE` | After core sink resistor; microphone-mute LED |
| 49 | 12 | `-LED_CAPSLOCK` | After core sink resistor; T470 Caps Lock LED; NC on T430 |
| 50 | 11 | `KBD_BL_PWM` | Direct 0-3.0 V push-pull PWM; never pull up to 5 V |
| 51 | 10 | `BL_5V` | Switched backlight 5 V; parallel with pins 52-54 |
| 52 | 9 | `BL_5V` | Switched backlight 5 V; parallel with pins 51, 53 and 54 |
| 53 | 8 | `BL_5V` | Switched backlight 5 V; parallel with pins 51, 52 and 54 |
| 54 | 7 | `BL_5V` | Switched backlight 5 V; parallel with pins 51-53 |
| 55 | 6 | GND | Backlight return; parallel return paths also use pins 58 and 60 |
| 56 | 5 | `VCC` | Regulated 3.0 V keyboard rail; parallel with pins 57 and 59 |
| 57 | 4 | `VCC` | Regulated 3.0 V keyboard rail; parallel with pins 56 and 59 |
| 58 | 3 | GND | Keyboard-logic and backlight return |
| 59 | 2 | `VCC` | Regulated 3.0 V keyboard rail; parallel with pins 56 and 57 |
| 60 | 1 | GND | Ground and edge guard |

Pins assigned to the same power rail must join with copper on both boards; do
not daisy-chain them through necked-down traces. Route pins 51-54 as a common
backlight bus immediately after the connector pads. The four contacts provide
connector margin but do not prove that the complete boost converter, FFC or
keyboard cable can carry the load. A T470 keyboard drew 595 mA at 80% duty and
620 mA at 85% duty with 50 kHz PWM. At 90%, the Analog Discovery supply shut
down before a current reading was recorded. Validate connector temperature and
voltage drop at the final maximum duty using a suitable current-limited supply.

### TrackPoint IV reset polarity and driver

`TP4_RESET` is **active high**. IBM's *TrackPoint System Version 4.0
Engineering Specification* explicitly describes a hard reset as the reset pin
being asserted high. Do not prefix this net with `-`, `/` or `n`, and do not
describe it as active low.

The reset input's normal released state is low. Revision B uses the same
non-inverting BSS138 topology for RESET as for DATA and CLOCK. For each of the
three channels:

- BSS138 source connects to the nRF/3.0 V side;
- BSS138 drain connects to the keyboard/5 V side;
- BSS138 gate connects directly to regulated 3.0 V `VCC`;
- a fitted 10 kOhm resistor pulls the nRF-side signal up to 3.0 V; and
- a fitted 10 kOhm resistor pulls the keyboard-side signal up to 5 V.

The nRF52840 internal pull-ups do not replace the fitted Revision B 10 kOhm
resistors. Confirm the dual-MOSFET package pinout from its datasheet; never
assign source, drain and gate from schematic-symbol appearance alone.

For RESET, this circuit preserves polarity:

| `NRF_TP4_RESET` | `TP4_RESET` | State |
| ---: | ---: | --- |
| High | 5 V | Reset asserted |
| Low | 0 V | Reset released |

Firmware must drive `NRF_TP4_RESET` high for 600 ms nominal, then drive it low
and wait for TrackPoint POST before PS/2 communication. Verify that exact
high-then-low waveform at the keyboard connector before attaching a
TrackPoint.

Do not copy the Revision A gate-driven low-side sink into Revision B. The
Revision A circuit is an inverter and requires the opposite nRF waveform. The
universal daughter board remains passive.

## Adapter identification

The core enables the internal pull-up on `ADAPTER_ID`. One bit is sufficient
because Revision B supports exactly two mutually exclusive adapter types:

| Adapter | `ADAPTER_ID` |
| --- | --- |
| No adapter or T430 | Open |
| T470 | GND |

Identification is diagnostic only. It must not automatically select a firmware
matrix transform or USB/BLE transport profile. Continue to build distinct T430
and T470 firmware images. Firmware must also tolerate an unpowered or absent
adapter without continuously leaking current through the ID pin. Because
`P0.09` and `P0.10` default to NFC operation, firmware must set
`CONFIG_NFCT_PINS_AS_GPIOS=y` before using `ADAPTER_ID` or
`-KBD_BL_DTCT`.

## Passive adapter requirements

- Map signals by universal net name, never by relative connector position.
- Fit no TrackPoint translator, LED sink transistor or LED series resistor on
  the adapter; those circuits belong to the core.
- Put at least 22 uF bulk plus 100 nF ceramic decoupling from `+5V` to GND
  beside the physical TrackPoint connector.
- Put 100 nF from `VCC` to GND beside each keyboard connector supply entry.
- Use optional zero-ohm links in `+5V` and `BL_5V` for first-article
  isolation and current measurement.
- Keep `TP4_DATA` and `TP4_CLOCK` short, do not route either beside `BL_5V`,
  and maintain a continuous ground plane below them.
- Do not hot-plug an adapter. Disconnect USB and the battery before opening
  either FFC lock.
- Add visible adapter name, revision, pin-1 and contact-side markings.

## Release checks

1. Check the library footprint pad numbering against the current manufacturer
   drawing, including the view from the PCB side.
2. Print the footprint 1:1 and place a real connector over it before ordering
   the PCB.
3. Verify both physical pin-1 marks and confirm that both cable contact faces
   point down. Confirm the adapter footprint is rotated, not mirrored.
4. With no keyboard attached, continuity-check every core physical pad `n` to
   adapter physical pad `61 - n`; check all 60 contacts, not only the sample
   points above.
5. Confirm no continuity from any 5 V pin (37, 39, 41-43 or 51-54) to `DRV`,
   `SENSE`, direct-input, ID or `VCC` pins.
6. Power the core from current-limited supplies and verify 3.0 V and both 5 V
   branches at the adapter before connecting a keyboard.
7. Validate every T430 and T470 matrix transition, TrackPoint movement and
   buttons, each keyboard indicator, and backlight startup/PWM behavior.
