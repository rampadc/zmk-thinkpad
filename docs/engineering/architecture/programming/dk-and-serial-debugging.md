---
title: DK programming & serial debugging
description: Bond-safe programming and serial logging on the nRF52840 DK.
---

# DK programming and serial debugging

## Flash without erasing bonds

Use J-Link Commander's `loadfile` command for normal development:

```text
JLinkExe -device NRF52840_XXAA -if SWD -speed 4000 -autoconnect 1
```

At the `J-Link>` prompt:

```text
loadfile /absolute/path/to/zmk.hex
r
g
exit
```

Typical images are:

```text
.zmk/build/thinkpad_t430/zephyr/zmk.hex
.zmk/build/thinkpad_t470/zephyr/zmk.hex
```

Tests on the T470 DK showed that `loadfile` left the 32 KiB settings partition
unchanged and preserved its BLE bond.

Do not copy a normal update to `/Volumes/JLINK`. The DK mass-storage programmer
erases settings even when the HEX file does not overlap that partition. Use it
only for an intentional clean flash:

```sh
cp path/to/zmk.hex /Volumes/JLINK/zmk.hex
sync
```

A clean flash removes BLE profiles and requires pairing each host again.

## Inspect settings

DK settings occupy `0xF8000–0xFFFFF`. To check suspected data loss, halt the
target and save the partition before and after flashing:

```text
savebin tmp/thinkpad-settings-before.bin 0xF8000 0x8000
savebin tmp/thinkpad-settings-after.bin 0xF8000 0x8000
```

Compare the files with `cmp` or SHA-256. Run `r` and `g` after diagnostic reads.

## Read serial logs

UART0 logs use the DK interface USB serial port at 115200 baud. On macOS:

```sh
brew install tio
find /dev -maxdepth 1 -name 'cu.usbmodem*' -print
tio -b 115200 /dev/cu.usbmodemXXXX
```

Replace the example device with the path reported by `find`. Connect the DK's
second USB port when testing USB keyboard and pointer output.

Custom Revision A boards use a different process. See
[Revision A programming](revision-a-programming.md).
