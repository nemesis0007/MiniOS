# Milestone 12 — Shortest Job First

## Concept

Non-preemptive SJF selects the READY process with the smallest original burst estimate. Equal bursts are broken by PID. Once selected, a process keeps the CPU until its timer-owned burst reaches zero and its actual C entry point returns.

Shell-created workloads share arrival time zero so each algorithm sees the same initial ready queue. Arrival-aware selection still exists in the scheduler and can be exercised directly by kernel tests.

## Example

```text
run long       # burst 24, PID 1
run short      # burst 3,  PID 2
run counter    # burst 8,  PID 3
scheduler sjf
start
stats
```

Expected order by start time:

```text
short -> counter -> long
```

## Difference from existing algorithms

- FCFS uses arrival and PID order and does not preempt.
- SJF uses the smallest known burst and does not preempt.
- Round Robin uses cyclic order and preempts on timer quantum expiry.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

## Confirmation checklist

- [ ] Smallest READY burst is selected first.
- [ ] Equal bursts are resolved by PID.
- [ ] Selected processes run non-preemptively.
- [ ] Timing records reflect SJF order.
- [ ] Real process stacks and timer-owned bursts remain in use.
