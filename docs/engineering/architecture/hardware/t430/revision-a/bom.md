---
title: BOM notes
description: Historical purchasing notes for the manufactured T430 Revision A board.
---

# T430 Revision A BOM notes

The [purchased BOM](purchased-bom.xls) records the parts used for Revision A.
The [schematic](schematic.pdf) is the source for designators, values, and
connectivity. Do not reconstruct the board from this page alone.

Revision A used parts that were practical for hand assembly and low-volume
LCSC purchasing. Important devices included:

| Function | Part |
| --- | --- |
| Wireless MCU module | HolyIOT 18010-A, nRF52840 |
| Charger and power path | ETA6002E8A |
| 3.0 V regulator | ME6211C30M5G-N |
| 5 V boost converter | SY7069ADC |
| USB current limiter | SY6280AAC |
| USB ESD protection | USBLC6-2SC6 |
| Level shifting and signal sinks | BSS138 |
| High-side power switches | YJL3401A |
| USB-C connector | TYPE-C-31-M-12 |
| Battery connector | JST S2B-PH-SM4-TB |
| SWD footprint | Tag-Connect TC2050-IDC-NL |

Exact manufacturer documents are in the
[datasheet library](../../../../../reference/datasheets/).

## Selection rules

- Use a protected, single-cell 4.2 V Li-ion or LiPo battery.
- Verify the HolyIOT top-view pad numbering against the exact module drawing.
- Check every footprint against the selected manufacturer's drawing.
- Use resistor arrays only when they contain independent resistors.
- Keep the USB, battery, TrackPoint, backlight, and logic voltage domains clear.
- Treat stock and catalogue numbers as purchasing records, not substitute
  specifications.

Revision B replaces the Revision A charger and power-path design. See the
deferred Revision B changes.
