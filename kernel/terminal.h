#ifndef MINIOS_TERMINAL_H
#define MINIOS_TERMINAL_H

void terminal_initialize(void);
void terminal_clear(void);
void terminal_backspace(void);
void terminal_putchar(char character);
void terminal_write(const char *text);
void terminal_writeline(const char *text);
void terminal_write_uint(unsigned int value);
void terminal_write_hundredths(unsigned int value);

#endif
