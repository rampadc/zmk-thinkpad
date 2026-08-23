# Flashing nRF52840 DK firmware

## Default: preserve BLE profiles and settings

Use J-Link Commander for normal development flashes. Its `loadfile` command
programs the addresses present in the firmware image without erasing the
separate settings partition.

For the connected nRF52840 DK:

```text
JLinkExe -device NRF52840_XXAA -if SWD -speed 4000 -autoconnect 1
```

At the `J-Link>` prompt, enter:

```text
loadfile /absolute/path/to/zmk.hex
r
g
exit
```

For the T470 build in this repository, the image is normally:

```text
.zmk/build/thinkpad_t470/zephyr/zmk.hex
```

Use the corresponding `thinkpad_t430` path for T430 firmware.

This method was verified on the T470 DK by reading all 32 KiB of the settings
partition before and after `loadfile`. The two copies were byte-for-byte
identical, and BLE profile 1 reconnected without pairing again.

## Destructive clean flash

Copying the HEX file to `/Volumes/JLINK` invokes the DK's mass-storage
programmer. In our hardware test this erased the BLE bond even though the HEX
file itself did not contain settings addresses.

Use this method only when a clean settings/bond reset is deliberately required:

```sh
cp path/to/zmk.hex /Volumes/JLINK/zmk.hex
sync
```

Afterward, all BLE hosts may need to be paired again using their security
codes. State this consequence before performing the operation.

## Storage layout and diagnostic check

On the current nRF52840 DK builds:

- Application firmware begins at `0x00000`.
- Internal settings storage is `0xF8000–0xFFFFF` (32 KiB).
- BLE host addresses, bonds, keys, and the requested ThinkPad transport/profile
  state live in that settings partition.

To investigate suspected data loss, halt the target and save the partition
before and after flashing:

```text
savebin tmp/thinkpad-settings-before.bin 0xF8000 0x8000
savebin tmp/thinkpad-settings-after.bin 0xF8000 0x8000
```

Then compare the files with `cmp` or SHA-256. Always reset and resume the target
with `r` followed by `g` after diagnostic reads.

## T430 Revision A Holyiot with ST-LINK

The custom Revision A controller uses the `holyiot_18010_nrf52840` board and
`thinkpad_t430_reva` shield. Its normal layout includes an Adafruit-compatible
UF2 bootloader:

- Nordic MBR: `0x00000-0x00FFF`
- ZMK application: `0x01000-0xE9FFF`
- ZMK settings and BLE bonds: `0xEA000-0xF1FFF`
- protected DFU workspace: `0xF2000-0xF3FFF`
- UF2 bootloader and metadata: `0xF4000-0xFFFFF`

Build the minimal RTT smoke image first:

```sh
(
  cd .zmk
  ZEPHYR_TOOLCHAIN_VARIANT=cross-compile \
  CROSS_COMPILE="$(brew --prefix)/bin/arm-none-eabi-" \
  .venv/bin/python -m west build -p always -s zephyr/samples/hello_world \
    -d build/holyiot_18010_smoke -b holyiot_18010_nrf52840 -- \
    -DBOARD_ROOT="$PWD/.." \
    -DEXTRA_CONF_FILE="$PWD/../config/holyiot_18010_smoke.conf"
)
```

For USB-only bring-up, build Zephyr's CDC ACM sample in a separate directory:

```sh
(
  cd .zmk
  ZEPHYR_TOOLCHAIN_VARIANT=cross-compile \
  CROSS_COMPILE="$(brew --prefix)/bin/arm-none-eabi-" \
  .venv/bin/python -m west build -p always -s zephyr/samples/subsys/usb/cdc_acm \
    -d build/holyiot_18010_usb_smoke -b holyiot_18010_nrf52840 -- \
    -DBOARD_ROOT="$PWD/.."
)
```

Build the full RTT debug image during bring-up:

```sh
(
  cd .zmk
  ZEPHYR_TOOLCHAIN_VARIANT=cross-compile \
  CROSS_COMPILE="$(brew --prefix)/bin/arm-none-eabi-" \
  .venv/bin/python -m west build -p always -s zmk/app \
    -d build/thinkpad_t430_reva_debug \
    -b holyiot_18010_nrf52840 -- \
    -DSHIELD=thinkpad_t430_reva \
    -DZMK_CONFIG="$PWD/../config" \
    -DZMK_EXTRA_MODULES="$PWD/.." \
    -DEXTRA_CONF_FILE="$PWD/../config/holyiot_t430_reva_debug.conf"
)
```

After bring-up, build the release image into the public output directory by
using `build/thinkpad_t430_reva` and
`config/holyiot_t430_reva_release.conf` in the same command.

For the STM32F429I-DISCO ST-LINK/V2-B, remove both CN4 jumpers. Power the
target normally at 3.0 V, connect that rail only to CN2 `VDD_TARGET`, and do
not connect a Discovery 3 V or 5 V output. Probe the device before writing:

```sh
openocd \
  -f interface/stlink-dap.cfg \
  -c "transport select dapdirect_swd" \
  -f target/nrf52.cfg \
  -c "adapter speed 1000; init; reset halt; mdw 0x10000060 2; reset run; exit"
```

The one-time bootloader installation requires SWD. Build it with:

```sh
tools/build-t430-reva-uf2-bootloader.sh
```

Then flash the bootloader and boot-aware ZMK image without a mass erase:

```sh
openocd \
  -f interface/stlink-dap.cfg \
  -c "transport select dapdirect_swd" \
  -f target/nrf52.cfg \
  -c "adapter speed 1000; init; reset halt; program .zmk/build/thinkpad_t430_reva_bootloader/thinkpad_t430_reva_bootloader_nosd.hex verify; program .zmk/build/thinkpad_t430_reva/zephyr/zmk.hex verify reset exit"
```

After that one-time installation, release iteration is fully software-driven:

```sh
tools/flash-t430-reva
```

The script rebuilds ZMK, opens its USB CDC interface at 1200 baud to request a
bootloader reboot, waits for `/Volumes/T430BOOT`, and copies `zmk.uf2`. No
button or Tag-Connect is needed. Copying to `T430BOOT` is the intended Rev A
UF2 update path and cannot overwrite the settings partition; the prohibition
on ordinary copies to `/Volumes/JLINK` still applies to the DK.

For the two staged smoke images, replace the final HEX path with
`.zmk/build/holyiot_18010_smoke/zephyr/zephyr.hex` or
`.zmk/build/holyiot_18010_usb_smoke/zephyr/zephyr.hex`. Confirm RTT output
after the first image and USB enumeration after the second before flashing the
complete keyboard image.

Never add `mass_erase` or `recover` to an ordinary update. If access protection
prevents connection, stop and obtain explicit approval before erasing settings
and BLE bonds.
