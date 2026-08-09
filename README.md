# ThinkPad T430 ZMK firmware

This repository currently contains the TrackPoint bring-up firmware for a
ThinkPad T430 keyboard connected to a Nordic nRF52840 DK.

## TrackPoint wiring

The nRF52840 GPIOs are **not 5 V tolerant**. Connect these signals only through
a bidirectional, open-collector-compatible level conversion stage and connect
the TrackPoint and DK grounds together.

| TrackPoint signal | T430 connector signal | nRF52840 pin | DK header |
| --- | --- | --- | --- |
| DATA | `TP4DATA` | P0.26 | D14 / SDA |
| CLOCK | `TP4CLK` | P0.27 | D15 / SCL |
| RESET | `TP4_RESET` | P1.08 | D7 |

Power the TrackPoint from the appropriate external 5 V supply; do not power it
from an nRF52840 GPIO.

## Build locally

All commands in this guide are run from the repository root—the directory named
`zmk-config-t430`. The commands that need to work inside `.zmk` use a temporary
subshell, so your terminal stays at the repository root.

> **Copy only the text inside each command box.** Do not type or paste the
> three backtick characters shown around a command box. If the terminal displays
> `bquote>`, press **Control-C** once to cancel, then paste the command again
> without the backticks.

If your prompt currently says `.zmk`, return to the repository root first:

```sh
cd ..
```

You can confirm that you are in the right place with:

```sh
pwd
```

The last part should be `/zmk-config-t430`.

### 1. Install the tools

On macOS with Homebrew, the required command-line tools can be installed with:

```sh
brew install git python cmake ninja west arm-none-eabi-gcc
```

### 2. Set up ZMK (first time only)

Run this whole block from `zmk-config-t430`:

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

The `if` check makes this safe when `.zmk` has already been initialized. The
messages `File exists` and `already initialized` mean you previously completed
that part; they do not mean the firmware is broken.

### 3. Build the firmware

Run this block from `zmk-config-t430`. It uses the Homebrew ARM toolchain and
performs a clean build:

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

Use the same build block again after changing the firmware. You do not need to
repeat the setup block.

## Flash and view logs

The easiest flashing method does not require another command-line tool:

1. Connect the DK's interface/debug USB port to the Mac.
2. Wait for a drive named `JLINK` to appear.
3. From `zmk-config-t430`, run:

```sh
cp .zmk/build/thinkpad_t430/zephyr/zmk.hex /Volumes/JLINK/
```

The drive may disconnect and reconnect while the DK programs itself. That is
normal.

If SEGGER J-Link is installed, this root-level command is an alternative:

```sh
(
  cd .zmk
  . .venv/bin/activate
  west flash -d build/thinkpad_t430 --runner jlink
)
```

UART0 logs are emitted through the interface/debug USB virtual serial port at
115200 baud. For example, install `tio` with `brew install tio`, then run:

```sh
ls /dev/cu.usbmodem*
tio -b 115200 /dev/cu.usbmodemXXXX
```

Replace the example device with the one reported by `ls`. A successful startup
should identify the PS/2 device and finish with data reporting enabled rather
than repeated clock or data timeouts.

Connect the DK's second, nRF USB port to the host when testing USB HID output.

## DK controls

| DK control | Action |
| --- | --- |
| SW1 | Select USB output |
| SW2 | Select BLE output |
| SW3 | Select the next of three BLE profiles |
| SW4 | Clear the current BLE profile and advertise for pairing |

The GitHub Actions build artifact is named `thinkpad_t430_trackpoint`. Extract
its `.hex` file and flash it using either method above.

For the first functional test, press SW1 and verify movement plus all three
buttons over USB. Press SW2 for BLE, use SW3 to choose a profile, and press SW4
to clear that profile before pairing it with a host. Repeat for all three
profiles.

Enabling pointing changes the BLE HID descriptor. Remove any older bond for
this keyboard from the host, clear the matching keyboard profile, and pair it
again before testing BLE pointing.

## Attribution

TrackPoint support is provided by the
[`kb_zmk_ps2_mouse_trackpoint_driver`](https://github.com/infused-kim/kb_zmk_ps2_mouse_trackpoint_driver)
module developed by [Kim Streich (`infused-kim`)](https://github.com/infused-kim)
and other ZMK contributors. This configuration also incorporates the Zephyr
compatibility work maintained in [badjeff's fork](https://github.com/badjeff/kb_zmk_ps2_mouse_trackpoint_driver).

For reproducible builds, West fetches
[`rampadc/kb_zmk_ps2_mouse_trackpoint_driver`](https://github.com/rampadc/kb_zmk_ps2_mouse_trackpoint_driver),
a preservation fork pinned to commit
`7ab7846a95b233c3ada0e0605b972f36353e7bc4`. Credit for the driver remains
with its original authors and contributors.
