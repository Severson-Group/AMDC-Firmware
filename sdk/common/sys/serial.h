#ifndef SERIAL_H
#define SERIAL_H

#include "sys/scheduler.h"

#define SERIAL_UPDATES_PER_SEC (10000)
#define SERIAL_INTERVAL_USEC   (USEC_IN_SEC / SERIAL_UPDATES_PER_SEC)

void serial_init(void);
void serial_callback(void *arg);

void serial_write(char *msg, int len);

#endif // SERIAL_H
