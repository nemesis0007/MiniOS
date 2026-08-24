# Milestone 5 — Process management

## Concept

A process is a program the operating system is tracking for execution. MiniOS now has a fixed-size process table containing Process Control Blocks (PCBs). A PCB preserves the identity, lifecycle state, scheduling inputs, stack, saved CPU state, and timing fields belonging to one process.

The supported lifecycle states are `NEW`, `READY`, `RUNNING`, `WAITING`, and `TERMINATED`. `UNUSED` is an internal marker for an empty table entry.

## PCB contents

- PID and bounded process name
- lifecycle state and priority
- arrival, burst, and remaining time
- program counter and private 4 KiB stack information
- saved general registers, stack pointer, instruction pointer, and flags
- start, completion, waiting, turnaround, and response statistics

## Behavior added

- `process_create` validates inputs and allocates a unique PID and stack.
- `process_admit` performs the `NEW -> READY` transition.
- `process_terminate` transitions any live process to `TERMINATED`.
- `process_find`, `process_at`, and `process_slot_count` provide bounded lookup.
- A boot-time self-test creates, admits, validates, terminates, and clears a PCB.
- `ps` prints the process table and `kill <pid>` performs termination.

Test programs are intentionally not present yet, so the visible table is empty after the self-test cleans up.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

Then run:

```text
MiniOS> ps
PID    PROGRAM         STATE       PRIORITY
------------------------------------------
(no processes)
```

## Confirmation checklist

- [ ] Kernel reports `Process manager self-test passed.`
- [ ] Every required PCB field exists.
- [ ] Invalid names, zero bursts, missing PIDs, and table overflow fail safely.
- [ ] State transitions `NEW -> READY -> TERMINATED` are validated.
- [ ] Each process receives an independent aligned stack.
- [ ] `ps` is safe with zero processes.
- [ ] No program execution or scheduler behavior is claimed yet.
