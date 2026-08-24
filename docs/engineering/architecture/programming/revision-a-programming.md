---
title: Revision A programming
description: First installation, USB/UF2 updates, recovery, and debugging on the custom Revision A PCB.
---

# Revision A programming guide

This is the operational guide for the custom Revision A PCB containing the
Holyiot 18010 nRF52840 module. It applies to both keyboard variants:

| Keyboard | Board | Shield | Build directory |
| --- | --- | --- | --- |
| T430 | `holyiot_18010_nrf52840` | `thinkpad_t430_reva` | `.zmk/build/thinkpad_t430_reva` |
| T60/T61 | `holyiot_18010_nrf52840` | `thinkpad_t60_reva` | `.zmk/build/thinkpad_t60_reva` |

The normal firmware layout contains an Adafruit-compatible UF2 bootloader.
ZMK begins at `0x01000`; settings and BLE bonds occupy
`0xEA000-0xF1FFF`. Never use `mass_erase` or `recover` for an ordinary update.

## Normal development: USB and UF2

After the bootloader has been installed once, use USB for every normal
iteration. This requires no button, Tag-Connect cable, ST-LINK, or J-Link.
The running firmware exposes a CDC ACM port and enters its bootloader when that
port is opened at 1200 baud.

For T430, the repository helper performs the complete build and update:

```sh
tools/flash-t430-reva
```

For another Revision A shield, build it explicitly. This T60 example can be
adapted by changing `shield` and `build_dir`:

```sh
repo_root=$PWD
shield=thinkpad_t60_reva
build_dir="$repo_root/.zmk/build/$shield"

(
  cd "$repo_root/.zmk"
  ZEPHYR_TOOLCHAIN_VARIANT=cross-compile \
  CROSS_COMPILE="$(brew --prefix)/bin/arm-none-eabi-" \
  .venv/bin/python -m west build -p always -s zmk/app \
    -d "$build_dir" -b holyiot_18010_nrf52840 -- \
    -DSHIELD="$shield" \
    -DZMK_CONFIG="$repo_root/config" \
    -DZMK_EXTRA_MODULES="$repo_root" \
    -DEXTRA_CONF_FILE="$repo_root/config/holyiot_t430_reva_release.conf"
)
```

Request the bootloader from macOS:

```sh
port=$(find /dev -maxdepth 1 -name 'cu.usbmodem*' -print | head -n 1)
PORT="$port" python3 - <<'PY'
import os
import termios
import time

fd = os.open(os.environ["PORT"], os.O_RDWR | os.O_NOCTTY)
attrs = termios.tcgetattr(fd)
attrs[4] = termios.B1200
attrs[5] = termios.B1200
termios.tcsetattr(fd, termios.TCSANOW, attrs)
time.sleep(0.5)
os.close(fd)
PY
```

Wait for `/Volumes/T430BOOT`, then copy the selected build's UF2 file:

```sh
while [ ! -d /Volumes/T430BOOT ]; do sleep 0.1; done
cp -X "$build_dir/zephyr/zmk.uf2" /Volumes/T430BOOT/ZMK2.UF2
```

The volume normally disappears as soon as the copy completes. This is
expected. The UF2 update cannot overwrite the settings partition, so existing
BLE bonds and requested transport/profile state are preserved.

Do not copy an ordinary update to `/Volumes/JLINK`; that is the Nordic DK
mass-storage programmer and may erase settings.

## First installation or recovery: STM32F429I-DISCO ST-LINK

Use the Discovery board only when the UF2 bootloader is absent or no longer
starts. Its ST-LINK/V2-B is the programmer; the target PCB must power itself.

Before connecting:

1. Remove both `CN4` jumpers so ST-LINK is isolated from the Discovery's
   onboard STM32F429.
2. Keep `SB7` open when nRESET is connected.
3. Power Revision A normally through USB-C and confirm its VCC is about 3.0 V.
4. Never connect the Discovery `3V3`, `5V`, or `5V USB` output to PCB VCC.
5. `VDD_TARGET` is voltage sense only in this setup; it observes PCB VCC.

### SWD wiring

The STM32F429I-DISCO `CN2` SWD header maps to the Revision A ARM 10-pin
Tag-Connect footprint as follows:

| Discovery CN2 | Signal | Tag-Connect / IDC pin |
| --- | --- | --- |
| 1 | `VDD_TARGET` voltage reference | 1 `VCC` |
| 2 | `SWCLK` | 4 `NRF_SWDCLK` |
| 3 | GND | 3, 5, or 9 GND |
| 4 | `SWDIO` | 2 `NRF_SWDIO` |
| 5 | `NRST` | 10 `-NRF_RESET` |
| 6 | `SWO` | leave disconnected |

Only one ground connection is required. Tag-Connect pins 6, 7, and 8 remain
unconnected. Hold a no-legs Tag-Connect probe firmly throughout each OpenOCD
operation.

### Probe before writing

With PCB USB-C power connected, verify the nRF52840 FICR device ID at 1 MHz:

```sh
openocd \
  -f interface/stlink-dap.cfg \
  -c "transport select dapdirect_swd" \
  -f target/nrf52.cfg \
  -c "adapter speed 1000; init; reset halt; mdw 0x10000060 2; reset run; exit"
```

If OpenOCD cannot read the target, recheck probe orientation, common ground,
`VDD_TARGET`, SWDIO, SWCLK, reset, CN4, and PCB power. If it reports access
protection, stop. Do not erase or recover without explicit authorization,
because that destroys settings and BLE bonds.

### Install the bootloader and ZMK

Build the bootloader:

```sh
tools/build-t430-reva-uf2-bootloader.sh
```

Build the desired ZMK shield first. The T430 helper builds
`.zmk/build/thinkpad_t430_reva`; use the explicit build command in the USB
section for T60.

Program only the addresses contained in the bootloader and application HEX
files, with verification and no mass erase:

```sh
shield=thinkpad_t430_reva # or thinkpad_t60_reva

openocd \
  -f interface/stlink-dap.cfg \
  -c "transport select dapdirect_swd" \
  -f target/nrf52.cfg \
  -c "adapter speed 1000; init; reset halt; program .zmk/build/thinkpad_t430_reva_bootloader/thinkpad_t430_reva_bootloader_nosd.hex verify; program .zmk/build/$shield/zephyr/zmk.hex verify reset exit"
```

After the target restarts, disconnect the SWD probe and confirm that its USB
CDC port enumerates. All subsequent development should use the USB/UF2 method.

## Diagnostic firmware

For verbose logs over the Revision A USB CDC port, substitute:

```text
config/holyiot_t430_reva_usb_debug.conf
```

for the release configuration. Connect after flashing with:

```sh
tio -b 115200 /dev/cu.usbmodem101
```

The exact numeric suffix may differ; locate it with
`find /dev -maxdepth 1 -name 'cu.usbmodem*' -print`.
