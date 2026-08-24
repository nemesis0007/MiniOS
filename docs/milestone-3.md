# Milestone 3 — Keyboard input

## Concept

The legacy PS/2 controller exposes keyboard data through x86 I/O ports. MiniOS checks status port `0x64`; when a byte is ready, it reads a Set 1 scan code from port `0x60`. The keyboard module translates press events into characters and ignores key-release events. Shift, Enter, Backspace, letters, numbers, spaces, and common punctuation are supported.

This milestone deliberately polls the controller. Hardware interrupts arrive in Milestone 8, where input can become event-driven.

## Files changed

- `kernel/io.h` contains the two low-level port I/O instructions.
- `kernel/keyboard.c` and `.h` translate PS/2 scan codes.
- `user/shell.c` and `.h` provide line editing and the first three commands.
- `kernel/terminal.c` now supports destructive backspace.
- `kernel/kernel.c` starts the shell after terminal initialization.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

Try:

```text
MiniOS> help
MiniOS> about
MiniOS> clear
```

Expected `help` output:

```text
Available commands:
help       Show available commands
clear      Clear screen
about      Show MiniOS information
```

## Testing

1. Type ordinary letters and verify each appears once.
2. Hold Shift for an uppercase letter.
3. Type a mistake and use Backspace before Enter.
4. Run all three commands.
5. Enter an unknown command and confirm the shell reports it without hanging.

## Common errors

- Repeated or missing characters usually mean release codes were not filtered with bit 7.
- No input usually means status port `0x64` is not being checked before reading port `0x60`.
- QEMU must have keyboard focus when testing interactively.

## Confirmation checklist

- [ ] Clean build and Multiboot validation pass.
- [ ] The prompt accepts keyboard input.
- [ ] Enter submits a command and Backspace edits it.
- [ ] `help`, `clear`, and `about` produce the documented output.
- [ ] Unknown input is handled safely.
- [ ] No process or scheduler implementation has been added yet.
