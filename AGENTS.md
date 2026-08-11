# ThinkPad firmware requirements

Before changing USB/BLE profile selection, pairing, or profile LED behavior,
read and follow [`docs/transport-profile-contract.md`](docs/transport-profile-contract.md).

The contract is authoritative. Do not infer user intent from ZMK's currently
usable endpoint: USB may be the effective fallback while BLE remains the
explicitly requested transport. Transport commands and LED indications must be
driven by the shared ThinkPad profile controller, not by physical key positions
or endpoint-availability events.

Changes to this behavior must build for both `thinkpad_t430` and
`thinkpad_t470`. Test every transition listed in the contract before describing
the behavior as complete.

## Flashing without erasing BLE bonds

Normal development flashes must preserve the nRF52840 settings partition and
its BLE bonds. Read and follow the flashing procedure in
[`docs/flashing.md`](docs/flashing.md).

- By default, flash with J-Link Commander's `loadfile` command.
- Never copy a HEX file to `/Volumes/JLINK` for an ordinary firmware update.
  The DK's mass-storage programming path erases the settings partition, so the
  user must enter BLE security codes and pair every host again.
- Copy to `/Volumes/JLINK` only when the user explicitly requests a clean flash
  or settings/bond erasure and acknowledges that all BLE profiles will be lost.
- Do not claim that bonds were preserved merely because the HEX file does not
  overlap the settings partition; the programming method determines whether a
  chip erase occurs.
