#ifndef COMMANDS_H
#define COMMANDS_H

#include "defines.h"

#define COMMANDS_UPDATES_PER_SEC	(10000)
#define COMMANDS_INTERVAL_USEC		(USEC_IN_SEC / COMMANDS_UPDATES_PER_SEC)

void commands_init(void);
void commands_callback(void);

#endif // COMMANDS_H
