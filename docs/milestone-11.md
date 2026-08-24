# Milestone 11 — Preemptive Round Robin

## Concept

Round Robin gives each READY process up to a configurable number of PIT ticks. IRQ0 decrements only the process that actually owns the CPU. When the quantum expires, the interrupt handler selects the next READY PCB and switches stacks before returning from the interrupt. The interrupted process later resumes inside its suspended timer handler and `iret` restores the exact interrupted instruction.

This is hardware-timer preemption. The shell and JavaScript do not calculate the rotation.

## Workload duration

Each program continues deterministic calculations while its PCB has remaining CPU ticks. Only ticks received while RUNNING reduce that value. Consequently, a burst of 8 means eight actual timer-owned CPU intervals, even if the host computer is faster or slower.

## Commands

```text
scheduler rr
quantum 2
run counter
run fibonacci
run prime
start
stats
```

The scheduler rotates after two ticks until all three real C entry points return through their process exit trampolines.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

## Important context behavior

- IRQ0 sends PIC EOI before switching so the newly running process can receive future ticks.
- A preempted process retains the IRQ frame and the scheduler call frames on its own stack.
- Resuming that stack unwinds the C handler, restores all general registers, and executes `iret`.
- A process that completes selects the next RR process directly; only the final process restores the shell stack.

## Confirmation checklist

- [ ] Quantum is configurable from 1–100 ticks.
- [ ] IRQ0 decrements only the RUNNING process.
- [ ] Quantum expiry performs a real stack switch.
- [ ] READY processes rotate by table order.
- [ ] All programs eventually terminate and restore the shell.
- [ ] Context-switch count increases for rotations.
- [ ] Waiting, turnaround, and response times reflect RR interleaving.
