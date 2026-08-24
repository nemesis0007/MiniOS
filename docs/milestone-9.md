# Milestone 9 — Context switching

## Concept

MiniOS can now suspend the shell's kernel stack, load a process's private stack, execute the process entry point, and restore the exact shell stack afterward. This is genuine CPU context switching rather than calling a workload as an ordinary shell function.

## Stack contract

`context_switch.s` saves flags and the x86 callee-saved registers, stores the old `esp`, loads a new `esp`, restores the new context, and returns into that context. A new process stack is prebuilt to match that layout:

```text
EDI ESI EBX EBP EFLAGS entry-point exit-trampoline
```

The first `ret` enters the program. When the program's C function returns, it lands in `dispatcher_process_exit`, which marks the PCB terminated and switches back to the suspended shell.

Timer interrupts remain active on either stack and preserve the interrupted general registers independently.

## Build and run

```sh
make clean
make
make check
qemu-system-i386 -cdrom build/minios.iso
```

Example:

```text
MiniOS> run short
Process created.
PID: 1

MiniOS> start
PID 1 completed; shell context restored.

MiniOS> ps
1      short          TERMINATED   1
```

One READY process is dispatched per `start` command. Algorithm selection and automatic multi-process scheduling begin in Milestone 10.

## Assembly explanation

- `pushfl` preserves interrupt state and arithmetic flags.
- `push` of EBP, EBX, ESI, and EDI satisfies the 32-bit C ABI's callee-saved contract.
- The current ESP is stored through the first argument.
- ESP is replaced with the second argument.
- The symmetric pops restore the selected context.
- `ret` resumes either a new process entry point or the suspended dispatcher call.

## Confirmation checklist

- [ ] Each process starts on its own aligned PCB-owned stack.
- [ ] The shell stack is not reused by the workload.
- [ ] A returning workload reaches the exit trampoline.
- [ ] PCB state changes `READY -> RUNNING -> TERMINATED`.
- [ ] Shell execution resumes after the context switch.
- [ ] Timer IRQs remain safe during process execution.
