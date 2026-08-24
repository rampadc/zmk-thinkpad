# ThinkPad ZMK

ThinkPad ZMK is open firmware and controller hardware for turning classic
ThinkPad keyboards into standalone USB and Bluetooth keyboards. It is built on
[ZMK](https://zmk.dev/) and targets the Nordic nRF52840.

The firmware supports T430 and T470 keyboards on an nRF52840 development kit.
The T430 target includes TrackPoint support; the current T470 target is limited
to the keyboard matrix. The custom Revision A controller supports T430 and T60
keyboards, including their TrackPoints. Revision A established the project's
first-time programming, USB/UF2 update, and serial-debugging workflows.

Revision B is the next planned hardware phase, beginning with a T470 adapter.
It has not been built.

> **Project status:** under active development and not yet released as a
> finished product.

## Build

The commands below assume macOS with Homebrew and are run from the repository
root.

Install the required tools:

```sh
brew install git python cmake ninja west arm-none-eabi-gcc
```

Initialize the local ZMK workspace once:

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

Build a development-kit target by setting `shield` to `thinkpad_t430` or
`thinkpad_t470`:

```sh
shield=thinkpad_t470

(
  cd .zmk
  . .venv/bin/activate
  ZEPHYR_TOOLCHAIN_VARIANT=cross-compile \
  CROSS_COMPILE="$(brew --prefix)/bin/arm-none-eabi-" \
  .venv/bin/python -m west build -p always -s zmk/app \
    -d "build/$shield" -b nrf52840dk_nrf52840 -- \
    -DSHIELD="$shield" \
    -DZMK_CONFIG="$PWD/../config" \
    -DZMK_EXTRA_MODULES="$PWD/.."
)
```

The resulting image is `.zmk/build/<shield>/zephyr/zmk.hex`.

For the custom T430 Revision A controller, the build and USB update are wrapped
by:

```sh
tools/flash-t430-reva
```

## Flash

Normal nRF52840 DK updates must use J-Link Commander's `loadfile` command so
the settings partition and existing Bluetooth bonds are preserved:

```text
JLinkExe -device NRF52840_XXAA -if SWD -speed 4000 -autoconnect 1
```

At the `J-Link>` prompt:

```text
loadfile /absolute/path/to/.zmk/build/thinkpad_t470/zephyr/zmk.hex
r
g
exit
```

Do not copy an ordinary firmware update to `/Volumes/JLINK`; the DK
mass-storage programming path can erase settings and Bluetooth bonds.

After its one-time bootloader installation, Revision A uses the
`tools/flash-t430-reva` USB/UF2 workflow above. SWD programming is reserved for
first installation and recovery.

## Documentation

Architecture, specifications, hardware records, and programming procedures are
published in the
[ThinkPad ZMK documentation](https://rampadc.github.io/zmk-thinkpad/).
