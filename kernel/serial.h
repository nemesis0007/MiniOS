#ifndef MINIOS_SERIAL_H
#define MINIOS_SERIAL_H

void serial_initialize(void);
void serial_putchar(char character);
void serial_write(const char *text);
void serial_writeline(const char *text);
void serial_write_uint(unsigned int value);

#endif
