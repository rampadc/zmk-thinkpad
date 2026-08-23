# Deferred Revision B changes

Do not begin Revision B schematic or PCB work until the assembled
[Revision A board](t430-revA.pdf) has been electrically tested and successfully
flashed. Revision A retains the seven individual SOT-23 BSS138 devices in its
BOM and must be validated as built.

## Required changes

- Replace the Revision A `SY6280AAC` plus `ETA6002E8A` power path with the
  `BQ24073RGTR` implementation specified in
  [`../Rev B/bq24073-power-path.md`](../Rev%20B/bq24073-power-path.md). Remove
  the obsolete `USB_5V_LIM` net and charger inductor; retain the protected
  `USB_VBUS_RAW`, `BAT_PROTECTED`, and downstream `VSYS` domains.
- Replace the seven individual SOT-23 BSS138 devices with four LGE
  `BSS138DW` dual MOSFET packages, using seven internal FETs and leaving one
  unused.
- Move `DRV8` from `P0.02/AIN0` to `P0.16`, reserve `P0.02/AIN0` for battery
  measurement, and update the Holyiot devicetree pin map.
- Add a voltage divider from `BAT_PROTECTED` to `P0.02/AIN0` and an ADC-node
  filter capacitor for ZMK battery-level reporting. Finalize the values,
  source-impedance behavior, and percentage calibration after Revision A
  power-path and battery measurements.
- Connect the ETA6002 `STAT` output to a charge-status LED or a spare Holyiot
  GPIO instead of leaving it unconnected.
- Provide accessible labeled test pads for `BAT_PROTECTED`, `VSYS`, `VCC`,
  `+5V`, GND, SWDIO, SWDCLK, and RESET.
- Keep the Tag-Connect programming footprint accessible after the keyboard and
  enclosure are assembled.
- Finalize mounting holes, keyboard and flex-connector clearances, USB connector
  support, and a battery mounting area that does not pinch or abrade the cell.
- Add clear silkscreen for the board revision, connector pin 1, battery polarity,
  `1S LiPo / 4.2 V max`, programming orientation, and test-point names.
- Preserve the Holyiot antenna keepout on every copper layer and keep the
  battery, mounting hardware, keyboard backplate, and flex cable clear of it.
- Retain hardware bias resistors that keep TrackPoint RESET, indicator sinks,
  backlight enable, and power switching in safe inactive states during reset and
  startup.
- Retain the removable branch-measurement links and add unpopulated footprints
  for optional signal series resistors or filter capacitors where Revision A
  testing shows they may be useful.
- Reconcile the complete Revision B schematic pin map with the Holyiot-specific
  devicetree before PCB release; do not reuse the nRF52840 DK pin assignments.

## Dual BSS138 substitution

| Function | Fit | Buy | Suggested part / LCSC | Package | Constraint |
| --- | ---: | ---: | --- | --- | --- |
| Dual small-signal N-MOSFETs | 4 | 10 | LGE `BSS138DW`, `C27975287` | SOT-363 | Eight independent N-MOSFETs total: use seven for the three non-inverting TrackPoint DATA/CLOCK/active-high RESET level shifters, three indicator sinks, and the backlight P-MOS gate pull-down; leave the eighth unused. |

Manufacturer pinout:

| Pin | Terminal |
| ---: | --- |
| 1 | S2 |
| 2 | G2 |
| 3 | D1 |
| 4 | S1 |
| 5 | G1 |
| 6 | D2 |

The package marking is `K38`. Re-check the manufacturer data sheet and LCSC
stock before Revision B release because the current LCSC catalogue metadata
incorrectly describes this dual device as one N-channel.

Preserve each existing circuit's source, gate, and drain connections.
TrackPoint DATA and CLOCK remain bidirectional open-collector translators with
each source on the 3.0 V side, gate at 3.0 V, and drain on the 5 V side. This
package-only consolidation does not require firmware polarity changes.

## Battery reporting

Move `DRV8` from `P0.02/AIN0` to `P0.16`. `P0.09` or `P0.10` are alternatives
only if their NFC-pin configuration and routing are handled. Reserve
`P0.02/AIN0` for the divider from `BAT_PROTECTED`, and update the Revision B
Holyiot devicetree to use the new DRV8 pin and select ADC channel 0 as
`zmk,battery`.

Add the finalized divider resistors and filter capacitor to the Revision B BOM
after Revision A measurements are complete.
