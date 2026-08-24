# Milestone 1 — Bootable MiniOS

## Concept in plain language

When an x86 PC starts, it needs a small program to find and load an operating system. GRUB performs that job for MiniOS. The special three-word Multiboot header in `boot/boot.s` tells GRUB that `minios.bin` is a kernel it understands. GRUB loads it, prepares a 32-bit CPU environment, and jumps to `_start`.

The assembly entry point selects a known-good 16 KiB stack and calls C code. The C kernel writes character-and-color pairs directly into VGA text memory at address `0xB8000`. This is genuine hardware-facing kernel output: there is no `printf`, terminal library, Windows API, or Linux system call beneath it.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

Expected display:

```text
================================
          MiniOS v0.1
================================

Welcome to MiniOS!
Kernel boot successful.
```

Close the QEMU window to stop the VM.

## Important code

### Multiboot header

GRUB searches near the beginning of the kernel for the magic value `0x1BADB002`. The flags request page-aligned loading and basic memory information. The checksum makes the three 32-bit values sum to zero, which GRUB validates.

### Entry point and stack

`_start` is the entry selected by `linker.ld`. It loads `esp` with the top of a statically reserved stack, passes GRUB's Multiboot values to `kernel_main` for future use, and calls the C kernel. If C ever returns, the assembly loop disables interrupts and halts safely.

### VGA output

Each VGA text cell occupies two bytes: one character byte and one color byte. `kernel/kernel.c` builds those pairs and stores them at `0xB8000`. Rows are 80 cells wide, so a row/column coordinate maps directly to a memory offset.

### Linker script

`linker.ld` places the image at 1 MiB, a conventional safe kernel load address. `KEEP(*(.multiboot))` prevents the header from being discarded and places it before the ordinary code.

## Common errors and fixes

### The compiler rejects `-m32`

Install GCC's 32-bit x86 support or use an `i686-elf` cross-compiler with `make CROSS=i686-elf-`. Do not silently substitute a Windows compiler. If a compatible cross-compiler uses a different prefix, pass it explicitly, for example `make CROSS=i386-elf-`.

### `grub-mkrescue: command not found`

Install the GRUB utilities and `xorriso`. On Debian/Ubuntu-like systems the relevant packages are commonly named `grub-pc-bin`, `grub-common`, and `xorriso`.

### `qemu-system-i386: command not found`

Install the QEMU x86 system emulator package. The exact package name varies by distribution.

### GRUB says there is no Multiboot header

Run `make clean && make check`. If this fails, inspect the compiler target and linker output; the build must remain 32-bit ELF, and the `.multiboot` section must stay near the beginning of the file.

### QEMU shows a blank screen or reboots

Run QEMU with `-no-reboot -no-shutdown` to keep a CPU failure visible. Rebuild from clean objects and confirm `make check` succeeds before debugging the C code.

## Confirmation before Milestone 2

- [ ] The kernel builds without warnings or errors.
- [ ] GRUB validates the image as Multiboot.
- [ ] QEMU boots the generated ISO.
- [ ] All six expected text lines and blank lines are positioned correctly.
- [ ] The system stays running without rebooting or faulting.
- [ ] No process, scheduler, shell, filesystem, or browser code has been added.

Proceed to Milestone 2 only after every item is confirmed.
