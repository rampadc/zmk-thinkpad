# ThinkPad T430 ZMK firmware

This repository turns a ThinkPad T430 keyboard and TrackPoint into a USB and
Bluetooth keyboard using a Nordic nRF52840 DK. It currently supports:

- the complete US QWERTY keyboard matrix, Fn key, power button, and dedicated
  media buttons;
- TrackPoint movement and all three TrackPoint buttons;
- USB and BLE output, with three independently bonded BLE hosts;
- automatic wake reporting over the selected USB or BLE transport;
- keyboard backlight control with **Fn+Space**;
- the power-button LED for connection state; and
- the speaker-mute and microphone-mute LEDs; and
- three low-power BLE-profile indicator LEDs.

Planning the custom Holyiot board? Start with the
[Holyiot preliminary PCB wiring and power plan](docs/preliminary-pcb-wiring.md). The
Holyiot GPIO assignment is intentionally optimized for PCB routing and is
different from the DK bring-up wiring documented below. A separate
[preliminary home-assembly BOM](docs/preliminary-bom.md) is an LCSC cut-tape-oriented
shopping list using SMD modules, jelly-bean parts, and E12/E24 resistor values.

## Before wiring anything

The nRF52840 is a **3.3 V device and its GPIOs are not 5 V tolerant**. Do not
connect a 5 V keyboard or TrackPoint signal directly to it. Use suitable level
conversion or driver circuitry and connect the keyboard, converter, and DK
grounds together.

All passive, active-low inputs on the nRF side are configured to use the
nRF52840's **internal pull-up resistors**. The firmware does not expect external
pull-up resistors on any nRF GPIO:

- all eight keyboard `SENSE` inputs;
- the separate Fn (`-HOTKEY`) and power-button (`-PWRSWITCH`) inputs;
- all four DK switches; and
- TrackPoint `DATA` and `CLOCK` on the 3.3 V side.

There is one electrical distinction that firmware cannot remove: PS/2 is an
open-collector bus. If a BSS138 translator separates a 3.3 V bus from a 5 V
bus, the nRF's internal pull-up only pulls up the **3.3 V side**. The 5 V side
must already have its own pull-up, either inside the TrackPoint/adapter or on
the translator module. Never use that 5 V pull-up on an nRF pin.

## T430 connector wiring

The connector names and pin numbers below refer to `J7` in the T430 schematic.
Confirm the connector orientation and check continuity before applying power;
the physical left-to-right order depends on which side of the keyboard cable
you are viewing.

### Keyboard matrix

The T430 has 16 matrix drive outputs and eight active-low sense inputs. Every
`SENSE` pin below has its nRF internal pull-up enabled.

| Signal | J7 pin | nRF52840 GPIO | Signal | J7 pin | nRF52840 GPIO |
| --- | ---: | --- | --- | ---: | --- |
| `DRV0` | 22 | P0.03 | `SENSE0` | 5 | P1.04 |
| `DRV1` | 18 | P0.04 | `SENSE1` | 13 | P1.05 |
| `DRV2` | 14 | P0.17 | `SENSE2` | 9 | P1.06 |
| `DRV3` | 10 | P0.19 | `SENSE3` | 7 | P1.07 |
| `DRV4` | 2 | P0.20 | `SENSE4` | 11 | P1.09 |
| `DRV5` | 4 | P0.21 | `SENSE5` | 3 | P1.10 |
| `DRV6` | 8 | P0.22 | `SENSE6` | 15 | P1.11 |
| `DRV7` | 12 | P0.23 | `SENSE7` | 17 | P1.13 |
| `DRV8` | 6 | P0.28 |  |  |  |
| `DRV9` | 20 | P0.29 |  |  |  |
| `DRV10` | 16 | P0.30 |  |  |  |
| `DRV11` | 24 | P0.31 |  |  |  |
| `DRV12` | 28 | P1.00 |  |  |  |
| `DRV13` | 32 | P1.01 |  |  |  |
| `DRV14` | 26 | P1.02 |  |  |  |
| `DRV15` | 30 | P1.03 |  |  |  |

P0.17 and P0.19-P0.23 are connected to the DK's onboard QSPI flash. This
firmware disables QSPI and reuses those pins for the keyboard matrix.

### Separate controls, backlight, and LEDs

| T430 signal | J7 pin | nRF52840 GPIO | Direction and behavior |
| --- | ---: | --- | --- |
| `-HOTKEY` (Fn) | 1 | P1.14 | Input, active low, internal pull-up |
| `-PWRSWITCH` | 19 | P1.15 | Input, active low, internal pull-up |
| `KBD_BL_PWM` | 25 | P1.12 | 1 kHz PWM output |
| `-LEDPWR` | 23 | P0.13 | Output, active low |
| `-LED_MUTE` | 33 | P0.14 | Output, active low |
| `-LEDMICMUTE` | 36 | P0.15 | Output, active low |

The LED and backlight GPIOs are logic/control outputs, not power supplies. Use
appropriate transistor or level-shifting circuitry for the T430's original
loads, and do not allow their keyboard-side voltage onto an nRF GPIO.

During DK bring-up, P0.13-P0.15 also drive onboard LED1-LED3, so connection,
mute, and mic-mute state can be tested before the keyboard LEDs are connected.

### BLE-profile LEDs on the DK

The preliminary PCB will have three adjacent LEDs labeled `1`, `2`, and `3`.
The DK build provides the same firmware behavior using LED4 plus two external
LEDs:

| Profile | DK GPIO | Connection |
| ---: | --- | --- |
| 1 | P0.16 | DK onboard LED4 |
| 2 | P0.02 | `3.3 V -> 4.7 kΩ -> LED anode`; LED cathode to P0.02 |
| 3 | P0.05 | `3.3 V -> 4.7 kΩ -> LED anode`; LED cathode to P0.05 |

The two external LEDs are active-low: the GPIO sinks approximately 0.2-0.3 mA
when illuminated. Use amber LEDs to match the preliminary design; red is an
acceptable substitute. Do not use green, blue, or white LEDs.
P0.05 is normally the DK virtual-serial RTS signal; this shield releases RTS
while retaining UART TX/RX logging through the J-Link serial port.

### TrackPoint

| TrackPoint signal | J7 pin | nRF52840 GPIO | DK header |
| --- | ---: | --- | --- |
| `TP4DATA` | 37 | P0.26 | D14 / SDA |
| `TP4CLK` | 39 | P0.27 | D15 / SCL |
| `TP4_RESET` | 40 | P1.08 | D7 |

`DATA` and `CLOCK` use internal pull-ups on the nRF side. Power the TrackPoint
from the correct translated/external supply; do not power it from an nRF GPIO.

## Keys and controls

The base layer is a normal US QWERTY T430 layout. The separate power button
toggles between USB and BLE output. The four DK buttons remain available as
recovery controls even after the complete keyboard is connected.

| Control | Action |
| --- | --- |
| Power button | Toggle USB/BLE output |
| Fn+Power | Advance to the next BLE host |
| Fn+1 / Fn+2 / Fn+3 | Select and connect to BLE host 1 / 2 / 3 |
| Fn+4 | Select USB output |
| Fn+Delete | Clear only the selected BLE profile and advertise for pairing |
| Fn+Space | Cycle keyboard backlight through off, 50%, and 100% |
| SW1 | Select USB output |
| SW2 | Select BLE output |
| SW3 | Advance to the next BLE host |
| SW4 | Clear the selected BLE profile and advertise for pairing |

The original T430-style Fn shortcuts are also present:

| Shortcut | Action | Shortcut | Action |
| --- | --- | --- | --- |
| Fn+F3 | Lock | Fn+F8 | Brightness down |
| Fn+F4 | Sleep | Fn+F9 | Brightness up |
| Fn+F5 | Wireless control | Fn+F10 | Previous track |
| Fn+F6 | System control panel | Fn+F11 | Play/pause |
| Fn+F7 | Display switch (Win+P) | Fn+F12 | Next track |
| Fn+B | Break | Fn+P | Pause |
| Fn+S | SysRq | Fn+K | Scroll Lock |

Media and system-key behavior depends on host OS support. The mute and
microphone-mute LEDs currently toggle when their matching physical T430 button
is pressed; they do not yet receive mute-state changes made elsewhere on the
host.

## Power-button connection LED

The LED inside the power button shows the selected output's state:

| Pattern | Meaning |
| --- | --- |
| Solid | Selected USB HID is ready, or selected BLE host is connected |
| Fast blink | Selected BLE profile is empty and advertising for pairing |
| Short pulse every second | Bonded BLE host is disconnected/reconnecting |
| Short pulse every second while USB is selected | USB is not enumerated |

## BLE-profile indicator LEDs

Only the currently selected profile LED is driven. The other two remain off:

| Pattern | Meaning |
| --- | --- |
| Fast blink | Selected profile is empty and advertising for pairing |
| Short pulse every two seconds | Selected profile is bonded but disconnected |
| Solid for 2.5 seconds | Profile was selected or has just connected |
| Off after the solid indication | Selected profile remains connected, or USB is selected |

The power-button LED continues to show the overall selected transport and
connection state. The separate profile LEDs answer which BLE slot is active
without leaving another LED continuously lit.

## Build locally

Run every command below from the repository root: the directory named
`zmk-config-t430`. The blocks which need `.zmk` use a temporary subshell, so
your terminal returns to the repository root automatically.

> Copy only the text inside each command box. Do not paste the three backtick
> characters. If the terminal shows `bquote>`, press **Control-C** once and
> paste the command again without the backticks.

If your prompt currently says `.zmk`, return to the repository root first:

```sh
cd ..
```

Confirm the current directory:

```sh
pwd
```

The last part should be `/zmk-config-t430`.

### 1. Install the tools

On macOS with Homebrew:

```sh
brew install git python cmake ninja west arm-none-eabi-gcc
```

### 2. Set up ZMK (first time only)

Run this complete block from `zmk-config-t430`:

```sh
mkdir -p .zmk/config
ln -sfn "$PWD/config/west.yml" .zmk/config/west.yml

if [ ! -d .zmk/.west ]; then
  (cd .zmk && west init -l config)
fi

(cd .zmk && west update --fetch-opt=--filter=tree:0)

python3 -m venv .zmk/.venv

(
  cd .zmk
  . .venv/bin/activate
  python -m pip install -r zephyr/scripts/requirements.txt
  west zephyr-export
)
```

The `if` check makes this safe if `.zmk` is already initialized. `File exists`
or `already initialized` usually means that step was completed previously.

### 3. Build the firmware

Run this complete block from `zmk-config-t430`:

```sh
(
  cd .zmk
  . .venv/bin/activate
  ZEPHYR_TOOLCHAIN_VARIANT=cross-compile \
  CROSS_COMPILE="$(brew --prefix)/bin/arm-none-eabi-" \
  west build -p always -s zmk/app -d build/thinkpad_t430 \
    -b nrf52840dk_nrf52840 -- \
    -DSHIELD=thinkpad_t430 \
    -DZMK_CONFIG="$PWD/../config" \
    -DZMK_EXTRA_MODULES="$PWD/.."
)
```

The finished firmware is:

```text
.zmk/build/thinkpad_t430/zephyr/zmk.hex
```

Use the same build block again after editing the firmware. The setup block is
only needed once.

## Flash and view logs

The simplest flashing method is drag-and-drop through the DK debugger:

1. Connect the DK's interface/debug USB port to the Mac.
2. Wait for a drive named `JLINK`.
3. From `zmk-config-t430`, run:

```sh
cp .zmk/build/thinkpad_t430/zephyr/zmk.hex /Volumes/JLINK/
```

The drive can disconnect and reconnect while programming. That is normal.

If SEGGER J-Link is installed, this root-level command is an alternative:

```sh
(
  cd .zmk
  . .venv/bin/activate
  west flash -d build/thinkpad_t430 --runner jlink
)
```

UART0 logs use the interface/debug USB virtual serial port at 115200 baud. For
example, install `tio` with `brew install tio`, then run:

```sh
ls /dev/cu.usbmodem*
tio -b 115200 /dev/cu.usbmodemXXXX
```

Replace the example path with the device reported by `ls`. Connect the DK's
second, nRF USB port to the host when testing USB keyboard/mouse output.

Enabling new HID features changes the BLE descriptor. Remove any older bond
for this keyboard from the host, clear the matching keyboard profile with
Fn+Delete or SW4, and pair it again before testing BLE.

## Hardware and keymap references

- The connector signal names and pin numbers come from the
  [T430 motherboard schematic](https://indiarefix.in/download/file.php?id=9313),
  at keyboard connector `J7`.
- The matrix positions are based on the
  [ThinkPad EC xx30 matrix documentation](https://github.com/hamishcoleman/thinkpad-ec/blob/master/docs/table_matrix.txt)
  and its [X230 matrix source](https://github.com/hamishcoleman/thinkpad-ec/blob/master/asm/ec_key_matrix_x230.mac),
  which applies to the shared xx30 keyboard generation.
- The Fn behavior follows the
  [official Lenovo ThinkPad T430/T430i user guide](https://download.lenovo.com/pccbbs/mobiles_pdf/t430_t430i_ug_en.pdf).

## TrackPoint driver attribution

TrackPoint support is provided by the
[`kb_zmk_ps2_mouse_trackpoint_driver`](https://github.com/infused-kim/kb_zmk_ps2_mouse_trackpoint_driver)
module developed by [Kim Streich (`infused-kim`)](https://github.com/infused-kim)
and other ZMK contributors. This configuration also incorporates the Zephyr
compatibility work maintained in
[badjeff's fork](https://github.com/badjeff/kb_zmk_ps2_mouse_trackpoint_driver).

For reproducible builds, West fetches
[`rampadc/kb_zmk_ps2_mouse_trackpoint_driver`](https://github.com/rampadc/kb_zmk_ps2_mouse_trackpoint_driver),
a preservation fork pinned to commit
`7ab7846a95b233c3ada0e0605b972f36353e7bc4`. Credit for the driver remains
with its original authors and contributors.
