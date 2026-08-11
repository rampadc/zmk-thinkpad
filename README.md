# ThinkPad T470 ZMK firmware

This repository provides ZMK firmware for a US-layout ThinkPad T470 keyboard
using a Nordic nRF52840 DK. The current scope is deliberately narrow:

- scan the 16 x 8 keyboard matrix;
- scan the separate Fn (`-HOTKEY`) contact;
- expose USB and BLE keyboard transports; and
- provide the complete 84-key US keymap, including the separate Fn key.

TrackPoint, mouse buttons, backlight, indicator LEDs, and other keyboard
electronics are not implemented yet.

The complete connector pinout, temporary DK matrix wiring, and proposed
custom-controller connectivity are in
[docs/preliminary-pcb-wiring.md](docs/preliminary-pcb-wiring.md). This guide assigns
the same Holyiot 18010 module used by the T430 project and includes TrackPoint,
buttons, backlight, keyboard indicators, USB/SWD reservations, voltage-domain
options, and first-article checks. It is a hardware plan, not yet a firmware
target.

## Build

The GitHub workflow builds the shield automatically. For a local checkout with
ZMK initialized under `.zmk`, the target is:

```sh
west build -s .zmk/zmk/app -d .zmk/build/thinkpad_t470 -b nrf52840dk_nrf52840 -- \
  -DSHIELD=thinkpad_t470 \
  -DZMK_CONFIG="$PWD/boards/shields/thinkpad_t470" \
  -DZMK_EXTRA_MODULES="$PWD"
```

The default firmware is ready for normal typing. Holding Fn provides the
ThinkPad media functions on F1 through F6: mute, volume down/up, microphone
mute, and brightness down/up. It also provides transport and BLE-profile
controls:

- Fn+F7 selects USB output.
- Fn+F8 selects BLE output.
- Fn+F9, Fn+F10, and Fn+F11 select BLE profiles 1, 2, and 3.
- Fn+F12 clears the bond for the currently selected BLE profile so it can be
  paired again.

The selected output and BLE profile are retained across resets. To pair a
device, select one of the three profiles, select BLE output, and pair with the
advertised ZMK keyboard from the host.

## Optional matrix rediscovery

The discovery helper remains available if another T470 keyboard needs to be
mapped. Enable it temporarily by adding this line to
`boards/shields/thinkpad_t470/thinkpad_t470.conf` and rebuilding:

```text
CONFIG_THINKPAD_T470_MATRIX_DISCOVERY=y
```

It guides you through all 84 keys without sending keys to the host. Flash the
result through the DK's `JLINK` drive:

```sh
cp .zmk/build/thinkpad_t470/zephyr/zmk.hex /Volumes/JLINK/
```

Connect to the DK's J-Link virtual serial port at 115200 baud. On macOS, find
the port and open it with:

```sh
ls /dev/cu.usbmodem*
screen /dev/cu.usbmodemXXXX 115200
```

Replace `XXXX` with the actual suffix. After boot, the UART prompts for one
physical key at a time:

```text
T470_GUIDE [1/84] PRESS ESC
T470_GUIDE CAPTURED ESC = DRV3 SENSE6 position=30
T470_GUIDE [2/84] PRESS F1
```

Press and release the requested key; the next prompt appears automatically.
Duplicate coordinates are rejected. After the last key, the firmware prints a
complete `T470_MAP_BEGIN` through `T470_MAP_END` summary. Reset the DK to restart
the sequence. To leave `screen`, press Control-A, then K, then Y.
