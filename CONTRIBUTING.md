# Contributing to MiniOS

MiniOS is intentionally small and educational. Changes should keep process
management and CPU scheduling easy to inspect rather than adding unrelated OS
features.

## Before opening a change

1. Build the freestanding kernel with `make clean && make check`.
2. Run the real-QEMU integration suite with `make test`.
3. Refresh the browser image with `make browser-image`.
4. Run `npm test` from `browser/`.
5. Update the relevant milestone or architecture documentation.

Kernel code must remain freestanding and warning-free under the flags in the
Makefile. New scheduling behavior should include a deterministic integration
case in `kernel/selftest.c`.

## Scope

Good contributions include scheduler policies, clearer metrics, deterministic
workloads, small hardware abstractions, tests, and educational documentation.
Networking, complex filesystems, multiprocessing, and production security
frameworks are deliberately outside the current project scope.
