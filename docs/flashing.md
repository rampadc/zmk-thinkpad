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
