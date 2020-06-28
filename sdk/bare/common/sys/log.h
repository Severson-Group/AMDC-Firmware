#ifndef LOG_H
#define LOG_H

#include "sys/scheduler.h"
#include <stdbool.h>
#include <stdint.h>

#define LOG_MAX_NUM_VARS          (32)
#define LOG_VARIABLE_SAMPLE_DEPTH (100000)

#define LOG_UPDATES_PER_SEC SYS_TICK_FREQ
#define LOG_INTERVAL_USEC   (USEC_IN_SEC / LOG_UPDATES_PER_SEC)

typedef enum var_type_e {
    LOG_INT = 1,
    LOG_FLOAT,
    LOG_DOUBLE,
} var_type_e;

void log_init(void);
void log_callback(void *arg);

void log_start(void);
void log_stop(void);
bool log_is_logging(void);

int log_var_register(int idx, char *name, void *addr, uint32_t samples_per_sec, var_type_e type);

int log_var_empty(int idx);
int log_var_dump_uart(int idx);

#endif // LOG_H
