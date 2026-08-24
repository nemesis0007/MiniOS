#ifndef MINIOS_CONTEXT_SWITCH_H
#define MINIOS_CONTEXT_SWITCH_H

void context_switch(unsigned int *old_stack_pointer,
                    unsigned int new_stack_pointer);

#endif

