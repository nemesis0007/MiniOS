# Milestone 18 - Browser Telemetry Bridge

MiniOS now initializes the COM1 serial port and publishes structured snapshots
after boot and after each shell command. The records cover the active scheduler,
quantum, PCB table, execution timeline, and system statistics. The browser reads
these records from the emulated UART; it does not calculate scheduling results.

Record format:

```text
@MINIOS|BEGIN
@MINIOS|SCHEDULER|Round Robin|2
@MINIOS|PROCESS|1|counter|READY|3|0|8|8
@MINIOS|EVENT|1|0|2
@MINIOS|STATS|933|2133|933|3|10000|14
@MINIOS|END
```

Build and validate with `make clean && make && make check`. In QEMU, attach
COM1 to a file or terminal with `-serial file:build/telemetry.log`.
