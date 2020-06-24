#include "usr/user_defines.h"

#if ENABLE_LOGGING == 1

#include "sys/cmd/cmd_log.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/log.h"
#include "sys/scheduler.h"
#include "sys/serial.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void log_preload(void);

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

    // TODO(NP): Debugging!
    // Initialize one log variable slot with known information to test dumping
    //    log_preload();
}

void log_preload(void)
{
    // Register last slot
    log_var_register(LOG_MAX_NUM_VARS - 1, "testvar", (void *) 0x1, 1000, LOG_INT);

    // Get point to slot
    log_var_t *v = &vars[LOG_MAX_NUM_VARS - 1];

    v->buffer_idx = 0;
    v->num_samples = 0;

    // Put fake sample entries into slot buffer
    for (int i = 0; i < 10000; i++) {
        v->buffer[v->buffer_idx].timestamp = 1000 + i;

        int out = 1000 + (10 * i);
        v->buffer[v->buffer_idx].value = *((uint32_t *) &out);

        v->buffer_idx++;
        v->num_samples++;
    }
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

int log_var_unregister(int idx)
{
    // Sanity check variable idx
    if (idx < 0 || idx >= LOG_MAX_NUM_VARS) {
        return FAILURE;
    }

    vars[idx].is_registered = false;

    return SUCCESS;
}

int log_var_is_registered(int idx, bool *is_registered)
{
    // Sanity check variable idx
    if (idx < 0 || idx >= LOG_MAX_NUM_VARS) {
        return FAILURE;
    }

    // Set output variable
    *is_registered = vars[idx].is_registered;

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

        char *mem_address = (char *) &vars[ctx->var_idx].buffer[0];
        memset(&mem_address[from_idx], 0, num_to_clear);

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
// using ASCII characters (human readable)
// ***************************

typedef enum sm_states_dump_ascii_e {
    DUMP_ASCII_TITLE = 1,
    DUMP_ASCII_NUM_SAMPLES,
    DUMP_ASCII_HEADER,
    DUMP_ASCII_VARIABLES_TS,
    DUMP_ASCII_VARIABLES_VALUE,
    DUMP_ASCII_FOOTER,
    DUMP_ASCII_REMOVE_TASK
} sm_states_dump_ascii_e;

typedef struct sm_ctx_dump_ascii_t {
    sm_states_dump_ascii_e state;
    int var_idx;
    int sample_idx;
    task_control_block_t tcb;
} sm_ctx_dump_ascii_t;

#define SM_DUMP_ASCII_UPDATES_PER_SEC (200)
#define SM_DUMP_ASCII_INTERVAL_USEC   (USEC_IN_SEC / SM_DUMP_ASCII_UPDATES_PER_SEC)

void state_machine_dump_ascii_callback(void *arg)
{
    sm_ctx_dump_ascii_t *ctx = (sm_ctx_dump_ascii_t *) arg;

    log_var_t *v = &vars[ctx->var_idx];
    buffer_entry_t *e = &v->buffer[ctx->sample_idx];

    switch (ctx->state) {
    case DUMP_ASCII_TITLE:
        debug_printf("LOG OF VARIABLE: '%s'\r\n", v->name);
        ctx->state = DUMP_ASCII_NUM_SAMPLES;
        break;

    case DUMP_ASCII_NUM_SAMPLES:
        debug_printf("NUM SAMPLES: %d\r\n", v->num_samples);
        ctx->state = DUMP_ASCII_HEADER;
        break;

    case DUMP_ASCII_HEADER:
        debug_printf("-------START-------\r\n");

        if (v->num_samples == 0) {
            // Nothing to dump!
            ctx->state = DUMP_ASCII_FOOTER;
        } else {
            ctx->state = DUMP_ASCII_VARIABLES_TS;
        }
        break;

    case DUMP_ASCII_VARIABLES_TS:
        // Print just the timestamp
        debug_printf("> %ld\t\t", e->timestamp);

        ctx->state = DUMP_ASCII_VARIABLES_VALUE;
        break;

    case DUMP_ASCII_VARIABLES_VALUE:
        // Print just the value
        if (v->type == LOG_INT) {
            debug_printf("%ld\r\n", e->value);
        } else if (v->type == LOG_FLOAT || v->type == LOG_DOUBLE) {
            float *f = (float *) &(e->value);
            debug_printf("%f\r\n", *f);
        }

        ctx->sample_idx++;

        if (ctx->sample_idx >= v->num_samples) {
            ctx->state = DUMP_ASCII_FOOTER;
        } else {
            ctx->state = DUMP_ASCII_VARIABLES_TS;
        }
        break;

    case DUMP_ASCII_FOOTER:
        debug_printf("-------END-------\r\n\r\n");

        ctx->state = DUMP_ASCII_REMOVE_TASK;
        break;

    case DUMP_ASCII_REMOVE_TASK:
        scheduler_tcb_unregister(&ctx->tcb);
        break;

    default:
        // Can't happen
        HANG;
        break;
    }
}

static sm_ctx_dump_ascii_t ctx_dump_ascii;

int log_var_dump_uart_ascii(int log_var_idx)
{
    // Sanity check variable idx
    if (log_var_idx < 0 || log_var_idx >= LOG_MAX_NUM_VARS) {
        return FAILURE;
    }

    if (!vars[log_var_idx].is_registered) {
        return FAILURE;
    }

    // Initialize the state machine context
    ctx_dump_ascii.state = DUMP_ASCII_TITLE;
    ctx_dump_ascii.var_idx = log_var_idx;
    ctx_dump_ascii.sample_idx = 0;

    // Initialize the state machine callback tcb
    scheduler_tcb_init(&ctx_dump_ascii.tcb,
                       state_machine_dump_ascii_callback,
                       &ctx_dump_ascii,
                       "logdascii",
                       SM_DUMP_ASCII_INTERVAL_USEC);
    scheduler_tcb_register(&ctx_dump_ascii.tcb);

    return SUCCESS;
}

// ***************************
// Code for running the state machine to
// dump the log buffers to the UART using binary
// ***************************

typedef enum sm_states_dump_binary_e {
    DUMP_BINARY_MAGIC_HEADER = 1,
    DUMP_BINARY_NUM_SAMPLES,
    DUMP_BINARY_DATA_TYPE,
    DUMP_BINARY_SAMPLE_TS,
    DUMP_BINARY_SAMPLE_VALUE,
    DUMP_BINARY_MAGIC_FOOTER,
    DUMP_BINARY_REMOVE_TASK
} sm_states_dump_binary_e;

typedef struct sm_ctx_dump_binary_t {
    sm_states_dump_binary_e state;
    int var_idx;
    int sample_idx;
    task_control_block_t tcb;
} sm_ctx_dump_binary_t;

// State machine at 2kHz results in 1kSPS dumping,
// which is nearly full UART bandwidth at 112500 baud
#define SM_DUMP_BIANRY_UPDATES_PER_SEC (2000)
#define SM_DUMP_BINARY_INTERVAL_USEC   (USEC_IN_SEC / SM_DUMP_BIANRY_UPDATES_PER_SEC)

static const uint32_t MAGIC_HEADER = 0x12345678;
static const uint32_t MAGIC_FOOTER = 0x11223344;

void state_machine_dump_binary_callback(void *arg)
{
    sm_ctx_dump_binary_t *ctx = (sm_ctx_dump_binary_t *) arg;

    log_var_t *v = &vars[ctx->var_idx];
    buffer_entry_t *e = &v->buffer[ctx->sample_idx];

    switch (ctx->state) {
    case DUMP_BINARY_MAGIC_HEADER:
    {
        serial_write((char *) &MAGIC_HEADER, 4);
        serial_write((char *) &MAGIC_HEADER, 4);
        serial_write((char *) &MAGIC_HEADER, 4);
        serial_write((char *) &MAGIC_HEADER, 4);
        ctx->state = DUMP_BINARY_NUM_SAMPLES;
        break;
    }

    case DUMP_BINARY_NUM_SAMPLES:
    {
        uint32_t out = (uint32_t) v->num_samples;
        serial_write((char *) &out, 4);
        ctx->state = DUMP_BINARY_DATA_TYPE;
        break;
    }

    case DUMP_BINARY_DATA_TYPE:
    {
        uint32_t var_type = (uint32_t) v->type;
        serial_write((char *) &var_type, 4);

        if (v->num_samples == 0) {
            // Nothing to dump!
            ctx->state = DUMP_BINARY_MAGIC_FOOTER;
        } else {
            ctx->state = DUMP_BINARY_SAMPLE_TS;
        }
        break;
    }

    case DUMP_BINARY_SAMPLE_TS:
    {
        // Dump timestamp
        uint32_t ts = e->timestamp;
        serial_write((char *) &ts, 4);

        ctx->state = DUMP_BINARY_SAMPLE_VALUE;
        break;
    }

    case DUMP_BINARY_SAMPLE_VALUE:
    {
        // Dump value
        if (v->type == LOG_INT) {
            int32_t out = (int32_t) e->value;
            serial_write((char *) &out, 4);
        } else if (v->type == LOG_FLOAT || v->type == LOG_DOUBLE) {
            float out = *((float *) &(e->value));
            serial_write((char *) &out, 4);
        }

        ctx->sample_idx++;

        if (ctx->sample_idx >= v->num_samples) {
            ctx->state = DUMP_BINARY_MAGIC_FOOTER;
        } else {
            ctx->state = DUMP_BINARY_SAMPLE_TS;
        }
        break;
    }

    case DUMP_BINARY_MAGIC_FOOTER:
    {
        serial_write((char *) &MAGIC_FOOTER, 4);
        serial_write((char *) &MAGIC_FOOTER, 4);
        serial_write((char *) &MAGIC_FOOTER, 4);
        serial_write((char *) &MAGIC_FOOTER, 4);
        ctx->state = DUMP_BINARY_REMOVE_TASK;
        break;
    }

    case DUMP_BINARY_REMOVE_TASK:
        debug_print("\r\n\r\n");
        scheduler_tcb_unregister(&ctx->tcb);
        break;

    default:
        // Can't happen
        HANG;
        break;
    }
}

static sm_ctx_dump_binary_t ctx_dump_binary;

int log_var_dump_uart_binary(int log_var_idx)
{
    // Sanity check variable idx
    if (log_var_idx < 0 || log_var_idx >= LOG_MAX_NUM_VARS) {
        return FAILURE;
    }

    if (!vars[log_var_idx].is_registered) {
        return FAILURE;
    }

    // Initialize the state machine context
    ctx_dump_binary.state = DUMP_BINARY_MAGIC_HEADER;
    ctx_dump_binary.var_idx = log_var_idx;
    ctx_dump_binary.sample_idx = 0;

    // Initialize the state machine callback tcb
    scheduler_tcb_init(&ctx_dump_binary.tcb,
                       state_machine_dump_binary_callback,
                       &ctx_dump_binary,
                       "logdbin",
                       SM_DUMP_BINARY_INTERVAL_USEC);
    scheduler_tcb_register(&ctx_dump_binary.tcb);

    return SUCCESS;
}

// ***************************
// Code for printing log info
// ***************************

typedef enum sm_states_info_e {
    INFO_HEAD = 1,
    INFO_MAX_SLOTS,
    INFO_MAX_DEPTH,
    INFO_MAX_SAMPLE_RATE,

    INFO_VAR_TITLE,
    INFO_VAR_DATA1,
    INFO_VAR_DATA2,
    INFO_VAR_DATA3,
    INFO_VAR_DATA4,
    INFO_VAR_DATA5,

    INFO_NEXT_VAR,

    INFO_REMOVE_TASK
} sm_states_info_e;

typedef struct sm_ctx_info_t {
    sm_states_info_e state;
    task_control_block_t tcb;

    int var_idx;
} sm_ctx_info_t;

#define SM_INFO_UPDATES_PER_SEC SYS_TICK_FREQ
#define SM_INFO_INTERVAL_USEC   (USEC_IN_SEC / SM_INFO_UPDATES_PER_SEC)

void state_machine_info_callback(void *arg)
{
    sm_ctx_info_t *ctx = (sm_ctx_info_t *) arg;

    log_var_t *v = &vars[ctx->var_idx];

    switch (ctx->state) {
    case INFO_HEAD:
    {
        debug_printf("Log Info\r\n");
        debug_printf("--------\r\n");
        ctx->state = INFO_MAX_SLOTS;
        break;
    }

    case INFO_MAX_SLOTS:
    {
        debug_printf("Max slots: %d\r\n", LOG_MAX_NUM_VARS);
        ctx->state = INFO_MAX_DEPTH;
        break;
    }

    case INFO_MAX_DEPTH:
    {
        debug_printf("Max sample depth: %d\r\n", LOG_VARIABLE_SAMPLE_DEPTH);
        ctx->state = INFO_MAX_SAMPLE_RATE;
        break;
    }

    case INFO_MAX_SAMPLE_RATE:
    {
        debug_printf("Max sample rate: %d Hz\r\n", LOG_UPDATES_PER_SEC);
        debug_printf("--------\r\n");
        ctx->state = INFO_VAR_TITLE;
        break;
    }

    case INFO_VAR_TITLE:
    {
        if (v->is_registered) {
            debug_printf("Slot %d:\r\n", ctx->var_idx);
            ctx->state = INFO_VAR_DATA1;
        } else {
            debug_printf("Slot %d: unused\r\n", ctx->var_idx);
            ctx->state = INFO_NEXT_VAR;
        }
        break;
    }

    case INFO_VAR_DATA1:
    {
        debug_printf("  Name: %s\r\n", v->name);
        ctx->state = INFO_VAR_DATA2;
        break;
    }

    case INFO_VAR_DATA2:
    {
        if (v->type == LOG_INT) {
            debug_printf("  Type: int\r\n");
        } else if (v->type == LOG_FLOAT) {
            debug_printf("  Type: float\r\n");
        } else {
            debug_printf("  Type: double\r\n");
        }
        ctx->state = INFO_VAR_DATA3;
        break;
    }

    case INFO_VAR_DATA3:
    {
        debug_printf("  Memory address: 0x%X\r\n", v->addr);
        ctx->state = INFO_VAR_DATA4;
        break;
    }

    case INFO_VAR_DATA4:
    {
        debug_printf("  Sampling interval (usec): %d\r\n", v->log_interval_usec);
        ctx->state = INFO_VAR_DATA5;
        break;
    }

    case INFO_VAR_DATA5:
    {
        debug_printf("  Num samples: %d\r\n", v->num_samples);
        ctx->state = INFO_NEXT_VAR;
        break;
    }

    case INFO_NEXT_VAR:
    {
        ctx->var_idx++;
        if (ctx->var_idx >= LOG_MAX_NUM_VARS) {
            ctx->state = INFO_REMOVE_TASK;
        } else {
            ctx->state = INFO_VAR_TITLE;
        }
        break;
    }

    case INFO_REMOVE_TASK:
        debug_printf("\r\n");
        scheduler_tcb_unregister(&ctx->tcb);
        break;

    default:
        // Can't happen
        HANG;
        break;
    }
}

static sm_ctx_info_t ctx_info;

int log_print_info(void)
{
    if (scheduler_tcb_is_registered(&ctx_info.tcb)) {
        // Already in process of printing something!!
        return FAILURE;
    }

    // Initialize the state machine context
    ctx_info.state = INFO_HEAD;
    ctx_info.var_idx = 0;

    // Initialize the state machine callback tcb
    scheduler_tcb_init(&ctx_info.tcb, state_machine_info_callback, &ctx_info, "loginfo", SM_INFO_INTERVAL_USEC);
    scheduler_tcb_register(&ctx_info.tcb);

    return SUCCESS;
}

#endif // ENABLE_LOGGING
