#include "sys/debug.h"
#include "sys/serial.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_LENGTH (1024)
static char buffer[BUFFER_LENGTH] = { 0 };

void debug_print(char *msg)
{
    serial_write(msg, strlen(msg));
}

void debug_printf(const char *format, ...)
{
    va_list vargs;
    va_start(vargs, format);

    vsnprintf(buffer, BUFFER_LENGTH, format, vargs);
    debug_print(buffer);

    va_end(vargs);
}
