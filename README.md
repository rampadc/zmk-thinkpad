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

The discovery build logs every matrix transition without sending a key to the
host. Flash this file through the DK's `JLINK` drive:

```sh
cp .zmk/build/thinkpad_t470/zephyr/zmk.hex /Volumes/JLINK/
```

Connect to the DK's J-Link virtual serial port at 115200 baud. On macOS, find
the port and open it with:

```sh
ls /dev/cu.usbmodem*
screen /dev/cu.usbmodemXXXX 115200
```

Replace `XXXX` with the actual suffix. Pressing a matrix key produces lines
like:

```text
T470_SCAN PRESS DRV3 SENSE6 position=30
T470_SCAN RELEASE DRV3 SENSE6 position=30
```

The separate Fn contact produces `T470_SCAN PRESS HOTKEY position=128`. Record
one coordinate per physical key. To leave `screen`, press Control-A, then K,
then Y.
