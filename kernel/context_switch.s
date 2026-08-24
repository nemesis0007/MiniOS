.section .text
.global context_switch

/*
 * cdecl arguments:
 *   old_stack_pointer address at 4(%esp)
 *   new_stack_pointer value   at 8(%esp)
 *
 * The saved stack layout is mirrored by process_prepare_context().
 */
context_switch:
    pushfl
    push %ebp
    push %ebx
    push %esi
    push %edi

    mov 24(%esp), %eax
    mov %esp, (%eax)
    mov 28(%esp), %esp

    pop %edi
    pop %esi
    pop %ebx
    pop %ebp
    popfl
    ret

