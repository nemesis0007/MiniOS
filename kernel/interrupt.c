#include "interrupt.h"
#include "io.h"

enum {
    IDT_ENTRY_COUNT = 256,
    IDT_INTERRUPT_GATE = 0x8E,
    PIC_MASTER_COMMAND = 0x20,
    PIC_MASTER_DATA = 0x21,
    PIC_SLAVE_COMMAND = 0xA0,
    PIC_SLAVE_DATA = 0xA1,
    PIC_EOI = 0x20
};

typedef struct idt_entry {
    unsigned short offset_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char attributes;
    unsigned short offset_high;
} __attribute__((packed)) idt_entry_t;

typedef struct idt_pointer {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)) idt_pointer_t;

static idt_entry_t idt[IDT_ENTRY_COUNT];

extern void irq0_stub(void);

static unsigned short current_code_selector(void)
{
    unsigned short selector;
    __asm__ volatile ("mov %%cs, %0" : "=r"(selector));
    return selector;
}

static void idt_set_gate(unsigned int vector, void (*handler)(void))
{
    unsigned int address = (unsigned int)handler;

    idt[vector].offset_low = (unsigned short)(address & 0xFFFFU);
    idt[vector].selector = current_code_selector();
    idt[vector].zero = 0;
    idt[vector].attributes = IDT_INTERRUPT_GATE;
    idt[vector].offset_high = (unsigned short)((address >> 16) & 0xFFFFU);
}

static void pic_remap_and_mask(void)
{
    io_out8(PIC_MASTER_COMMAND, 0x11);
    io_out8(PIC_SLAVE_COMMAND, 0x11);
    io_out8(PIC_MASTER_DATA, 0x20);
    io_out8(PIC_SLAVE_DATA, 0x28);
    io_out8(PIC_MASTER_DATA, 0x04);
    io_out8(PIC_SLAVE_DATA, 0x02);
    io_out8(PIC_MASTER_DATA, 0x01);
    io_out8(PIC_SLAVE_DATA, 0x01);
    io_out8(PIC_MASTER_DATA, 0xFE);
    io_out8(PIC_SLAVE_DATA, 0xFF);
}

void interrupt_initialize(void)
{
    idt_pointer_t pointer;
    unsigned int index;

    interrupt_disable();
    for (index = 0; index < IDT_ENTRY_COUNT; ++index) {
        idt[index].offset_low = 0;
        idt[index].selector = 0;
        idt[index].zero = 0;
        idt[index].attributes = 0;
        idt[index].offset_high = 0;
    }

    idt_set_gate(32, irq0_stub);
    pic_remap_and_mask();
    pointer.limit = (unsigned short)(sizeof(idt) - 1);
    pointer.base = (unsigned int)&idt[0];
    __asm__ volatile ("lidt %0" : : "m"(pointer));
}

void interrupt_enable(void)
{
    __asm__ volatile ("sti");
}

void interrupt_disable(void)
{
    __asm__ volatile ("cli");
}

void interrupt_send_eoi(unsigned char irq)
{
    if (irq >= 8) {
        io_out8(PIC_SLAVE_COMMAND, PIC_EOI);
    }
    io_out8(PIC_MASTER_COMMAND, PIC_EOI);
}

