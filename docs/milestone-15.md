# Milestone 15 — Statistics

## Metrics

For every completed process MiniOS records arrival, burst, first start, completion, waiting, turnaround, and response time. The system statistics module aggregates:

- average waiting, turnaround, and response time
- CPU utilization
- throughput in completed processes per scheduler tick
- dispatch/context-switch count

All arithmetic is integer-only. Values ending in hundredths are rendered with two decimal places without floating-point instructions or a C library.

## Definitions

```text
turnaround = completion - arrival
waiting    = turnaround - CPU burst - blocked time
response   = first start - arrival
utilization = busy ticks / elapsed scheduler ticks
throughput  = completed processes / elapsed scheduler ticks
```

## Command

```text
stats
```

The command prints the per-process table followed by system averages, utilization, throughput, context switches, and the Round Robin quantum when relevant.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

## Confirmation checklist

- [ ] Zero completed processes does not divide by zero.
- [ ] A single process has zero waiting and response time.
- [ ] Averages include completed processes only.
- [ ] CPU utilization distinguishes busy and idle logical ticks.
- [ ] Context switches use the same definition for every algorithm.
- [ ] No floating-point kernel state is required.
