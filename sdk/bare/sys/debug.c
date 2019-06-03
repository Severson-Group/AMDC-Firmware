#include "debug.h"
#include "serial.h"
#include <string.h>

void debug_print(char *msg)
{
	serial_write(msg, strlen(msg));
}
