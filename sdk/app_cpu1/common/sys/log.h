#ifndef LOG_H
#define LOG_H

#include "sys/scheduler.h"
#include "usr/user_config.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef USER_CONFIG_LOGGING_MAX_NUM_VARIABLES
#define LOG_MAX_NUM_VARIABLES (USER_CONFIG_LOGGING_MAX_NUM_VARIABLES)
#else
#define LOG_MAX_NUM_VARIABLES (32)
#endif

#ifdef USER_CONFIG_LOGGING_SAMPLE_DEPTH_PER_VARIABLE
#define LOG_SAMPLE_DEPTH_PER_VARIABLE (USER_CONFIG_LOGGING_SAMPLE_DEPTH_PER_VARIABLE)
#else
#define LOG_SAMPLE_DEPTH_PER_VARIABLE (100000)
#endif

// Check the memory usage of the logging buffers... enforce that the total number
// of variable entries is reasonably small, i.e. expected to fit in memory.
//
// The PicoZed 7030 device has 1GB of external memory. When compiling with 100M
// logging slot sample locations, the resulting binary consumes 760MB of RAM.
#if (LOG_SAMPLE_DEPTH_PER_VARIABLE * LOG_MAX_NUM_VARIABLES) > (100 * 1000 * 1000)
#error "Logging memory usage too high! Reduce sample depth or max number of variables."
#endif

// Limit the max number of logging slots. If there are too many possible variables,
// the firmware can't log them all in one time quantum!
//
// This number was found experimentally when no control algorithms were running.
// There is some head room in the limit.
//
// Interestingly, the max number of variables doesn't depend on the scheduler
// time quantum, like you'd think... I think this is because in the logging task
// callback, it iterates over the whole log (all slots)... Since they are stored
// in external memory and the stride of access is VERY large, each memory access
// results in a cache miss in the SoC, so we are seeing ~100s of memory access
// latencies stack up (from CPU core all the way out to external RAM device),
// thus overruns the time slice!
#if LOG_MAX_NUM_VARIABLES > 150
#error "Max number of logging variables too large to meet timing! Please reduce."
#endif

#define LOG_UPDATES_PER_SEC (SYS_TICK_FREQ)
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
int log_var_empty_all(void);

int log_var_dump_ascii(int idx, int dump_method);
int log_var_dump_binary(int idx, int dump_method);

int log_var_unregister(int idx);
int log_var_is_registered(int idx, bool *is_registered);

int log_print_info(void);

int log_stream(bool enable, int idx, int socket_id);
void log_stream_synctime(void);

#endif // LOG_H
