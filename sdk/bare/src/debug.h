#ifndef DEBUG_H
#define DEBUG_H

#include "defines.h"
#include <stdint.h>

#define DEBUG_UPDATES_PER_SEC		(10000)
#define DEBUG_INTERVAL_USEC			(USEC_IN_SEC / DEBUG_UPDATES_PER_SEC)

void debug_init(void);
void debug_callback(void);

void debug_print(const char *msg);

#endif // DEBUG_H
