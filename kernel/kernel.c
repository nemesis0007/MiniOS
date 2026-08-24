#include "interrupt.h"
#include "kernel.h"
#include "process.h"
#include "scheduler.h"
#include "serial.h"
#include "shell.h"
#include "terminal.h"
#include "timer.h"

void kernel_main(unsigned int multiboot_magic, unsigned int multiboot_info)
{
    (void)multiboot_magic;
    (void)multiboot_info;

    terminal_initialize();
    serial_initialize();
    process_initialize();
    scheduler_initialize();
    interrupt_initialize();
    timer_initialize(100);
    interrupt_enable();
    shell_run();
}
