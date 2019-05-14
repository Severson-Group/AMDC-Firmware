#ifndef DEBUG_H
#define DEBUG_H

#include "defines.h"

#define DEBUG_UPDATES_PER_SEC		(1000)
#define DEBUG_INTERVAL_USEC			(USEC_IN_SEC / DEBUG_UPDATES_PER_SEC)

void debug_init(void);
void debug_callback(void);


#endif // DEBUG_H
