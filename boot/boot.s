/* Multiboot v1 header consumed by GRUB. */
.set MULTIBOOT_MAGIC,    0x1BADB002
.set MULTIBOOT_FLAGS,    0x00000003
.set MULTIBOOT_CHECKSUM, -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

.section .multiboot
.align 4
.long MULTIBOOT_MAGIC
.long MULTIBOOT_FLAGS
.long MULTIBOOT_CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global _start
.extern kernel_main

_start:
    /* GRUB has entered 32-bit protected mode. Give C a private stack. */
    mov $stack_top, %esp

    /* Keep the call boundary aligned and pass GRUB's values using cdecl. */
    sub $8, %esp
    push %ebx
    push %eax
    call kernel_main

1:
    cli
    hlt
    jmp 1b
