CROSS ?=
CC := $(CROSS)gcc
GRUB_FILE ?= grub-file
GRUB_MKRESCUE ?= grub-mkrescue
QEMU ?= qemu-system-i386

CFLAGS := -std=gnu11 -m32 -ffreestanding -fno-builtin -fno-pie -fno-stack-protector \
	-fno-asynchronous-unwind-tables -fno-unwind-tables \
	-mpreferred-stack-boundary=2 -mincoming-stack-boundary=2 \
	-Wall -Wextra -Werror -O2 -Ikernel -Iuser -Iprograms
LDFLAGS := -m32 -nostdlib -no-pie -Wl,--build-id=none -T linker.ld

BUILD_DIR := build
ISO_ROOT := $(BUILD_DIR)/isodir
KERNEL := $(BUILD_DIR)/minios.bin
ISO := $(BUILD_DIR)/minios.iso
OBJECTS = $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/terminal.o \
	$(BUILD_DIR)/keyboard.o $(BUILD_DIR)/interrupt-stub.o \
	$(BUILD_DIR)/interrupt.o $(BUILD_DIR)/timer.o $(BUILD_DIR)/context-switch.o \
	$(BUILD_DIR)/process.o $(BUILD_DIR)/dispatcher.o $(BUILD_DIR)/scheduler.o \
	$(BUILD_DIR)/statistics.o $(BUILD_DIR)/timeline.o $(BUILD_DIR)/experiment.o \
	$(BUILD_DIR)/serial.o $(BUILD_DIR)/telemetry.o \
	$(BUILD_DIR)/selftest.o \
	$(PROGRAM_OBJECTS) \
	$(BUILD_DIR)/shell.o
PROGRAM_SOURCES := programs/programs.c programs/counter.c programs/fibonacci.c \
	programs/prime.c programs/short_task.c programs/long_task.c programs/io_test.c
PROGRAM_OBJECTS := $(patsubst programs/%.c,$(BUILD_DIR)/program-%.o,$(PROGRAM_SOURCES))

.PHONY: all check run test browser-image clean

all: $(ISO)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/boot.o: boot/boot.s | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel.o: kernel/kernel.c kernel/kernel.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/terminal.o: kernel/terminal.c kernel/terminal.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/keyboard.o: kernel/keyboard.c kernel/keyboard.h kernel/io.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/interrupt-stub.o: kernel/interrupt.s | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/interrupt.o: kernel/interrupt.c kernel/interrupt.h kernel/io.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/timer.o: kernel/timer.c kernel/timer.h kernel/interrupt.h kernel/io.h kernel/dispatcher.h kernel/scheduler.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/context-switch.o: kernel/context_switch.s kernel/context_switch.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/process.o: kernel/process.c kernel/process.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/dispatcher.o: kernel/dispatcher.c kernel/dispatcher.h kernel/context_switch.h kernel/interrupt.h kernel/process.h kernel/scheduler.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/scheduler.o: kernel/scheduler.c kernel/scheduler.h kernel/dispatcher.h kernel/interrupt.h kernel/process.h kernel/timeline.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/statistics.o: kernel/statistics.c kernel/statistics.h kernel/process.h kernel/scheduler.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/timeline.o: kernel/timeline.c kernel/timeline.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/serial.o: kernel/serial.c kernel/serial.h kernel/io.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/telemetry.o: kernel/telemetry.c kernel/telemetry.h kernel/process.h kernel/scheduler.h kernel/serial.h kernel/statistics.h kernel/timeline.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/selftest.o: kernel/selftest.c kernel/selftest.h kernel/process.h kernel/scheduler.h kernel/serial.h kernel/statistics.h kernel/timeline.h programs/programs.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/experiment.o: kernel/experiment.c kernel/experiment.h kernel/process.h kernel/scheduler.h kernel/statistics.h programs/programs.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/program-%.o: programs/%.c programs/programs.h kernel/process.h kernel/scheduler.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/shell.o: user/shell.c user/shell.h kernel/experiment.h kernel/keyboard.h kernel/process.h kernel/scheduler.h kernel/selftest.h kernel/statistics.h kernel/telemetry.h kernel/terminal.h kernel/timeline.h kernel/timer.h programs/programs.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJECTS) linker.ld
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

check: $(KERNEL)
	$(GRUB_FILE) --is-x86-multiboot $(KERNEL)

$(ISO): $(KERNEL) grub.cfg
	mkdir -p $(ISO_ROOT)/boot/grub
	cp $(KERNEL) $(ISO_ROOT)/boot/minios.bin
	cp grub.cfg $(ISO_ROOT)/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $(ISO) $(ISO_ROOT)

run: $(ISO)
	$(QEMU) -cdrom $(ISO)

test: $(ISO)
	bash tests/run-qemu-tests.sh $(QEMU) $(ISO) $(BUILD_DIR)/test-serial.log

browser-image: $(ISO)
	mkdir -p browser/public/images
	cp $(ISO) browser/public/images/minios.iso

clean:
	rm -rf $(BUILD_DIR)
