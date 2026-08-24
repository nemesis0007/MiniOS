# MiniOS Browser Lab

This interface boots the real `minios.iso` in the official v86 x86 emulator.
The dashboard does not contain scheduler implementations.

## Data flow

```text
Dashboard command -> v86 PS/2 keyboard -> MiniOS shell -> kernel scheduler
Kernel snapshot -> COM1 UART -> v86 serial event -> dashboard visualization
```

MiniOS publishes bounded snapshot records after boot and every command. The
client replaces its process table, timeline, scheduler status, and metrics only
after a complete `BEGIN`/`END` snapshot arrives.

## Run locally

Build MiniOS in the repository root and copy the current image:

```sh
make
make browser-image
```

Then run the site:

```sh
cd browser
npm install
npm run dev
```

Use `npm test` for a production build, server-rendered page validation, and a
headless v86 boot that waits for telemetry from the packaged MiniOS kernel.

## Included emulator assets

- `public/images/minios.iso` — the bootable MiniOS image
- `public/v86/v86.wasm` — official v86 WebAssembly runtime
- `public/v86/seabios.bin` and `vgabios.bin` — BIOS images used by v86

The v86 project and source links are documented in
`public/v86/THIRD_PARTY_NOTICES.md`.
