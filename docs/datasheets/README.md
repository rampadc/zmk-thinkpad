# Component datasheets

Local copies are kept here so schematic capture and PCB review use the exact
manufacturer variant selected by the preliminary BOM. LCSC catalogue numbers
identify procurement entries; they do not make same-named parts interchangeable.

## Included exact-part documents

| BOM function | Selected part | Local document |
| --- | --- | --- |
| Wireless module | Holyiot 18010 V1.0 | [holyiot-18010-module.pdf](holyiot-18010-module.pdf) |
| Charger/power path | ETA6002E8A | [eta6002e8a.pdf](eta6002e8a.pdf) |
| 3.0 V regulator | MICRONE ME6211C30M5G-N | [me6211c30m5g-n.pdf](me6211c30m5g-n.pdf) |
| 5 V boost | Silergy SY7069ADC | [silergy-sy7069adc.pdf](silergy-sy7069adc.pdf) |
| USB current limiter | Silergy SY6280AAC | [sy6280aac.pdf](sy6280aac.pdf) |
| USB data ESD protector | TECH PUBLIC USBLC6-2SC6 | [tech-public-usblc6-2sc6.pdf](tech-public-usblc6-2sc6.pdf) |
| VBUS TVS | Nexperia PTVS5V0S1UR,115 | [nexperia-ptvs5v0s1ur-115.pdf](nexperia-ptvs5v0s1ur-115.pdf) |
| Charger inductor | cjiang FXL0420-2R2-M | [cjiang-fxl0420-2r2-m.pdf](cjiang-fxl0420-2r2-m.pdf) |
| Boost inductor | cjiang FXL0420-1R5-M | [cjiang-fxl0420-1r5-m.pdf](cjiang-fxl0420-1r5-m.pdf) |
| USB-C receptacle | Korean Hroparts TYPE-C-31-M-12 | [hroparts-type-c-31-m-12.pdf](hroparts-type-c-31-m-12.pdf) |
| Momentary hardware-reset switch | SHOU HAN TS24CA | [shou-han-ts24ca.pdf](shou-han-ts24ca.pdf) |
| BLE profile LEDs | Hubei KENTO KT-0603R | [kento-kt-0603r.pdf](kento-kt-0603r.pdf) |
| Reverse-battery clamp | MDD SS34 | [mdd-ss34.pdf](mdd-ss34.pdf) |
| Small-signal N-MOSFET | Yangjie BSS138 | [yangjie-bss138.pdf](yangjie-bss138.pdf) |
| System and backlight high-side P-MOSFETs | Yangjie YJL3401A | [yangjie-yjl3401a.pdf](yangjie-yjl3401a.pdf) |
| Battery fault fuse | Littelfuse 0470002.DRSNP | [littelfuse-0470002-drsnp.pdf](littelfuse-0470002-drsnp.pdf) |
| USB VBUS PTC | LUTE 1206L100/16NR | [lute-1206l100-16nr.pdf](lute-1206l100-16nr.pdf) |
| Battery connector | JST S2B-PH-SM4-TB(LF)(SN) | [jst-ph-series.pdf](jst-ph-series.pdf) |
| Programming footprint | Tag-Connect TC2050-IDC-NL | [tag-connect-tc2050-idc-nl-footprint.pdf](tag-connect-tc2050-idc-nl-footprint.pdf) |
| Latching main-power button | XKB Connection XKB5858-Z-E | [xkb-xkb5858-z-e.pdf](xkb-xkb5858-z-e.pdf) |

## Parts that must be selected before schematic release

The preliminary BOM still describes the following by requirements rather than
by an exact manufacturer part number. They are not locked components and a
generic document would not be sufficient for footprint or rating review:

- T430 flex connector;

The user will supply JAE `AA01B-S040VA1` connectors listed by JLCPCB as
`C9900009426`. The public distributor download found for the obsolete part was
actually a mislabeled drawing for a different JAE connector, so it was not
retained here. Obtain an authentic drawing and physically mate-test the actual
T430 connector before PCB release.

Discrete resistors, resistor arrays, capacitors, inductors, and zero-ohm links
are intentionally excluded from this library. Once their exact orderable MPNs
are chosen, the fabrication BOM should retain manufacturer and supplier links.
