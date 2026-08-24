# Milestone 13 — Priority scheduling

## Convention

MiniOS uses a lower numeric value to represent a higher priority. Priority 1 runs before priority 2, which runs before priority 3. Equal priorities are resolved by PID so results remain deterministic.

Priority scheduling is initially non-preemptive: a higher-priority process created after another process starts does not interrupt it.

## Example

```text
run long       # priority 4
run counter    # priority 3
run short      # priority 1
scheduler priority
start
stats
```

Expected order:

```text
short -> counter -> long
```

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

## Tradeoff

Priority scheduling lets important work run first, but a steady supply of high-priority work can starve low-priority processes. Aging is a possible future extension but is intentionally outside the current educational scope.

## Confirmation checklist

- [ ] Lowest priority number is selected first.
- [ ] Equal priorities are resolved by PID.
- [ ] A selected process runs to completion.
- [ ] Timing records reflect priority order.
- [ ] The priority convention is visible in documentation.
