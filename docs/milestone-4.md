# Milestone 4 — Extensible shell

## Concept

The shell now separates a command line into a command and arguments. It validates numeric input without a C library and dispatches through a small, readable command table expressed as explicit branches. The parser accepts repeated spaces and rejects unexpected extra arguments.

Process-related commands are visible but deliberately report that process management is not available yet. This proves the user interface without pretending processes exist.

## Commands

```text
help
clear
about
ps
run <program>
kill <pid>
scheduler <fcfs|sjf|priority|rr>
quantum <1-100>
start
stop
stats
```

At this stage, `scheduler` and `quantum` store validated configuration only. They do not run scheduling algorithms.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

Try:

```text
MiniOS> scheduler rr
Scheduler selected: rr
It will become active after process scheduling is added.

MiniOS> quantum 2
Time quantum set to 2 ticks.
```

## Testing

- Test all scheduler names and an unknown name.
- Test quantum values `1`, `100`, `0`, `101`, and non-numeric input.
- Add leading or repeated spaces to commands.
- Confirm process commands explain that Milestone 5 is required.
- Confirm a long input line is safely capped at the input-buffer size.

## Confirmation checklist

- [ ] Clean build, Multiboot validation, and QEMU boot pass.
- [ ] Commands and arguments are separated correctly.
- [ ] Scheduler names and quantum ranges are validated.
- [ ] All eventual shell commands appear in `help`.
- [ ] No fake process or scheduling behavior is present.
