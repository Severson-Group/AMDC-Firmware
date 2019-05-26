#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
#include "../sys/defines.h"

#define DEBUG_UPDATES_PER_SEC		(10000)
#define DEBUG_INTERVAL_USEC			(USEC_IN_SEC / DEBUG_UPDATES_PER_SEC)

void debug_init(void);
void debug_callback(void *arg);

void debug_print(const char *msg);

#endif // DEBUG_H
