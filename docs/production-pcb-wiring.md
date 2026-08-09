# T430 production PCB wiring and design plan

This document plans the custom PCB that will replace the nRF52840 DK with a
Holyiot 18010 V1.0 module. It covers the T430 keyboard connector, TrackPoint,
backlight, indicator LEDs, USB, power, SWD programming, and the corresponding
firmware/build work.

> **Status:** this is the proposed production layout. It has not yet been
> applied to the firmware. The current `thinkpad_t430.overlay` still contains
> the tested nRF52840 DK pin assignment. Do not route a production PCB from the
> DK table in the root README.

## Design goals

- Keep the 44-pin T430 keyboard connector routes as straight as practical.
- Use the nRF52840 internal pull-ups for every 3.3 V GPIO input that needs one.
- Keep all 5 V signals away from nRF52840 GPIOs.
- Support USB HID, BLE, USB logging, and three BLE profiles.
- Wake a suspended host from keyboard or TrackPoint activity over the selected
  USB or BLE transport, subject to the host's wake settings.
- Make SWD programming and recovery possible even when USB firmware is broken.
- Preserve the keyboard backlight, TrackPoint, power LED, mute LED, and
  microphone-mute LED.
- Keep the RF antenna clear of copper and the T430 keyboard's metal backplate.

## References and pin-number conventions

The production plan uses the newer **18010-A top-view numbering** shown in the
[Holyiot 18010-A manual](https://fccid.io/2ALGY-18010-A/User-Manual/User-Manual-5877632.pdf).
The drawing is 13.5 mm by 18 mm and labels the module `VER1.0`.

An older Holyiot manual numbers many of the same physical pads while looking
at the bottom of the module. It also disagrees with the newer manual about the
P1.10/P1.11 order. Use GPIO names such as `P1.10`, not pad numbers alone, in
the schematic and PCB net names. Before assembly, compare the purchased module
with the top-view drawing and continuity-check the questionable P1.10/P1.11
pads if possible.

The T430 connector is `J7` in the
[T430 motherboard schematic](https://indiarefix.in/download/file.php?id=9313).
Connector pin numbering reverses visually depending on which side of the flex
cable is being viewed. Mark pin 1 on both the schematic and PCB silkscreen and
verify continuity before applying power.

## System voltage domains

The nRF52840 GPIO domain is 3.0-3.3 V and is not 5 V tolerant.

| Rail or signal group | Voltage | Notes |
| --- | ---: | --- |
| Holyiot `VDD-nRF`, module pad 14 | 3.0-3.3 V | Never connect directly to USB or 5 V |
| T430 J7 pin 35 | 3.3 V | Keyboard logic and indicator LED rail |
| T430 J7 pins 29 and 31 | 5 V | Keyboard backlight supply |
| T430 J7 pin 38 | 5 V | TrackPoint supply |
| T430 J7 pins 34 and 41-44 | Ground | Join to one solid PCB ground system |
| `KBD_BL_PWM` | 3.3 V logic | The load it controls is powered from 5 V |
| TrackPoint DATA/CLOCK/RESET | 5 V open-collector side | Requires translation/isolation |

The keyboard matrix, Fn key, and power button are passive switch contacts. They
do not require 5 V translation.

## Module placement and PCB stack-up

Place the Holyiot at a PCB edge with its ceramic antenna at the outside edge.
The preferred order is:

```text
case or PCB edge
┌─────────────────────────────────────┐
│ Holyiot antenna: no copper or metal │
│ ┌─────────────────────────────────┐ │
│ │        Holyiot 18010 V1.0       │ │
│ └─────────────────────────────────┘ │
│       short fan-out and vias        │
│                                     │
│ J7 pin 1 ═════════════════ pin 44   │
└─────────────────────────────────────┘
```

Keep copper pours, traces, planes, stitching vias, the flex cable, battery,
and other metal away from the antenna on every layer. The T430 keyboard has a
large metal backplate, so position the antenna near a plastic case opening or
outside the backplate footprint.

A four-layer PCB is strongly recommended:

1. Top: module, J7, USB, translators, and short signal routes.
2. Layer 2: solid ground, except for the complete antenna keepout.
3. Layer 3: 3.3 V and 5 V distribution.
4. Bottom: remaining matrix routes and low-speed controls.

Place J7 with pin 1 nearest the Holyiot's pad 1/left-edge side. This lets the
matrix lines leave the module in nearly the same order in which they enter J7.

## Proposed production GPIO assignment

This assignment is optimized for the 18010-A **top-view** footprint. It must
replace the current DK assignment in a future Holyiot-specific devicetree.

| Function | T430 J7 pin | Holyiot pad | nRF52840 GPIO | Electrical behavior |
| --- | ---: | ---: | --- | --- |
| Fn, `-HOTKEY` | 1 | 2 | P1.11 | Active-low input, internal pull-up |
| `DRV4` | 2 | 3 | P1.10 | Matrix output |
| `SENSE5` | 3 | 4 | P1.13 | Active-low input, internal pull-up |
| `DRV5` | 4 | 5 | P1.15 | Matrix output |
| `SENSE0` | 5 | 6 | P0.03 | Active-low input, internal pull-up |
| `DRV8` | 6 | 7 | P0.02 | Matrix output |
| `SENSE3` | 7 | 8 | P0.28 | Active-low input, internal pull-up |
| `DRV6` | 8 | 9 | P0.29 | Matrix output |
| `SENSE2` | 9 | 10 | P0.30 | Active-low input, internal pull-up |
| `DRV3` | 10 | 11 | P0.31 | Matrix output |
| `SENSE4` | 11 | 12 | P0.04 | Active-low input, internal pull-up |
| `DRV7` | 12 | 13 | P0.05 | Matrix output |
| `SENSE1` | 13 | 15 | P0.07 | Active-low input, internal pull-up |
| `DRV2` | 14 | 16 | P1.09 | Matrix output |
| `SENSE6` | 15 | 17 | P0.12 | Active-low input, internal pull-up |
| `DRV10` | 16 | 18 | P0.23 | Matrix output |
| `SENSE7` | 17 | 19 | P0.21 | Active-low input, internal pull-up |
| `DRV1` | 18 | 20 | P0.19 | Matrix output |
| Power, `-PWRSWITCH` | 19 | 26 | P0.22 | Active-low input, internal pull-up |
| `DRV9` | 20 | 27 | P1.00 | Matrix output; therefore SWO is unavailable |
| `DRV0` | 22 | 28 | P1.03 | Matrix output |
| `-LEDPWR` | 23 | 29 | P1.01 | Drives an external low-side MOSFET |
| `DRV11` | 24 | 30 | P1.02 | Matrix output |
| `KBD_BL_PWM` | 25 | 33 | P1.04 | 1 kHz, 3.3 V PWM output |
| `DRV14` | 26 | 34 | P1.06 | Matrix output |
| `DRV12` | 28 | 47 | P1.07 | Matrix output |
| `DRV15` | 30 | 48 | P1.05 | Matrix output |
| `DRV13` | 32 | 49 | P0.24 | Matrix output |
| `-LED_MUTE` | 33 | 50 | P0.20 | Drives an external low-side MOSFET |
| `-LEDMICMUTE` | 36 | 51 | P0.17 | Drives an external low-side MOSFET |
| `TP4DATA` | 37 | 52 | P0.15 | UARTE RX and GPIO, internal 3.3 V pull-up |
| `TP4CLK` | 39 | 53 | P0.14 | GPIO edge interrupt, internal 3.3 V pull-up |
| `TP4_RESET` | 40 | 54 | P0.13 | Drives an open-drain reset MOSFET |

Reserved module connections:

| Holyiot pad | Signal | Use |
| ---: | --- | --- |
| 1, 25, 37 | GND | Ground plane |
| 14 | `VDD-nRF` | Regulated module supply |
| 21 | P0.18/nRESET | Reset button and TC2050 reset |
| 22 | VBUS | USB VBUS detection only |
| 23 | USB D- | USB connector |
| 24 | USB D+ | USB connector |
| 31 | SWDCLK | TC2050 programming footprint |
| 32 | SWDIO | TC2050 programming footprint |
| 38-46, 55 | Spare GPIOs | Test pads or future expansion |

P0.09 and P0.10 remain reserved for NFC and are not needed by this design.

## Internal pull-up policy

The firmware must explicitly configure internal nRF pull-ups on:

- `SENSE0` through `SENSE7`;
- Fn (`-HOTKEY`);
- the power button (`-PWRSWITCH`); and
- the 3.3 V sides of TrackPoint DATA and CLOCK.

Do not place external pull-up resistors on these nRF GPIOs. Matrix `DRV` lines,
PWM, reset-drive, and LED-drive pins are outputs and do not need pull-ups.

The 5 V TrackPoint bus is a separate electrical domain. Its high-side pull-ups
are required because the nRF's internal pull-ups cannot pull a BSS138's 5 V
side high.

## TrackPoint level conversion

### DATA and CLOCK

Use one BSS138 for DATA and one for CLOCK in the usual bidirectional,
open-collector arrangement:

```text
                          4.7 kΩ
                            │
                           +5 V
                            │
nRF GPIO ── source  BSS138  drain ── J7 DATA or CLOCK
 internal       gate │
 pull-up             └────────────── 3.3 V
```

- BSS138 source: 3.3 V/nRF side.
- BSS138 drain: 5 V/TrackPoint side.
- BSS138 gate: 3.3 V.
- High-side pull-up: 4.7 kΩ to 5 V on each signal.
- Low-side pull-up: nRF internal pull-up only.
- Place both MOSFETs and their high-side resistors close to J7 pins 37 and 39.

### RESET

Use a third BSS138 as a unidirectional open-drain reset driver:

```text
              +5 V
                │
              10 kΩ
                │
J7 TP4_RESET ─ drain  BSS138  source ─ GND
                         gate
                           │
                     nRF P0.13
                           │
                     100 kΩ to GND
```

GPIO high turns the MOSFET on and asserts the active-low 5 V reset. The 100 kΩ
gate pull-down keeps reset released while the MCU is unpowered or starting.
The Holyiot devicetree will need the correct reset polarity for this circuit.

## Indicator LEDs

The T430 power, mute, and microphone-mute LEDs use the keyboard's 3.3 V rail,
not its 5 V rail. They do not require voltage translation. Low-side MOSFETs are
still recommended to isolate the nRF and reproduce the original motherboard's
current-sinking behavior.

Use three additional BSS138s:

```text
J7 pin 35, 3.3 V
        │
 keyboard's internal LED
        │
      J7 LED pin
        │
 original series resistor
        │
 BSS138 drain
 BSS138 source ───────── GND
 BSS138 gate ─────────── nRF GPIO
        │
      100 kΩ
        │
       GND
```

| LED | J7 signal/pin | Series resistor | Proposed GPIO |
| --- | --- | ---: | --- |
| Power/connectivity | `-LEDPWR`, pin 23 | 220 Ω | P1.01 |
| Speaker mute | `-LED_MUTE`, pin 33 | 3.9 kΩ | P0.20 |
| Microphone mute | `-LEDMICMUTE`, pin 36 | 3.9 kΩ | P0.17 |

The resistors reproduce R13, R41, and R44 in the T430 motherboard schematic.
With MOSFET gate drive, GPIO high means LED on; the production devicetree must
therefore use active-high GPIO semantics rather than the DK's current
active-low direct-drive semantics.

## Backlight

- Supply 5 V to both J7 pins 29 and 31 with suitably wide traces.
- Connect J7 pin 25 `KBD_BL_PWM` directly to the 3.3 V PWM GPIO P1.04.
- Do not put a BSS138 in the PWM path.
- Place local bulk and high-frequency decoupling near J7. Start with 22 µF plus
  100 nF on the 5 V backlight rail and adjust after measuring inrush/noise.
- The known T430 assembly makes `-KBD_BL_DTCT` on J7 pin 21 unnecessary; leave
  it unconnected unless automatic keyboard-type detection is added later.

## BSS138 quantity and supporting resistors

| Purpose | BSS138 quantity |
| --- | ---: |
| TrackPoint DATA and CLOCK translators | 2 |
| TrackPoint RESET open-drain driver | 1 |
| Power, mute, and microphone-mute LED sinks | 3 |
| **Total fitted** | **6** |

Buy at least 10; buying 20 is reasonable for prototypes and rework.

Related resistor count:

| Value | Quantity | Purpose |
| ---: | ---: | --- |
| 4.7 kΩ | 2 | TrackPoint DATA/CLOCK 5 V pull-ups |
| 10 kΩ | 1 | TrackPoint RESET 5 V pull-up |
| 100 kΩ | 4 | RESET and three LED MOSFET gate pull-downs |
| 220 Ω | 1 | Power LED current limiting |
| 3.9 kΩ | 2 | Mute and microphone-mute LED current limiting |

## USB-C

| USB-C function | Holyiot connection |
| --- | --- |
| VBUS | Module pad 22 for USB detection; also feed the protected power path |
| D- | Module pad 23 |
| D+ | Module pad 24 |
| Ground/shield | Ground plane using the selected shield/ESD strategy |

Include:

- one 5.1 kΩ pull-down from CC1 to ground;
- one 5.1 kΩ pull-down from CC2 to ground;
- a low-capacitance USB ESD array immediately beside the connector;
- a fuse or current-limited load switch on VBUS; and
- short, parallel D+/D- traces over uninterrupted ground.

Do not connect USB VBUS directly to Holyiot `VDD-nRF`. Module pad 14 must be
powered through the board's regulated 3.0-3.3 V rail.

## SWD programming with TC2050

Use the official Tag-Connect `TC2050-NL` target footprint. For a direct
one-to-one connection to the nRF52840 DK's 0.050-inch P19 header, use the
[TC2050-IDC-NL-050-ALL](https://www.tag-connect.com/product/tc2050-idc-nl-050-all)
cable. The ordinary `TC2050-IDC-NL` terminates in a 0.1-inch connector and
requires an adapter.

Wire the target footprint one-to-one with Nordic P19:

| TC2050 pad | DK P19 signal | Production PCB connection |
| ---: | --- | --- |
| 1 | `SWD0_VTG` | Regulated target VDD at module pad 14 |
| 2 | `SWD0_SWDIO` | Holyiot pad 32, SWDIO |
| 3 | `SWD0_SELECT` | Ground; selects the DK's external target |
| 4 | `SWD0_SWDCLK` | Holyiot pad 31, SWDCLK |
| 5 | GND | Ground |
| 6 | SWO | Leave unconnected; RTT does not use SWO |
| 7 | NC | Leave unconnected |
| 8 | NC | Leave unconnected |
| 9 | NC | Leave unconnected |
| 10 | `SWD0_RESET` | Holyiot pad 21, P0.18/nRESET |

Nordic documents the P19 selection and power behavior in
[Programming an external board](https://docs.nordicsemi.com/r/bundle/ug_nrf52840_dk/page/ug/dk/ext_programming_support_p19.html).

TC2050 layout requirements:

- use the official footprint dimensions and non-plated locating holes;
- mark pad 1 clearly on silkscreen;
- put the footprint somewhere accessible after assembly;
- orient the cable away from the RF antenna and J7 flex cable;
- keep components out of the probe and retaining-clip area; and
- use the `TC2050-CLIP` for extended debugging with the no-legs cable, or hold
  it in a production fixture for short programming operations.

Add a reset pushbutton from P0.18 to ground. The Holyiot board configuration
must enable `CONFIG_GPIO_AS_PINRESET=y`; the current DK build does not enable
that option.

### Debug-target power rules

Prefer to power the production PCB normally and connect its regulated VDD to
TC2050 pad 1 for target detection/reference. Do not configure P19 to source
power at the same time.

The DK normally runs at 3.0 V when USB-powered, and Nordic says the external
target voltage must match. During bring-up, either operate the module rail at
3.0 V or configure the DK/reference power arrangement for the target voltage.
Do not short DK solder bridge SB47 while the target is independently powered.

SEGGER RTT is the preferred bring-up log path. It operates through SWD and does
not consume SWO or a UART.

## Power architecture

### USB-powered prototype

For a board that only needs to operate while USB is connected:

```text
USB VBUS ── protection ─┬─ 5 V TrackPoint/backlight rail
                        └─ 3.0/3.3 V regulator ─ Holyiot and keyboard logic
```

Module pad 22 should see actual USB VBUS so firmware can detect USB presence.

### Truly wireless BLE operation

A wireless board still needs 5 V for the TrackPoint and backlight. It therefore
needs a battery power path in addition to the Holyiot regulator:

```text
USB-C ─ charger/power path ─ battery
                 │
                 ├─ 3.0/3.3 V regulator ─ Holyiot + keyboard logic
                 └─ 5 V boost/load switch ─ TrackPoint + backlight
```

Do not connect boosted `5V_SYS` back to Holyiot VBUS pad 22. That pad must
indicate real USB cable presence only. The final charger, battery, boost
converter, and backlight-current budget remain to be selected.

## Suspend, idle, and host wake behavior

The initial production firmware deliberately uses light CPU idle, not nRF52840
System OFF. Zephyr can idle the CPU between interrupts while the Bluetooth
controller maintains a connection or advertising. A matrix, power-button, or
TrackPoint interrupt wakes the CPU and ZMK immediately resumes reporting.

The shield configuration makes this policy explicit:

```text
CONFIG_USB_DEVICE_REMOTE_WAKEUP=y
CONFIG_ZMK_SLEEP=n
```

`CONFIG_ZMK_SLEEP=n` does **not** mean the CPU runs continuously. It prevents
ZMK's longer idle timeout from entering System OFF, which would terminate BLE
and require a hardware wake followed by boot and reconnection. Reconsider
System OFF only after measuring the finished board's battery life and wake
latency.

### USB wake

ZMK v0.3 requests USB remote wake automatically when it has a HID report to
send while USB is suspended. The host must have enumerated this keyboard, kept
VBUS present, enabled wake for the USB device, and armed remote wake before it
suspended. BIOS/UEFI and operating-system settings can still prevent wake.

USB wake follows ZMK's selected output. If BLE is selected, normal key and
TrackPoint reports go to BLE and are not duplicated to the sleeping USB host.
Select USB before suspending when the wired PC must be woken. Supporting
simultaneous wake of an unselected USB host would require a deliberate custom
endpoint policy and is not part of this design.

The matrix, Fn, power-button, and TrackPoint inputs must remain interrupt
capable. The current shield already marks its composite key scanner as a
devicetree wake source. Connect module pad 22 only to real USB VBUS so the
firmware can distinguish attached USB power from the boosted internal 5 V rail.

### BLE wake

ZMK's BLE HID service advertises the HID RemoteWake capability. While BLE is
selected, a bonded and connected host can therefore receive the first input
after the keyboard CPU wakes from idle. Whether that input wakes the whole PC
depends on the host Bluetooth controller, its driver, firmware, OS power
policy, and whether the controller remains powered during sleep. A host that
turns its Bluetooth radio off cannot be woken by this keyboard; the keyboard
will reconnect when that host resumes.

For predictable behavior, validate every intended host/OS combination. Keep
the power/connectivity LED off or at a very low duty cycle during idle so it
does not dominate battery consumption.

## Firmware and build plan

Keep the working DK target and add a separate production board target:

| Target | Purpose |
| --- | --- |
| `nrf52840dk_nrf52840 + thinkpad_t430` | Existing bench and TrackPoint testing |
| `holyiot_18010_nrf52840 + thinkpad_t430` | Production PCB |

The Holyiot board definition must provide:

- the production GPIO assignment in this document;
- the module's low-frequency crystal configuration;
- USB device support and internal flash/settings partitions;
- P0.18 hardware reset;
- J-Link/SWD runner support;
- no DK buttons, DK LEDs, QSPI flash, or J-Link UART assumptions;
- UARTE reception plus GPIO clock interrupts for the TrackPoint;
- interrupt-capable wake inputs for the matrix, Fn, power button, and
  TrackPoint, while retaining USB remote wake and connected BLE idle; and
- the existing TrackPoint interrupt-priority overrides regenerated from the
  compiled Holyiot devicetree.

Planned build variants:

| Build | Logging | Use |
| --- | --- | --- |
| Holyiot debug | SEGGER RTT | Initial PCB and TrackPoint bring-up |
| Holyiot USB log | ZMK `zmk-usb-logging` snippet | Routine testing without SWD attached |
| Holyiot release | Minimal or disabled debug logs | Normal keyboard use |

Initial and recovery programming will use SWD, approximately:

```sh
west flash -d build/thinkpad_t430_holyiot --runner jlink
```

The exact build directory and target name will be finalized when the board
definition is added. A Holyiot-specific UF2 or MCUboot configuration can be
added later for convenient USB updates, but SWD must remain accessible as the
guaranteed recovery path.

## PCB bring-up order

1. Inspect the unpowered PCB for shorts, particularly VBUS-to-VDD and 5 V-to-GPIO.
2. Apply only the regulated module supply; verify VDD and idle current.
3. Attach TC2050 and confirm the DK can identify and erase the nRF52840.
4. Flash a minimal Holyiot board test and confirm RTT logging and reset.
5. Test USB enumeration without the keyboard connected.
6. Connect only the passive matrix/Fn/power signals and verify all keys.
7. Enable the 5 V rail and verify TrackPoint supply and reset levels.
8. Connect TrackPoint DATA/CLOCK and repeat the existing movement/button tests.
9. Test each indicator MOSFET before attaching the keyboard LEDs.
10. Enable backlight power at minimum PWM duty, then test 50% and 100%.
11. Pair and exercise all three BLE profiles.
12. Test USB/BLE switching, power cycling, bond persistence, and SWD recovery.
13. Select USB, suspend the wired host, and verify a key, power button, and
    TrackPoint activity wake it; note whether the first input is consumed.
14. On every supported BLE host, select its profile, suspend it for at least 30
    minutes, then verify key and TrackPoint wake/reconnect behavior and latency.
15. Repeat the wake tests on battery power and confirm the keyboard remains
    responsive without an unexpected reboot or lost bond.

## Pre-fabrication checklist

- [ ] Purchased Holyiot pad layout checked against the 18010-A top-view drawing.
- [ ] J7 pin 1 orientation verified against the actual flex cable.
- [ ] Complete antenna keepout present on every copper layer.
- [ ] No 5 V net reaches an nRF GPIO or module VDD.
- [ ] All eight SENSE inputs rely on internal pull-ups.
- [ ] Fn and power inputs rely on internal pull-ups.
- [ ] TrackPoint low side relies on internal pull-ups; high side has 4.7 kΩ.
- [ ] Six BSS138 footprints fitted with correct source/drain orientation.
- [ ] LED current-limit and MOSFET gate pull-down resistors fitted.
- [ ] USB CC resistors, ESD protection, and VBUS protection included.
- [ ] SWDIO, SWDCLK, reset, target VDD, and ground reach the TC2050 footprint.
- [ ] TC2050 pad 3 is grounded so DK P19 selects the external target.
- [ ] Reset button and accessible VDD/GND test points included.
- [ ] USB VBUS detection is isolated from any boosted 5 V system rail.
- [ ] USB remote wake works with the intended BIOS/UEFI and OS settings.
- [ ] BLE wake behavior is verified on every intended host/OS combination.
- [ ] Idle current and 30-minute BLE response latency are measured on battery.
- [ ] Battery, charger, regulator, and 5 V boost current budgets finalized.
