#include "usr/user_config.h"

#if USER_CONFIG_ENABLE_LOGGING == 1

#include "sys/cmd/cmd_log.h"
#include "sys/commands.h"
#include "sys/crc32.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/icc_tx.h"
#include "sys/log.h"
#include "sys/scheduler.h"
#include "sys/serial.h"
#include "sys/util.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Used for debugging the logging system (preloads last slot with known data)
#define LOG_DEBUG_ENABLE_PRELOAD_SLOT (0)

// This ought to be larger than the `sys/commands.c` max char per argument
#define LOG_VAR_NAME_MAX_CHARS (40)

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
    uint32_t last_logged_usec;

    int num_samples;
    buffer_entry_t buffer[LOG_SAMPLE_DEPTH_PER_VARIABLE];
    int buffer_idx;

    // Streaming:
    bool is_streaming;
    int socket_id;
    uint32_t last_streamed_usec;
} log_var_t;

static log_var_t vars[LOG_MAX_NUM_VARIABLES] = { 0 };

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
    for (int i = 0; i < LOG_MAX_NUM_VARIABLES; i++) {
        vars[i].addr = NULL;
    }

    // Start with logging disabled
    log_stop();

    // Register command
    cmd_log_register();

#if LOG_DEBUG_ENABLE_PRELOAD_SLOT == 1
    // Initialize one log variable slot (last one) with known information to test dumping
    log_var_register(LOG_MAX_NUM_VARS - 1, "LOG_testvar", (void *) 0x1, 1000, LOG_INT);

    // Get point to slot
    log_var_t *v = &vars[LOG_MAX_NUM_VARS - 1];

    v->buffer_idx = 0;
    v->num_samples = 0;

    // Put fake sample entries into slot buffer
    for (int i = 0; i < 100; i++) {
        v->buffer[v->buffer_idx].timestamp = 2000 + i;

        int out = 2000 + i;
        v->buffer[v->buffer_idx].value = *((uint32_t *) &out);

        v->buffer_idx++;
        v->num_samples++;
    }
#endif // LOG_DEBUG_ENABLE_PRELOAD_SLOT
}

static void _do_log_to_buffer(uint32_t elapsed_usec)
{
    if (!is_log_running) {
        return;
    }

    for (uint8_t i = 0; i < LOG_MAX_NUM_VARIABLES; i++) {
        log_var_t *v = &vars[i];

        if (!v->is_registered) {
            // Variable not active for logging, so skip
            continue;
        }

        uint32_t usec_since_last_run = elapsed_usec - v->last_logged_usec;

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
            if (v->buffer_idx >= LOG_SAMPLE_DEPTH_PER_VARIABLE) {
                v->buffer_idx = 0;
            }

            if (v->num_samples < LOG_SAMPLE_DEPTH_PER_VARIABLE) {
                v->num_samples++;
            }
        }
    }
}

static void _do_log_to_stream(uint32_t elapsed_usec)
{
    for (uint8_t i = 0; i < LOG_MAX_NUM_VARIABLES; i++) {
        log_var_t *v = &vars[i];

        if (!v->is_registered) {
            // Variable not active for logging, so skip
            continue;
        }

        if (!v->is_streaming) {
            // Variable not streaming
            continue;
        }

        uint32_t usec_since_last_streamed = elapsed_usec - v->last_streamed_usec;

        if (usec_since_last_streamed >= v->log_interval_usec) {
            // Time to stream this variable!
            v->last_streamed_usec = elapsed_usec;

            // Build object to stream out
            uint32_t stream_obj_ts = elapsed_usec;
            uint32_t stream_obj_data = 0;

            if (v->type == LOG_INT) {
                stream_obj_data = *((uint32_t *) v->addr);
            } else if (v->type == LOG_FLOAT) {
                float *f = (float *) &(stream_obj_data);
                *f = *((float *) v->addr);
            } else if (v->type == LOG_DOUBLE) {
                float *f = (float *) &(stream_obj_data);
                double value = *((double *) v->addr);
                *f = (float) value;
            }

            // Pass to streaming utility
            icc_tx_log_stream(v->socket_id, i, stream_obj_ts, stream_obj_data);
        }
    }
}

void log_callback(void *arg)
{
    uint32_t elapsed_usec = scheduler_get_elapsed_usec();

    _do_log_to_buffer(elapsed_usec);
    _do_log_to_stream(elapsed_usec);
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
    if (idx < 0 || idx >= LOG_MAX_NUM_VARIABLES) {
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
    if (idx < 0 || idx >= LOG_MAX_NUM_VARIABLES) {
        return FAILURE;
    }

    // Mark as unregistered, this allows slot to be reused
    vars[idx].is_registered = false;

    // Also, empty slot so when variable is rereg the next time,
    // previous samples aren't still stored.
    log_var_empty(idx);

    return SUCCESS;
}

int log_var_is_registered(int idx, bool *is_registered)
{
    // Sanity check variable idx
    if (idx < 0 || idx >= LOG_MAX_NUM_VARIABLES) {
        return FAILURE;
    }

    // Set output variable
    *is_registered = vars[idx].is_registered;

    return SUCCESS;
}

int log_var_empty(int idx)
{
    // Sanity check variable idx
    if (idx < 0 || idx >= LOG_MAX_NUM_VARIABLES) {
        return FAILURE;
    }

    // Result metadata
    vars[idx].buffer_idx = 0;
    vars[idx].last_logged_usec = 0;
    vars[idx].num_samples = 0;

    // Note: we don't have to actually clear the memory buffer since we only dump
    // the number of samples we recorded! So the old data can live in memory
    // and not be an issue.

    return SUCCESS;
}

int log_var_empty_all(void)
{
    for (int idx = 0; idx < LOG_MAX_NUM_VARIABLES; idx++) {
        vars[idx].buffer_idx = 0;
        vars[idx].last_logged_usec = 0;
        vars[idx].num_samples = 0;
    }

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
        cmd_resp_printf("LOG OF VARIABLE: '%s'\r\n", v->name);
        ctx->state = DUMP_ASCII_NUM_SAMPLES;
        break;

    case DUMP_ASCII_NUM_SAMPLES:
        cmd_resp_printf("NUM SAMPLES: %d\r\n", v->num_samples);
        ctx->state = DUMP_ASCII_HEADER;
        break;

    case DUMP_ASCII_HEADER:
        cmd_resp_printf("-------START-------\r\n");

        if (v->num_samples == 0) {
            // Nothing to dump!
            ctx->state = DUMP_ASCII_FOOTER;
        } else {
            ctx->state = DUMP_ASCII_VARIABLES_TS;
        }
        break;

    case DUMP_ASCII_VARIABLES_TS:
        // Print just the timestamp
        cmd_resp_printf("> %ld\t\t", e->timestamp);

        ctx->state = DUMP_ASCII_VARIABLES_VALUE;
        break;

    case DUMP_ASCII_VARIABLES_VALUE:
        // Print just the value
        if (v->type == LOG_INT) {
            cmd_resp_printf("%ld\r\n", e->value);
        } else if (v->type == LOG_FLOAT || v->type == LOG_DOUBLE) {
            float *f = (float *) &(e->value);
            cmd_resp_printf("%f\r\n", *f);
        }

        ctx->sample_idx++;

        if (ctx->sample_idx >= v->num_samples) {
            ctx->state = DUMP_ASCII_FOOTER;
        } else {
            ctx->state = DUMP_ASCII_VARIABLES_TS;
        }
        break;

    case DUMP_ASCII_FOOTER:
        cmd_resp_printf("-------END-------\r\n\r\n");

        ctx->state = DUMP_ASCII_REMOVE_TASK;
        break;

    default:
    case DUMP_ASCII_REMOVE_TASK:
        scheduler_tcb_unregister(&ctx->tcb);
        break;
    }
}

static sm_ctx_dump_ascii_t ctx_dump_ascii;

int log_var_dump_ascii(int log_var_idx, int dump_method)
{
    // Sanity check variable idx
    if (log_var_idx < 0 || log_var_idx >= LOG_MAX_NUM_VARIABLES) {
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
    DUMP_BINARY_SAMPLE_INTERVAL_USEC,
    DUMP_BINARY_DATA_TYPE,
    DUMP_BINARY_SAMPLE_VALUE,
    DUMP_BINARY_MAGIC_FOOTER,
    DUMP_BINARY_PRINT_FOOTER_SPACE,
    DUMP_BINARY_REMOVE_TASK
} sm_states_dump_binary_e;

typedef struct sm_ctx_dump_binary_t {
    sm_states_dump_binary_e state;
    int var_idx;
    int sample_idx;
    int dump_method;
    uint32_t crc;
    task_control_block_t tcb;
} sm_ctx_dump_binary_t;

// State machine at 2kHz results in 2kSPS dumping,
// which is nearly full UART bandwidth at 112500 baud
//
// NOTE: the state machine runs at full 10kHz when
// dumping over Ethernet and binary format!
//
#define SM_DUMP_BIANRY_UPDATES_PER_SEC (2000)
#define SM_DUMP_BINARY_INTERVAL_USEC   (USEC_IN_SEC / SM_DUMP_BIANRY_UPDATES_PER_SEC)

static const uint32_t MAGIC_HEADER = 0x12345678;
static const uint32_t MAGIC_FOOTER = 0x11223344;

void state_machine_dump_binary_callback(void *arg)
{
    sm_ctx_dump_binary_t *ctx = (sm_ctx_dump_binary_t *) arg;

    log_var_t *v = &vars[ctx->var_idx];

    switch (ctx->state) {

    case DUMP_BINARY_MAGIC_HEADER:
    {
        // Write to data stream output (UART)
        cmd_resp_write((char *) &MAGIC_HEADER, 4);
        cmd_resp_write((char *) &MAGIC_HEADER, 4);
        cmd_resp_write((char *) &MAGIC_HEADER, 4);
        cmd_resp_write((char *) &MAGIC_HEADER, 4);

        // Run data through CRC
        ctx->crc = crc32_calc_part((uint8_t *) &MAGIC_HEADER, 4, ctx->crc);
        ctx->crc = crc32_calc_part((uint8_t *) &MAGIC_HEADER, 4, ctx->crc);
        ctx->crc = crc32_calc_part((uint8_t *) &MAGIC_HEADER, 4, ctx->crc);
        ctx->crc = crc32_calc_part((uint8_t *) &MAGIC_HEADER, 4, ctx->crc);

        ctx->state = DUMP_BINARY_NUM_SAMPLES;
        break;
    }

    case DUMP_BINARY_NUM_SAMPLES:
    {
        uint32_t out = (uint32_t) v->num_samples;

        // Write to output data stream (UART)
        cmd_resp_write((char *) &out, 4);

        // Run data through CRC
        ctx->crc = crc32_calc_part((uint8_t *) &out, 4, ctx->crc);

        ctx->state = DUMP_BINARY_SAMPLE_INTERVAL_USEC;
        break;
    }

    case DUMP_BINARY_SAMPLE_INTERVAL_USEC:
    {
        uint32_t interval_usec = (uint32_t) v->log_interval_usec;

        // Write to output data stream (UART)
        cmd_resp_write((char *) &interval_usec, 4);

        // Run data through CRC
        ctx->crc = crc32_calc_part((uint8_t *) &interval_usec, 4, ctx->crc);

        ctx->state = DUMP_BINARY_DATA_TYPE;
        break;
    }

    case DUMP_BINARY_DATA_TYPE:
    {
        uint32_t var_type = (uint32_t) v->type;

        // Write to output data stream (UART)
        cmd_resp_write((char *) &var_type, 4);

        // Run data through CRC
        ctx->crc = crc32_calc_part((uint8_t *) &var_type, 4, ctx->crc);

        if (v->num_samples == 0) {
            // Nothing to dump!
            ctx->state = DUMP_BINARY_MAGIC_FOOTER;
        } else {
            ctx->state = DUMP_BINARY_SAMPLE_VALUE;
        }
        break;
    }

    case DUMP_BINARY_SAMPLE_VALUE:
    {
        int max_num_samples = 1;
        if (ctx->dump_method == 2) {
            // Means Ethernet
            max_num_samples = 20;

            // To compute network load:
            // Mbps = (max_num_samples*4*8*10e3) / (1024*1024)
            //
            // 20 ==> 6.1 Mbps
            //
            // This is the upper limit; might stop early
            // due to buffer getting full!
        }

        for (int i = 0; i < max_num_samples; i++) {
            buffer_entry_t *e = &v->buffer[ctx->sample_idx];

            // Stop when the buffer gets full
            // Leave a few bytes of extra space free
            if (task_icc_tx_get_buffer_space_available() < 10) {
                // Break out of this local for loop
                break;
            }

            // Dump the sampled value
            if (v->type == LOG_INT) {
                int32_t out = (int32_t) e->value;
                cmd_resp_write((char *) &out, 4);
                ctx->crc = crc32_calc_part((uint8_t *) &out, 4, ctx->crc);
            } else if (v->type == LOG_FLOAT || v->type == LOG_DOUBLE) {
                // During the sampling, the distinction between float and double variable types
                // was accounted for. The data is stored in the log array in the float format.
                float out = *((float *) &(e->value));
                cmd_resp_write((char *) &out, 4);
                ctx->crc = crc32_calc_part((uint8_t *) &out, 4, ctx->crc);
            }

            ctx->sample_idx++;

            // Stay in the current state until we have dumped enough samples
            if (ctx->sample_idx >= v->num_samples) {
                ctx->state = DUMP_BINARY_MAGIC_FOOTER;

                // Break out of this local for loop
                break;
            }
        }

        break;
    }

    case DUMP_BINARY_MAGIC_FOOTER:
    {
        cmd_resp_write((char *) &MAGIC_FOOTER, 4);
        cmd_resp_write((char *) &MAGIC_FOOTER, 4);
        cmd_resp_write((char *) &MAGIC_FOOTER, 4);
        cmd_resp_write((char *) &MAGIC_FOOTER, 4);

        // Calculate CRC across footer bytes
        ctx->crc = crc32_calc_part((uint8_t *) &MAGIC_FOOTER, 4, ctx->crc);
        ctx->crc = crc32_calc_part((uint8_t *) &MAGIC_FOOTER, 4, ctx->crc);
        ctx->crc = crc32_calc_part((uint8_t *) &MAGIC_FOOTER, 4, ctx->crc);
        ctx->crc = crc32_calc_part((uint8_t *) &MAGIC_FOOTER, 4, ctx->crc);

        // After we are done calculating the CRC, we must
        // invert it to match the Python implementation.
        ctx->crc = ~(ctx->crc);

        // The CRC includes the footer, so the client must
        // read an extra 4 bytes (the CRC bytes!)
        cmd_resp_write((char *) &(ctx->crc), 4);

        ctx->state = DUMP_BINARY_PRINT_FOOTER_SPACE;
        break;
    }

    case DUMP_BINARY_PRINT_FOOTER_SPACE:
    {
        cmd_resp_print("\r\n\r\n");
        ctx->state = DUMP_BINARY_REMOVE_TASK;
        break;
    }

    case DUMP_BINARY_REMOVE_TASK:
    default:
    {
        scheduler_tcb_unregister(&ctx->tcb);
        break;
    }
    }
}

static sm_ctx_dump_binary_t ctx_dump_binary;

int log_var_dump_binary(int log_var_idx, int dump_method)
{
    // Sanity check variable idx
    if (log_var_idx < 0 || log_var_idx >= LOG_MAX_NUM_VARIABLES) {
        return FAILURE;
    }

    if (!vars[log_var_idx].is_registered) {
        return FAILURE;
    }

    // Initialize the state machine context
    ctx_dump_binary.state = DUMP_BINARY_MAGIC_HEADER;
    ctx_dump_binary.var_idx = log_var_idx;
    ctx_dump_binary.sample_idx = 0;
    ctx_dump_binary.dump_method = dump_method;

    uint32_t INTERVAL_USEC = SM_DUMP_BINARY_INTERVAL_USEC;
    if (dump_method == 2) {
        // Means Ethernet ==> run state machine at 10 kHz
        INTERVAL_USEC = (USEC_IN_SEC / 10e3);
    }

    // CRC-32 must be seeded as follows:
    ctx_dump_binary.crc = CRC32_DEFAULT_INIT;

    // Initialize the state machine callback tcb
    scheduler_tcb_init(
        &ctx_dump_binary.tcb, state_machine_dump_binary_callback, &ctx_dump_binary, "logdbin", INTERVAL_USEC);
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
        cmd_resp_printf("Log Info\r\n");
        cmd_resp_printf("--------\r\n");
        ctx->state = INFO_MAX_SLOTS;
        break;
    }

    case INFO_MAX_SLOTS:
    {
        cmd_resp_printf("Max slots: %d\r\n", LOG_MAX_NUM_VARIABLES);
        ctx->state = INFO_MAX_DEPTH;
        break;
    }

    case INFO_MAX_DEPTH:
    {
        cmd_resp_printf("Max sample depth: %d\r\n", LOG_SAMPLE_DEPTH_PER_VARIABLE);
        ctx->state = INFO_MAX_SAMPLE_RATE;
        break;
    }

    case INFO_MAX_SAMPLE_RATE:
    {
        cmd_resp_printf("Max sample rate: %d Hz\r\n", LOG_UPDATES_PER_SEC);
        cmd_resp_printf("--------\r\n");
        ctx->state = INFO_VAR_TITLE;
        break;
    }

    case INFO_VAR_TITLE:
    {
        if (v->is_registered) {
            cmd_resp_printf("Slot %d:\r\n", ctx->var_idx);
            ctx->state = INFO_VAR_DATA1;
        } else {
            cmd_resp_printf("Slot %d: unused\r\n", ctx->var_idx);
            ctx->state = INFO_NEXT_VAR;
        }
        break;
    }

    case INFO_VAR_DATA1:
    {
        cmd_resp_printf("  Name: %s\r\n", v->name);
        ctx->state = INFO_VAR_DATA2;
        break;
    }

    case INFO_VAR_DATA2:
    {
        if (v->type == LOG_INT) {
            cmd_resp_printf("  Type: int\r\n");
        } else if (v->type == LOG_FLOAT) {
            cmd_resp_printf("  Type: float\r\n");
        } else {
            cmd_resp_printf("  Type: double\r\n");
        }
        ctx->state = INFO_VAR_DATA3;
        break;
    }

    case INFO_VAR_DATA3:
    {
        cmd_resp_printf("  Memory address: 0x%X\r\n", v->addr);
        ctx->state = INFO_VAR_DATA4;
        break;
    }

    case INFO_VAR_DATA4:
    {
        cmd_resp_printf("  Sampling interval (usec): %d\r\n", v->log_interval_usec);
        ctx->state = INFO_VAR_DATA5;
        break;
    }

    case INFO_VAR_DATA5:
    {
        cmd_resp_printf("  Num samples: %d\r\n", v->num_samples);
        ctx->state = INFO_NEXT_VAR;
        break;
    }

    case INFO_NEXT_VAR:
    {
        ctx->var_idx++;
        if (ctx->var_idx >= LOG_MAX_NUM_VARIABLES) {
            cmd_resp_printf("\r\n");
            ctx->state = INFO_REMOVE_TASK;
        } else {
            ctx->state = INFO_VAR_TITLE;
        }
        break;
    }

    case INFO_REMOVE_TASK:
    default:
        cmd_resp_printf("SUCCESS\r\n\n");
        scheduler_tcb_unregister(&ctx->tcb);
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

int log_stream(bool enable, int idx, int socket_id)
{
    bool is_registered = false;
    if (log_var_is_registered(idx, &is_registered) != SUCCESS) {
        return FAILURE;
    }

    if (!is_registered) {
        return FAILURE;
    }

    if ((enable && vars[idx].is_streaming) || (!enable && !vars[idx].is_streaming)) {
        return FAILURE;
    }

    vars[idx].is_streaming = enable;
    vars[idx].last_streamed_usec = 0;
    vars[idx].socket_id = socket_id;

    return SUCCESS;
}

void log_stream_synctime(void)
{
    for (uint8_t i = 0; i < LOG_MAX_NUM_VARIABLES; i++) {
        log_var_t *v = &vars[i];

        if (!v->is_registered) {
            // Variable not active for logging, so skip
            continue;
        }

        if (!v->is_streaming) {
            // Variable not streaming
            continue;
        }

        v->last_streamed_usec = 0;
    }
}

#endif // USER_CONFIG_ENABLE_LOGGING
