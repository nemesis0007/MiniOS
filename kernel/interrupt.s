.section .text
.global irq0_stub
.extern timer_interrupt_handler

irq0_stub:
    cld
    pusha
    call timer_interrupt_handler
    popa
    iret

