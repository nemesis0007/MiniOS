# Milestone 14 — Scheduler selection

## Clean interface

The process manager owns PCBs and lifecycle data. The dispatcher owns low-level stack movement. The scheduler owns only policy, logical time, metrics, and timer-driven decisions:

```text
Process Manager
      |
Scheduler policy: FCFS / SJF / Priority / Round Robin
      |
Dispatcher and context_switch.s
      |
CPU and PIT IRQ0
```

Changing the algorithm does not rewrite process creation or context switching.

## Commands

```text
scheduler              Show current selection
scheduler fcfs
scheduler sjf
scheduler priority
scheduler rr
quantum 3
```

The quantum applies to Round Robin and is retained when switching temporarily to another policy.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

## Confirmation checklist

- [ ] All four names resolve through one public scheduler API.
- [ ] The current selection is queryable.
- [ ] Policy changes do not mutate existing PCB metadata.
- [ ] Only Round Robin preempts on quantum expiry.
- [ ] The dispatcher contains no FCFS/SJF/priority policy.
