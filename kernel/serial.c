#include "serial.h"
#include "io.h"

enum { COM1 = 0x3F8 };

static int serial_available;

void serial_initialize(void)
{
    io_out8(COM1 + 1, 0x00);
    io_out8(COM1 + 3, 0x80);
    io_out8(COM1 + 0, 0x03);
    io_out8(COM1 + 1, 0x00);
    io_out8(COM1 + 3, 0x03);
    io_out8(COM1 + 2, 0xC7);
    io_out8(COM1 + 4, 0x0B);
    serial_available = 1;
}

void serial_putchar(char character)
{
    unsigned int attempt;

    if (serial_available == 0) {
        return;
    }
    for (attempt = 0; attempt < 100000U; ++attempt) {
        if ((io_in8(COM1 + 5) & 0x20U) != 0) {
            io_out8(COM1, (unsigned char)character);
            return;
        }
    }
}

void serial_write(const char *text)
{
    unsigned int index = 0;

    while (text[index] != '\0') {
        serial_putchar(text[index]);
        ++index;
    }
}

void serial_writeline(const char *text)
{
    serial_write(text);
    serial_putchar('\n');
}

void serial_write_uint(unsigned int value)
{
    char digits[10];
    unsigned int count = 0;

    if (value == 0) {
        serial_putchar('0');
        return;
    }
    while (value != 0) {
        digits[count++] = (char)('0' + value % 10);
        value /= 10;
    }
    while (count != 0) {
        serial_putchar(digits[--count]);
    }
}
