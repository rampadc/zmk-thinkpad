---
title: Companion app
description: Draft requirements for a local desktop application that manages ZMK-ThinkPad keyboards.
---

# Companion app specification

> **Status: proposal.** No companion app or management protocol exists yet.

The companion app manages ZMK-ThinkPad keyboards on Windows, macOS, and Linux.
It is not part of the first firmware or hardware release. This page records
the boundaries for later work; it is not an implementation guide.

## Goals

The app should:

- identify each connected controller;
- show firmware, connection, battery, and configuration state;
- update supported settings without reflashing;
- coordinate mute LEDs with host state;
- edit keymaps and bounded macros; and
- update firmware without erasing settings or Bluetooth bonds.

The app must work locally without an account. Normal keyboard and TrackPoint
input must not depend on the app.

## Related specifications

Transport and pairing commands must follow the
[transport profile contract](transport-profile-contract.md). In particular:

- USB availability does not change the requested transport;
- the active endpoint does not represent user intent;
- selecting USB suspends BLE;
- pairing is an explicit command; and
- normal updates preserve settings and bonds.

## Scope

### First milestone

The first useful milestone provides:

- USB device discovery;
- stable controller identity;
- read-only device and connection state;
- diagnostics export; and
- one safe writable setting, such as TrackPoint sensitivity.

This milestone proves the protocol and storage model before broader product
work begins.

### Later work

Later versions may add:

- BLE management, if platform prototypes prove reliable;
- keymap and layer editing;
- host and device macros;
- meeting-app mute integration;
- context profiles; and
- signed firmware updates.

Cloud sync, a plugin marketplace, arbitrary remote command execution, and
mobile apps are out of scope.

## Architecture

Use Tauri 2 with a Rust core and a web UI. Keep device discovery, protocol,
storage, platform integration, and UI code separate. The UI must not call HID,
audio, or operating-system input APIs directly.

Platform adapters must report their capabilities. The UI must hide or disable
unsupported features instead of emulating success.

Closing the window may leave monitoring active, but the user must have a clear
Quit command.

## Device protocol

### Transport

Use a vendor-defined HID collection over USB, separate from keyboard, consumer,
and pointer reports. Ordinary input continues to use standard HID.

BLE management remains optional until tested on all supported operating
systems. Do not assume that a USB design will work unchanged over BLE.

### Identity

Each controller stores a random, persistent UUID. A factory reset may replace
it; configuration import must not. The app must not use a USB path, BLE
address, model name, or VID/PID as the sole identity.

The user selects the physical keyboard model during setup. Firmware metadata
may suggest a model but must not silently choose one.

### Messages

Each message contains:

- protocol major and minor versions;
- command or event type;
- request ID;
- payload length; and
- payload.

Ignore unknown optional fields. If the protocol major version is unsupported,
allow safe reads and explain why writes are disabled.

The protocol needs a separate byte-level definition before implementation. It
must specify report IDs, field sizes, byte order, limits, errors, retries, and
transaction behavior.

### Required operations

The protocol should eventually support:

- reading identity, capabilities, firmware version, and state;
- reading and writing settings;
- requesting USB;
- selecting or pairing a BLE profile;
- reading and writing keymaps; and
- starting a firmware update.

Validate a write before committing it. An interrupted write must leave either
the old value or the complete new value, never a partial value.

Commands that change transport can disconnect the management channel. An
acknowledgement means the command was accepted, not that the final state was
observed.

### Authorization

Read-only queries may work without authorization. Configuration changes,
pairing, bond removal, factory reset, and firmware updates require physical
confirmation on the keyboard.

A device UUID is an identifier, not a credential. The protocol must prevent
replay and expose a firmware-controlled configuration lock.

## Configuration storage

Writable configuration must use a versioned, checksummed format with atomic
commit and recovery. It must have explicit limits for size, write frequency,
and schema migrations.

Every update test must verify that Bluetooth bonds, requested transport, the
selected profile, and the device UUID survive.

## Device management

The app shows configured and currently connected controllers. Each entry may
show:

- user-assigned name and keyboard model;
- connection transport and BLE profile;
- firmware and protocol versions;
- battery state;
- update availability; and
- warnings or unsupported features.

Do not infer the active keyboard from typing activity. Do not change transport
because USB appeared or disappeared.

## TrackPoint settings

Before exposing TrackPoint sensitivity, firmware must define the supported
range and verify the command on each hardware model.

The UI provides a labeled slider, numeric value, default marker, Apply, and
Restore default. A preview expires after 15 seconds unless applied. Firmware
must serialize setting writes with normal PS/2 traffic.

## Mute state

Do not model mute as one global Boolean. Track the source, target device or
application, value, confidence, and age.

Speaker mute follows the selected system output device. Microphone state may
come from the system capture device, a meeting application, or a hardware
privacy switch. When sources disagree, report `mixed`; when no fresh source is
available, report `unknown`.

The firmware owns LED priority. Transport and profile indications outrank host
mute updates. After a higher-priority indication ends, LEDs resume from the
latest host state.

An integration failure must not interrupt keyboard input or other integrations.

## Keymaps and automation

Use one versioned binding model for remaps, layers, macros, and host actions. A
binding contains a trigger, optional conditions, actions, execution location,
and concurrency policy.

Prefer on-keyboard execution for remaps, layers, tap/hold behavior, combos, and
short key sequences. Application and operating-system automation runs on the
host.

Automation must be bounded and interruptible. On cancel, disconnect, sleep, or
failure, release every key and button pressed by the macro.

Local command execution is disabled by default. If enabled, show the exact
command and working directory, use a restricted environment, bound output and
runtime, and require review after import.

Text macros must be disabled in detected password or secure-entry fields.
Where the platform cannot detect such fields, say so plainly.

## Data and privacy

Store configuration locally by default. Exported files must be versioned and
must not contain device credentials, bond keys, tokens, or the controller UUID.

Logs must redact typed text, macro contents, commands, paths containing user
names, tokens, and device credentials. Diagnostics require an explicit user
action and a preview before export.

## Reliability

Keyboard and TrackPoint input take priority over management work. The app must
recover from device reconnects, suspend and resume, stale state, crashed
integrations, and interrupted writes.

Targets for the first milestone:

- detect a USB connection within 2 seconds;
- show a state change within 500 ms of receiving it;
- add less than 1 percent average CPU load while idle; and
- stop retrying with exponential backoff after repeated device errors.

## Release gates

Do not claim support until the relevant gate passes:

| Gate | Evidence required |
| --- | --- |
| Platform | Prototype device discovery, permissions, packaging, and recovery on each supported OS. |
| BLE | Reliable discovery, authorization, reconnect, and coexistence with normal HID. |
| Storage | Atomic update, power-loss recovery, migration, and bond-preservation tests on real hardware. |
| Automation | Safe event capture, cancellation, stuck-key recovery, and secure-field behavior. |
| Mute | Documented APIs, permissions, stale-state behavior, and tests for each supported application. |
| Firmware update | Signed package validation, rollback or recovery, interrupted-update tests, and preserved settings. |
| TrackPoint | Verified ranges and write behavior on each advertised keyboard model. |

## Open decisions

- Vendor HID report layout and maximum payload size
- Configuration storage format and capacity
- BLE management transport
- Physical-confirmation gesture
- Firmware signing and recovery policy
- Supported operating-system versions
- Meeting applications included in the first release
