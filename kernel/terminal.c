#include "terminal.h"

enum {
    VGA_WIDTH = 80,
    VGA_HEIGHT = 25,
    VGA_COLOR_LIGHT_GREY_ON_BLACK = 0x07
};

static volatile unsigned short *const VGA_MEMORY =
    (volatile unsigned short *)0xB8000;

static unsigned int terminal_row;
static unsigned int terminal_column;
static unsigned char terminal_color;

static unsigned short vga_entry(char character, unsigned char color)
{
    return (unsigned short)(unsigned char)character |
           ((unsigned short)color << 8);
}

static void terminal_scroll(void)
{
    unsigned int row;
    unsigned int column;

    for (row = 1; row < VGA_HEIGHT; ++row) {
        for (column = 0; column < VGA_WIDTH; ++column) {
            VGA_MEMORY[(row - 1) * VGA_WIDTH + column] =
                VGA_MEMORY[row * VGA_WIDTH + column];
        }
    }

    for (column = 0; column < VGA_WIDTH; ++column) {
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + column] =
            vga_entry(' ', terminal_color);
    }

    terminal_row = VGA_HEIGHT - 1;
}

void terminal_clear(void)
{
    unsigned int cell;

    for (cell = 0; cell < VGA_WIDTH * VGA_HEIGHT; ++cell) {
        VGA_MEMORY[cell] = vga_entry(' ', terminal_color);
    }

    terminal_row = 0;
    terminal_column = 0;
}

void terminal_initialize(void)
{
    terminal_color = VGA_COLOR_LIGHT_GREY_ON_BLACK;
    terminal_clear();
}

void terminal_backspace(void)
{
    if (terminal_column != 0) {
        --terminal_column;
    } else if (terminal_row != 0) {
        --terminal_row;
        terminal_column = VGA_WIDTH - 1;
    } else {
        return;
    }

    VGA_MEMORY[terminal_row * VGA_WIDTH + terminal_column] =
        vga_entry(' ', terminal_color);
}

void terminal_putchar(char character)
{
    if (character == '\n') {
        terminal_column = 0;
        ++terminal_row;
    } else if (character == '\r') {
        terminal_column = 0;
    } else {
        VGA_MEMORY[terminal_row * VGA_WIDTH + terminal_column] =
            vga_entry(character, terminal_color);
        ++terminal_column;

        if (terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            ++terminal_row;
        }
    }

    if (terminal_row == VGA_HEIGHT) {
        terminal_scroll();
    }
}

void terminal_write(const char *text)
{
    unsigned int index = 0;

    while (text[index] != '\0') {
        terminal_putchar(text[index]);
        ++index;
    }
}

void terminal_writeline(const char *text)
{
    terminal_write(text);
    terminal_putchar('\n');
}

void terminal_write_uint(unsigned int value)
{
    char digits[10];
    unsigned int count = 0;

    if (value == 0) {
        terminal_putchar('0');
        return;
    }

    while (value != 0) {
        digits[count] = (char)('0' + value % 10);
        value /= 10;
        ++count;
    }

    while (count != 0) {
        --count;
        terminal_putchar(digits[count]);
    }
}

void terminal_write_hundredths(unsigned int value)
{
    unsigned int fraction = value % 100;

    terminal_write_uint(value / 100);
    terminal_putchar('.');
    if (fraction < 10) {
        terminal_putchar('0');
    }
    terminal_write_uint(fraction);
}
