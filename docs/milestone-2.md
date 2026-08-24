# Milestone 2 — Terminal output

## Concept

MiniOS cannot call a host operating system's printing functions. In VGA text mode, the display is represented by 2,000 character cells beginning at physical address `0xB8000`. Each cell contains a character and a color byte. The terminal module owns the cursor position and translates text operations into writes to those cells.

## Files changed

- `kernel/terminal.c` implements clearing, characters, strings, lines, unsigned integers, wrapping, and scrolling.
- `kernel/terminal.h` exposes the small terminal interface.
- `kernel/kernel.c` initializes the terminal and exercises each important output operation.
- `Makefile` compiles and links the terminal module.
- `grub.cfg` labels the boot menu as MiniOS v0.2.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

## Expected output

```text
================================
          MiniOS v0.2
================================

Kernel initialized.
CPU initialized.
Memory initialized.
Terminal columns: 80

Milestone 2: terminal output ready.
```

## Important implementation details

- `terminal_putchar` is the single character-output primitive. It handles newlines, carriage returns, line wrapping, and scrolling.
- `terminal_write` repeatedly calls that primitive, so all string output inherits the same cursor behavior.
- `terminal_write_uint` converts a number without `printf`, division helpers, or a standard library.
- When row 25 is reached, `terminal_scroll` shifts the visible rows upward and clears the last row.

## Common errors

- A blank display usually means the kernel was not entered or VGA memory was addressed incorrectly. First run `make check` to separate boot-header errors from terminal errors.
- Garbled colors usually indicate the character and color bytes were reversed.
- Text overwriting one line indicates that the row or column was not advanced after wrapping.

## Confirmation checklist

- [ ] Clean build succeeds with warnings treated as errors.
- [ ] GRUB recognizes the kernel as Multiboot.
- [ ] QEMU displays every expected line.
- [ ] The integer `80` is rendered by the kernel, not hard-coded into the surrounding string.
- [ ] No keyboard, shell, process, or scheduling feature has been introduced yet.
