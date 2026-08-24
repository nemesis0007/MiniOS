# Milestone 17 — Experimental comparison

## Fixed workloads

```text
Workload A: counter, fibonacci, prime
Workload B: short, long, io_test, fibonacci
```

`benchmark <a|b>` resets kernel process/scheduler state and executes the selected workload four times: FCFS, SJF, priority, then Round Robin with quantum 2. Every run creates fresh PCBs with identical program metadata and shared arrival time zero.

## Output

The kernel retains average waiting, turnaround, and response time plus context switches for each run. The shell prints a comparison table and an ASCII average-waiting graph. The browser can later consume the same kernel-owned records and timeline rather than implementing algorithms itself.

## Commands

```text
benchmark a
benchmark b
```

The benchmark intentionally replaces the current process table. The final visible table and timeline belong to the Round Robin run.

## Interpretation

- FCFS is simple and low-overhead but can make short work wait behind long work.
- SJF often reduces average waiting when bursts are known, but long jobs can starve.
- Priority runs important work first but can starve low-priority jobs.
- Round Robin improves responsiveness and fairness at the cost of more context switches.

No algorithm is universally best; results depend on the workload and goal.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

## Confirmation checklist

- [ ] Every algorithm receives fresh identical PCBs.
- [ ] Real program entry points execute for every run.
- [ ] Round Robin uses hardware IRQ preemption.
- [ ] Results and bars come from kernel statistics.
- [ ] Both fixed workloads complete reproducibly.
- [ ] Documentation explains tradeoffs without a universal-winner claim.
