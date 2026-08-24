# Milestone 7 — Process creation command

## Concept

The program registry and process manager are now connected. `run <program>` resolves a kernel-native program descriptor, allocates its PCB and private stack, copies scheduling metadata, and admits it from `NEW` to `READY`. It does not execute the entry point yet.

Each successful command receives a monotonically increasing arrival value and unique PID. Invalid program names and a full process table fail without corrupting existing PCBs.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

Example:

```text
MiniOS> run counter
Process created.
PID: 1

MiniOS> run fibonacci
Process created.
PID: 2

MiniOS> ps
PID    PROGRAM         STATE       PRIORITY
------------------------------------------
1      counter          READY       3
2      fibonacci          READY       2
```

The spacing is intentionally simple VGA text output; the PCB values are the authoritative content.

## Testing

- Create each of the six built-ins.
- Confirm PIDs are unique and states are `READY`.
- Kill a PID and confirm `ps` reports `TERMINATED`.
- Try an unknown name and `run` without an argument.
- Fill all 16 slots and confirm the seventeenth creation fails safely.

## Confirmation checklist

- [ ] `run` validates program names through the registry.
- [ ] Successful creation prints a unique PID.
- [ ] `ps` displays matching PCB names, states, and priorities.
- [ ] `kill` terminates the selected PCB.
- [ ] No program entry point runs before timer/context-switch milestones.
