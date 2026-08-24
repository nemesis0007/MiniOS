# Milestone 10 — First Come First Serve

## Concept

FCFS selects the READY process with the earliest arrival value. Equal arrivals are broken by PID, making every run deterministic. Each selected process runs on its real private stack until its C entry point finishes; FCFS never preempts it.

The scheduler owns a logical clock measured in workload ticks. This makes repeatable comparisons possible even when the emulator host runs at a different speed. Hardware PIT ticks remain available separately through `ticks`.

## Recorded fields

- arrival and estimated burst
- first start and completion
- waiting = start - arrival
- turnaround = completion - arrival
- response = first start - arrival

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

Example:

```text
run long
run short
scheduler fcfs
start
stats
```

Even though `short` has a smaller burst, `long` completes first because it arrived first.

## Confirmation checklist

- [ ] READY processes are selected by arrival, then PID.
- [ ] Each process runs until completion without scheduler preemption.
- [ ] All six per-process timing fields are recorded.
- [ ] `start` continues until no READY process remains.
- [ ] `stats` displays the kernel's deterministic timing records.
- [ ] The shell regains its saved context after the batch.
