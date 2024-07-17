#ifndef SERIAL_H
#define SERIAL_H

#define SERIAL_UPDATES_PER_SEC (10000)
#define SERIAL_INTERVAL_TICKS  (pdMS_TO_TICKS(1000.0 / SERIAL_UPDATES_PER_SEC))

void serial_init(void);
void serial_main(void *arg);

void serial_write(char *msg, int len);

#endif // SERIAL_H
