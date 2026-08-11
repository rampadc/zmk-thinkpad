# ThinkPad T470 ZMK firmware

This repository is an initial ZMK bring-up target for a ThinkPad T470 keyboard
using a Nordic nRF52840 DK. The current scope is deliberately narrow:

- scan the 16 x 8 keyboard matrix;
- scan the separate Fn (`-HOTKEY`) contact;
- expose USB and BLE keyboard transports; and
- provide a safe, inert raw matrix map for discovering physical key positions.

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

The initial keymap intentionally emits no HID keys. Use debug logging to map
each physical key to its `DRVn`/`SENSEn` coordinate before adding the final
physical layout and bindings.

## Scan the matrix on the nRF52840 DK

The discovery build guides you through all 84 keys on a US T470 keyboard
without sending keys to the host. Flash this file through the DK's `JLINK`
drive:

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
