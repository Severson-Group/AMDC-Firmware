#ifndef INJECTION_H
#define INJECTION_H

#include "scheduler.h"

#define INJECTION_UPDATES_PER_SEC			SYS_TICK_FREQ
#define INJECTION_INTERVAL_USEC				(USEC_IN_SEC / INJECTION_UPDATES_PER_SEC)

void injection_init(void);
void injection_callback(void *arg);

#endif // INJECTION_H
