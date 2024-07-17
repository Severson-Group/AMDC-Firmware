#ifndef WATCHDOG_H
#define WATCHDOG_H

/* this might not do anything anymore (because no scheduler), but you know what they say. We'll let sleeping dogs lie */

void watchdog_init(void);
void watchdog_reset(void);

#endif // WATCHDOG_H
