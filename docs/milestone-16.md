# Milestone 16 — Execution timeline

## Concept

Every timer tick records which PID actually owned the CPU. Consecutive ticks for the same PID are coalesced into one event, giving a compact Gantt representation without losing boundaries where ownership changed.

The fixed event buffer holds 128 segments and reports overflow explicitly. With the built-in workloads this is sufficient even for small Round Robin quanta.

## Command

```text
timeline
```

Example:

```text
Timeline (Round Robin):
0-2:P1 | 2-4:P2 | 4-6:P3 | 6-8:P1 | 8-10:P2
```

These events originate in the kernel's PIT scheduling path. The display layer does not recalculate the algorithm.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

## Confirmation checklist

- [ ] Every busy scheduler tick is assigned to one PID.
- [ ] Adjacent ticks for the same PID coalesce.
- [ ] Preemptions create visible event boundaries.
- [ ] FCFS/SJF/priority produce one segment per completed process.
- [ ] Overflow is bounded and reported.
