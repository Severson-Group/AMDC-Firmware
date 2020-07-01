#include "usr/user_config.h"

#if USER_CONFIG_ENABLE_LOGGING == 1

#include "sys/cmd/cmd_log.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/log.h"
#include "sys/scheduler.h"
#include "sys/util.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define LOG_BUFFER_LENGTH (LOG_VARIABLE_SAMPLE_DEPTH * sizeof(buffer_entry_t))

#define LOG_VAR_NAME_MAX_CHARS (20)

typedef struct buffer_entry_t {
    uint32_t timestamp;
    uint32_t value;
} buffer_entry_t;

typedef struct log_var_t {
    bool is_registered;
    char name[LOG_VAR_NAME_MAX_CHARS];
    void *addr;
    var_type_e type;

    uint32_t log_interval_usec;
    uint64_t last_logged_usec;

    int num_samples;
    buffer_entry_t buffer[LOG_VARIABLE_SAMPLE_DEPTH];
    int buffer_idx;
} log_var_t;

static log_var_t vars[LOG_MAX_NUM_VARS] = { 0 };

static bool is_log_running = false;

static task_control_block_t tcb;

void log_init(void)
{
    // Register task which samples variables etc
    // NOTE: this runs at the base scheduler time quantum,
    //       or as fast as possible!
    scheduler_tcb_init(&tcb, log_callback, NULL, "log", LOG_INTERVAL_USEC);
    scheduler_tcb_register(&tcb);

    // Initialize all the variables to NULL address,
    // which indicates they aren't active
    for (int i = 0; i < LOG_MAX_NUM_VARS; i++) {
        vars[i].addr = NULL;
    }

    // Start with logging disabled
    log_stop();

    // Register command
    cmd_log_register();
}

void log_callback(void *arg)
{
    if (!is_log_running) {
        return;
    }

    uint64_t elapsed_usec = scheduler_get_elapsed_usec();

    for (uint8_t i = 0; i < LOG_MAX_NUM_VARS; i++) {
        log_var_t *v = &vars[i];

        if (!v->is_registered) {
            // Variable not active for logging, so skip
            continue;
        }

        uint64_t usec_since_last_run = elapsed_usec - v->last_logged_usec;

        if (usec_since_last_run >= v->log_interval_usec) {
            // Time to log this variable!
            v->last_logged_usec = elapsed_usec;

            v->buffer[v->buffer_idx].timestamp = (uint32_t) elapsed_usec;

            if (v->type == LOG_INT) {
                v->buffer[v->buffer_idx].value = *((uint32_t *) v->addr);
            } else if (v->type == LOG_FLOAT) {
                float *f = (float *) &(v->buffer[v->buffer_idx].value);
                *f = *((float *) v->addr);
            } else if (v->type == LOG_DOUBLE) {
                float *f = (float *) &(v->buffer[v->buffer_idx].value);
                double value = *((double *) v->addr);
                *f = (float) value;
            }

            v->buffer_idx++;
            if (v->buffer_idx >= LOG_VARIABLE_SAMPLE_DEPTH) {
                v->buffer_idx = 0;
            }

            if (v->num_samples < LOG_VARIABLE_SAMPLE_DEPTH) {
                v->num_samples++;
            }
        }
    }
}

void log_start(void)
{
    is_log_running = true;
}

void log_stop(void)
{
    is_log_running = false;
}

bool log_is_logging(void)
{
    return is_log_running;
}

int log_var_register(int idx, char *name, void *addr, uint32_t samples_per_sec, var_type_e type)
{
    // Sanity check variable idx
    if (idx < 0 || idx >= LOG_MAX_NUM_VARS) {
        return FAILURE;
    }

    // Populate variable entry...
    strncpy(vars[idx].name, name, LOG_VAR_NAME_MAX_CHARS);
    vars[idx].addr = addr;
    vars[idx].type = type;

    // Calculate 'log_interval_usec' from samples per second
    vars[idx].log_interval_usec = USEC_IN_SEC / samples_per_sec;
    vars[idx].last_logged_usec = 0;

    // Mark as registered
    vars[idx].is_registered = true;

    return SUCCESS;
}

// ***************************
// Code for running the state machine to
// clear and empty a log buffer
// ***************************

typedef enum sm_states_empty_e { EMPTY_CLEARING = 1, EMPTY_REMOVE_TASK } sm_states_empty_e;

typedef struct sm_ctx_empty_t {
    sm_states_empty_e state;
    int var_idx;
    int cleared_up_to_idx;
    task_control_block_t tcb;
} sm_ctx_empty_t;

#define MAX_CLEAR_PER_SLICE (100)

#define SM_EMPTY_UPDATES_PER_SEC SYS_TICK_FREQ
#define SM_EMPTY_INTERVAL_USEC   (USEC_IN_SEC / SM_EMPTY_UPDATES_PER_SEC)

void state_machine_empty_callback(void *arg)
{
    sm_ctx_empty_t *ctx = (sm_ctx_empty_t *) arg;

    switch (ctx->state) {
    case EMPTY_CLEARING:
    {
        int from_idx = ctx->cleared_up_to_idx;
        int num_to_clear = MIN(MAX_CLEAR_PER_SLICE, LOG_BUFFER_LENGTH - from_idx);

        memset(&vars[ctx->var_idx].buffer[from_idx], 0, num_to_clear);

        ctx->cleared_up_to_idx = from_idx + num_to_clear;

        if (ctx->cleared_up_to_idx >= LOG_BUFFER_LENGTH) {
            ctx->state = EMPTY_REMOVE_TASK;
        }
        break;
    }

    case EMPTY_REMOVE_TASK:
        scheduler_tcb_unregister(&ctx->tcb);
        break;

    default:
        // Can't happen
        HANG;
        break;
    }
}

static sm_ctx_empty_t ctx_empty;

int log_var_empty(int idx)
{
    // Sanity check variable idx
    if (idx < 0 || idx >= LOG_MAX_NUM_VARS) {
        return FAILURE;
    }

    if (scheduler_tcb_is_registered(&ctx_empty.tcb)) {
        // Already in process of emptying something!!
        return FAILURE;
    }

    vars[idx].buffer_idx = 0;
    vars[idx].last_logged_usec = 0;
    vars[idx].num_samples = 0;

    // Initialize the state machine context
    ctx_empty.state = EMPTY_CLEARING;
    ctx_empty.var_idx = idx;
    ctx_empty.cleared_up_to_idx = 0;

    // Initialize the state machine callback tcb
    scheduler_tcb_init(&ctx_empty.tcb, state_machine_empty_callback, &ctx_empty, "logempty", SM_EMPTY_INTERVAL_USEC);
    scheduler_tcb_register(&ctx_empty.tcb);

    return SUCCESS;
}

// ***************************
// Code for running the state machine to
// dump the log buffers to the UART
// ***************************

typedef enum sm_states_dump_e {
    DUMP_TITLE = 1,
    DUMP_NUM_SAMPLES,
    DUMP_HEADER,
    DUMP_VARIABLES_TS,
    DUMP_VARIABLES_VALUE,
    DUMP_FOOTER,
    DUMP_REMOVE_TASK
} sm_states_dump_e;

typedef struct sm_ctx_dump_t {
    sm_states_dump_e state;
    int var_idx;
    int sample_idx;
    task_control_block_t tcb;
} sm_ctx_dump_t;

#define SM_DUMP_UPDATES_PER_SEC (200)
#define SM_DUMP_INTERVAL_USEC   (USEC_IN_SEC / SM_DUMP_UPDATES_PER_SEC)

void state_machine_dump_callback(void *arg)
{
    sm_ctx_dump_t *ctx = (sm_ctx_dump_t *) arg;

    log_var_t *v = &vars[ctx->var_idx];
    buffer_entry_t *e = &v->buffer[ctx->sample_idx];

    switch (ctx->state) {
    case DUMP_TITLE:
        debug_printf("LOG OF VARIABLE: '%s'\r\n", v->name);
        ctx->state = DUMP_NUM_SAMPLES;
        break;

    case DUMP_NUM_SAMPLES:
        debug_printf("NUM SAMPLES: %d\r\n", v->num_samples);
        ctx->state = DUMP_HEADER;
        break;

    case DUMP_HEADER:
        debug_printf("-------START-------\r\n");
        ctx->state = DUMP_VARIABLES_TS;
        break;

    case DUMP_VARIABLES_TS:
        // Print just the timestamp
        debug_printf("> %ld\t\t", e->timestamp);

        ctx->state = DUMP_VARIABLES_VALUE;
        break;

    case DUMP_VARIABLES_VALUE:
        // Print just the value
        if (v->type == LOG_INT) {
            debug_printf("%ld\r\n", e->value);
        } else if (v->type == LOG_FLOAT || v->type == LOG_DOUBLE) {
            float *f = (float *) &(e->value);
            debug_printf("%f\r\n", *f);
        }

        ctx->sample_idx++;

        if (ctx->sample_idx >= v->num_samples) {
            ctx->state = DUMP_FOOTER;
        } else {
            ctx->state = DUMP_VARIABLES_TS;
        }
        break;

    case DUMP_FOOTER:
        debug_printf("-------END-------\r\n\r\n");

        ctx->state = DUMP_REMOVE_TASK;
        break;

    case DUMP_REMOVE_TASK:
        scheduler_tcb_unregister(&ctx->tcb);
        break;

    default:
        // Can't happen
        HANG;
        break;
    }
}

static sm_ctx_dump_t ctx_dump;

int log_var_dump_uart(int log_var_idx)
{
    // Sanity check variable idx
    if (log_var_idx < 0 || log_var_idx >= LOG_MAX_NUM_VARS) {
        return FAILURE;
    }

    if (!vars[log_var_idx].is_registered) {
        return FAILURE;
    }

    // Initialize the state machine context
    ctx_dump.state = DUMP_TITLE;
    ctx_dump.var_idx = log_var_idx;
    ctx_dump.sample_idx = 0;

    // Initialize the state machine callback tcb
    scheduler_tcb_init(&ctx_dump.tcb, state_machine_dump_callback, &ctx_dump, "logdump", SM_DUMP_INTERVAL_USEC);
    scheduler_tcb_register(&ctx_dump.tcb);

    return SUCCESS;
}

#endif // USER_CONFIG_ENABLE_LOGGING
