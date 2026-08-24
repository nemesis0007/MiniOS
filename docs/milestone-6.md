# Milestone 6 — Deterministic test programs

## Concept

MiniOS now contains six programs compiled directly into its kernel image. They use only C and volatile result locations, so the compiler cannot erase their work. No host library or executable format is involved. A registry maps shell-visible names to entry addresses, estimated bursts, and default priorities.

## Programs

- `counter` advances deterministically to 200,000.
- `fibonacci` performs 250,000 unsigned Fibonacci iterations.
- `prime` finds primes through 12,000.
- `short` performs a small deterministic arithmetic workload.
- `long` performs a substantially larger CPU-bound workload.
- `io_test` performs five distinct work phases. Timer-backed waits are connected after the hardware timer exists in Milestone 8.

Unsigned overflow is intentional and deterministic for the arithmetic workloads.

## Registry

Each descriptor contains:

```text
name, entry point, estimated burst, default priority
```

The boot-time registry self-test checks all six entries, their metadata, reverse lookup, and unknown-program rejection without executing the workloads early.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

Expected additional boot line:

```text
Program registry self-test passed (6 workloads).
```

`about` lists the exact accepted names. `run counter` validates the name but waits for Milestone 7 to allocate its PCB.

## Confirmation checklist

- [ ] All six program sources compile into the kernel image.
- [ ] Registry self-test passes in QEMU.
- [ ] Program names, entry points, bursts, and priorities are non-empty.
- [ ] Short and long workloads differ materially in work performed.
- [ ] Unknown names are rejected.
- [ ] No workload is executed before a PCB is created.
