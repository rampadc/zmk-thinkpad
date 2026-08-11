# USB, BLE profile, pairing, and LED contract

This document is the source of truth for transport and profile behavior on all
ThinkPad keyboards in this repository.

## Concepts that must remain separate

- **Requested transport** is the transport explicitly selected by the user.
- **Active BLE profile** is the selected BLE slot, numbered 1 through 3.
- **Usable endpoint** is the endpoint to which ZMK can currently send reports.
  ZMK may temporarily use USB when the requested BLE profile is not connected.
- **Bond state** records whether a BLE profile contains host keys.
- **LED state** communicates the requested action and profile state. It must not
  be inferred solely from the usable endpoint.

USB becoming available must never change the requested transport. Explicitly
selecting USB is exclusive: the BLE host stack is suspended, existing BLE
connections are disconnected, and the keyboard must not advertise or reconnect
until the user selects `Fn+1`, `Fn+2`, or `Fn+3`.

## Commands

| Input | Command | Required result |
| --- | --- | --- |
| Tap `Fn+1` | Select BLE profile 1 | Request BLE, select slot 1, preserve its bond |
| Tap `Fn+2` | Select BLE profile 2 | Request BLE, select slot 2, preserve its bond |
| Tap `Fn+3` | Select BLE profile 3 | Request BLE, select slot 3, preserve its bond |
| Hold `Fn+1` for 3 seconds | Pair BLE profile 1 | Request BLE, select slot 1, clear only slot 1, advertise for authenticated pairing |
| Hold `Fn+2` for 3 seconds | Pair BLE profile 2 | Request BLE, select slot 2, clear only slot 2, advertise for authenticated pairing |
| Hold `Fn+3` for 3 seconds | Pair BLE profile 3 | Request BLE, select slot 3, clear only slot 3, advertise for authenticated pairing |
| Tap `Fn+4` | Select USB | Request USB, suspend BLE connectivity, and preserve every BLE bond and profile |
| Plug or unplug USB | No command | Do not change requested transport or active BLE profile |

An empty BLE profile automatically advertises after selection. It does not
require a destructive long hold. A long hold exists to replace an occupied
profile deliberately.

## Pairing security

BLE pairing requires keyboard-side passkey entry. The host displays six digits;
the user types them on the ThinkPad keyboard and presses Enter. Escape cancels.
No profile may be silently overwritten by a new host.

## Profile LEDs

| State or transition | Required indication |
| --- | --- |
| Bonded BLE profile selected | Only that profile LED is solid for 5 seconds |
| Empty profile selected | Only that profile LED blinks rapidly until pairing completes or another command is selected |
| Pair command | Only that profile LED blinks rapidly until pairing completes or another command is selected |
| Bonded profile disconnected while BLE is requested | Only that profile LED gives the slow reconnect indication |
| BLE profile connected | The selected profile LED is solid for 5 seconds, then turns off |
| USB selected with `Fn+4` | All three profile LEDs are solid for 1 second, then turn off |
| USB merely plugged or unplugged | No profile LED indication and no transport change |

The three-LED USB indication is tied to the explicit USB command, never to USB
enumeration, charging, endpoint fallback, boot, suspend, or resume.

The host showing a BLE device as connected is not acceptable in USB mode. USB
selection must disconnect BLE at the radio/host-stack level, not merely route
HID reports to USB. Selecting any BLE profile resumes Bluetooth before
advertising or reconnecting that profile.

## Persistence

Requested transport, active BLE profile, host addresses, and negotiated keys
must survive reset and normal firmware flashing. They are stored in the
nRF52840 internal-flash settings partition; no external SPI flash is required.
Full-chip erase, recover, settings-reset firmware, or a changed storage
partition may erase them.

Normal flashing must use the bond-preserving J-Link `loadfile` procedure in
[`flashing.md`](flashing.md). Copying firmware to `/Volumes/JLINK` is a
destructive clean-flash operation on the development DK and must not be used for
routine updates.

## Implementation constraints

- `Fn+1/2/3/4` must invoke the shared ThinkPad profile controller.
- LED drivers consume explicit controller commands and BLE state.
- LED drivers must not observe matrix positions to reconstruct commands.
- `zmk_endpoints_selected()` is the usable endpoint, not necessarily requested
  transport, and must not be used as the sole source for requested intent.
- Endpoint selection alone is insufficient for USB mode. The shared controller
  must suspend BLE on `Fn+4` and resume it only on `Fn+1/2/3`.
- Both T430 and T470 must implement the same command semantics.

## Required regression transitions

Test at least:

1. BLE connected, then USB plug and unplug: BLE remains requested.
2. USB requested, then tap each bonded BLE profile: requested profile connects.
3. USB requested, then select an empty profile: only its LED rapidly blinks.
4. Hold each profile command: only that bond is cleared and its LED rapidly blinks.
5. Tap `Fn+4`: all three LEDs illuminate for one second exactly once.
6. After `Fn+4`, every BLE host disconnects and the keyboard remains absent
   from BLE connections until a BLE profile is explicitly selected.
7. USB suspend/resume/reconnect without `Fn+4`: no three-LED indication and no
   change to BLE radio state.
8. Select a bonded BLE profile from USB mode: Bluetooth resumes and that profile
   reconnects without re-pairing.
9. Complete passkey pairing: selected LED is solid for five seconds, then off.
10. Reflash with J-Link `loadfile`: bonds and requested selection remain stored.
11. Use `/Volumes/JLINK` only for an intentional clean flash and verify that the
   user expects BLE settings to be erased.
