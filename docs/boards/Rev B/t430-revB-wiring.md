# T430 connector - Revision B wiring

Revision B uses one shared HolyIOT core for either a T430 or T470 keyboard.
The authoritative module GPIO allocation and connector-sharing rules are in
[`shared-module-wiring.md`](shared-module-wiring.md). This document defines the
T430 connector side of that design. It replaces
the matrix assignment in
[`../Rev A/t430-preliminary-pcb-wiring.md`](../Rev%20A/t430-preliminary-pcb-wiring.md)
for Revision B hardware. Connector signal names and matrix indices do not
change; only the HolyIOT GPIO used by `DRV8` changes relative to Revision A.

The Holyiot-18010 pad numbers below use the manufacturer's **top-view** module
drawing. Verify that the PCB footprint uses the same view before routing.

## Revision A to Revision B matrix changes

| Matrix signal | T430 J7 pin | Revision A module connection | Revision B module connection | Required PCB action |
| --- | ---: | --- | --- | --- |
| `DRV8` | 6 | Holyiot pad 7, `P0.02/AIN0` | Holyiot pad 55, `P0.16` | Remove the J7 pin 6 route from pad 7 and route J7 pin 6 to pad 55 |
| `DRV0`-`DRV7`, `DRV9`-`DRV15` | See complete table below | Existing Revision A assignments | Unchanged | Retain existing routes |
| `SENSE0`-`SENSE7` | See complete table below | Existing Revision A assignments | Unchanged | Retain existing routes |

No T430 keyboard-connector pin moves. `DRV8` remains J7 pin 6 and remains
matrix row 8 in firmware. The move is only at the Holyiot end of that net.

After moving `DRV8`, Holyiot pad 7 / `P0.02/AIN0` is no longer a matrix pin.
Reserve it exclusively for battery measurement:

```text
BAT_PROTECTED -- 1.0 MOhm --+-- Holyiot pad 7, P0.02/AIN0
                            |
                            +-- 330 kOhm -- GND
                            |
                            +-- 100 nF -- GND
```

Use 1% resistors. Do not connect the ADC node to J7 pin 6 or any other matrix
net.

## Complete Revision B matrix assignment

### Matrix outputs

The firmware row array must remain ordered `DRV0` through `DRV15`.

| Firmware row | Matrix signal | T430 J7 pin | Holyiot pad | Revision B GPIO | Change from Revision A |
| ---: | --- | ---: | ---: | --- | --- |
| 0 | `DRV0` | 22 | 28 | `P1.03` | None |
| 1 | `DRV1` | 18 | 20 | `P0.19` | None |
| 2 | `DRV2` | 14 | 16 | `P1.09` | None |
| 3 | `DRV3` | 10 | 11 | `P0.31` | None |
| 4 | `DRV4` | 2 | 3 | `P1.10` | None |
| 5 | `DRV5` | 4 | 5 | `P1.15` | None |
| 6 | `DRV6` | 8 | 9 | `P0.29` | None |
| 7 | `DRV7` | 12 | 13 | `P0.05` | None |
| 8 | `DRV8` | 6 | 55 | `P0.16` | **Moved from pad 7 / `P0.02`** |
| 9 | `DRV9` | 20 | 27 | `P1.00` | None |
| 10 | `DRV10` | 16 | 18 | `P0.23` | None |
| 11 | `DRV11` | 24 | 30 | `P1.02` | None |
| 12 | `DRV12` | 28 | 47 | `P1.07` | None |
| 13 | `DRV13` | 32 | 49 | `P0.24` | None |
| 14 | `DRV14` | 26 | 34 | `P1.06` | None |
| 15 | `DRV15` | 30 | 48 | `P1.05` | None |

Configure every `DRV` signal as an active-low, open-drain matrix output. The
Revision B devicetree row entry at index 8 must therefore be:

```dts
<&gpio0 16 (GPIO_ACTIVE_LOW | GPIO_OPEN_DRAIN)>
```

Do not reorder the row array when changing that entry; the physical keymap is
indexed by the `DRV` number.

### Matrix inputs

The firmware column array must remain ordered `SENSE0` through `SENSE7`.
None of these signals move in Revision B.

| Firmware column | Matrix signal | T430 J7 pin | Holyiot pad | Revision B GPIO | Change from Revision A |
| ---: | --- | ---: | ---: | --- | --- |
| 0 | `SENSE0` | 5 | 6 | `P0.03` | None |
| 1 | `SENSE1` | 13 | 15 | `P0.07` | None |
| 2 | `SENSE2` | 9 | 10 | `P0.30` | None |
| 3 | `SENSE3` | 7 | 8 | `P0.28` | None |
| 4 | `SENSE4` | 11 | 12 | `P0.04` | None |
| 5 | `SENSE5` | 3 | 4 | `P1.13` | None |
| 6 | `SENSE6` | 15 | 17 | `P0.12` | None |
| 7 | `SENSE7` | 17 | 19 | `P0.21` | None |

Configure every `SENSE` signal as an active-low input with an internal pull-up.
Do not fit external pull-up resistors on these matrix inputs.

## Direct keyboard inputs

Fn and the power button are outside the 16 by 8 matrix and do not move in
Revision B.

| Function | T430 J7 pin | Holyiot pad | Revision B GPIO | Change from Revision A |
| --- | ---: | ---: | --- | --- |
| Fn, `-HOTKEY` | 1 | 2 | `P1.11` | None |
| Power, `-PWRSWITCH` | 19 | 26 | `P0.22` | None |

Configure both as active-low inputs with internal pull-ups.

## Schematic and firmware checklist

- Rename or reroute the J7 pin 6 net so `DRV8` terminates at Holyiot pad 55,
  `P0.16`.
- Remove every `DRV8` connection from Holyiot pad 7, `P0.02/AIN0`.
- Connect the battery divider and 100 nF filter only to pad 7 / `P0.02/AIN0`.
- Change only row-array element 8 in the Holyiot-specific devicetree from
  `gpio0 2` to `gpio0 16`.
- Keep all other matrix output, matrix input, Fn, and power-button assignments
  exactly as tabulated above.
- Before PCB release, continuity-check J7 pin 6 to Holyiot pad 55 and confirm
  there is no continuity from J7 pin 6 to Holyiot pad 7.
- Exercise all keys involving `DRV8` after assembly, then run a complete matrix
  key test to catch opens or accidental row reordering.
