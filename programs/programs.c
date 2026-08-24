#include "programs.h"

static const program_descriptor_t programs[] = {
    { "counter",   program_counter,   8,  3 },
    { "fibonacci", program_fibonacci, 12, 2 },
    { "prime",     program_prime,     16, 2 },
    { "short",     program_short_task, 3, 1 },
    { "long",      program_long_task, 24, 4 },
    { "io_test",   program_io_test,   10, 2 }
};

static int strings_equal(const char *left, const char *right)
{
    unsigned int index = 0;

    while (left[index] != '\0' && right[index] != '\0') {
        if (left[index] != right[index]) {
            return 0;
        }
        ++index;
    }
    return left[index] == right[index];
}

unsigned int program_count(void)
{
    return sizeof(programs) / sizeof(programs[0]);
}

const program_descriptor_t *program_at(unsigned int index)
{
    if (index >= program_count()) {
        return 0;
    }
    return &programs[index];
}

const program_descriptor_t *program_lookup(const char *name)
{
    unsigned int index;

    if (name == 0) {
        return 0;
    }

    for (index = 0; index < program_count(); ++index) {
        if (strings_equal(name, programs[index].name)) {
            return &programs[index];
        }
    }
    return 0;
}

int program_registry_self_test(void)
{
    unsigned int index;

    if (program_count() != 6 || program_lookup("missing") != 0) {
        return 0;
    }

    for (index = 0; index < program_count(); ++index) {
        const program_descriptor_t *program = program_at(index);

        if (program == 0 || program->name == 0 || program->entry == 0 ||
            program->estimated_burst == 0 || program->default_priority == 0 ||
            program_lookup(program->name) != program) {
            return 0;
        }
    }
    return 1;
}

