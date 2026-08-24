# Milestone 8 — Timer and interrupts

## Concept

An interrupt lets hardware temporarily transfer control to the kernel. MiniOS now owns an Interrupt Descriptor Table (IDT), remaps the legacy Programmable Interrupt Controller (PIC) away from CPU exception vectors, and programs the Programmable Interval Timer (PIT) to generate IRQ0 at 100 Hz.

On each tick:

```text
PIT raises IRQ0
  -> CPU uses IDT vector 32
  -> assembly saves general registers
  -> C timer handler increments the kernel tick counter
  -> PIC receives end-of-interrupt
  -> assembly restores registers and executes iret
```

Only IRQ0 is unmasked during this milestone. Keyboard input continues polling, which keeps the change focused and testable.

## Assembly

`kernel/interrupt.s` contains the IRQ0 entry stub. `pusha` preserves the interrupted code's general registers, `call` enters the C handler, `popa` restores the exact values, and `iret` atomically restores the saved instruction pointer, code segment, and flags.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

Wait briefly, then run:

```text
MiniOS> ticks
Timer ticks: 1234 at 100 Hz.
```

The exact count varies, but it must increase between commands while the shell remains responsive.

## Common errors

- An immediate reboot often indicates a missing IDT gate or invalid code selector.
- A frozen timer after one tick usually means the PIC did not receive EOI.
- QEMU's `-no-reboot -no-shutdown` options keep interrupt failures visible.

## Confirmation checklist

- [ ] IDT loads and PIC IRQs are remapped to vectors 32–47.
- [ ] Only IRQ0 is unmasked.
- [ ] PIT runs at the configured 100 Hz.
- [ ] Tick count increases while the shell polls for input.
- [ ] Registers are saved and restored around the handler.
- [ ] QEMU does not reboot or freeze.
