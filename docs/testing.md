# Testing MiniOS

`make test` boots the real ISO in QEMU, types `selftest` through the emulated
PS/2 keyboard, and checks the summary received from the kernel over COM1.

The 13 kernel integration cases cover:

- PCB creation, admission, termination, and state changes
- zero-process and one-process scheduling
- FCFS, SJF, priority, and Round Robin ordering
- simultaneous arrivals, equal bursts, and equal priorities
- Round Robin rotation and configured quantum
- quantum larger than a process burst
- real context-switch counts
- waiting, turnaround, and response calculations
- scheduler selection and multiple simultaneous processes
- a real I/O workload entering WAITING, waking, and creating CPU idle time

These tests exercise the freestanding kernel and actual context switch path,
not a host-side scheduler model.
