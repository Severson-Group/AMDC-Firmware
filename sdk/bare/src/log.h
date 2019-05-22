#ifndef LOG_H
#define LOG_H

#include <stdint.h>

#define LOG_MAX_NUM_VARS				(16)
#define LOG_VARIABLE_SAMPLE_DEPTH		(100)


void log_init(void);
void log_callback(void);

void log_start(void);
void log_stop(void);

void log_var_register(int idx, char* name, void *addr, uint32_t samples_per_sec);
void log_var_empty(int idx);
void log_var_dump_uart(int idx);

#endif // LOG_H
