#include "usr/user_config.h"

#if USER_CONFIG_ENABLE_INJECTION == 1

#include "sys/cmd/cmd_inj.h"
#include "sys/commands.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/injection.h"
#include "sys/scheduler.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Linked list of all registered injection contexts
static inj_ctx_t *inj_ctxs = NULL;

// Used to give each registered ctx a unique ID
static int next_ctx_id = 0;

// Chirp function
//
// Generates the chirp signal value given:
// - time:   current time instant
// - w1:     low freq (rad/s)
// - w2:     high freq (rad/s)
// - A:      amplitude
// - period: time period (sec)
static inline double _chirp(double w1, double w2, double A, double period, double time)
{
    double half_period = period / 2.0;
    double freq_slope = (w2 - w1) / half_period;

    double mytime;
    double mygain;
    if (time < half_period) {
        mytime = time;
        mygain = 1.0;
    } else {
        mytime = period - time;
        mygain = -1.0;
    }

    double freq = freq_slope * mytime / 2.0 + w1;
    double out = A * mygain * sin(freq * mytime);

    return out;
}

static inline double _triangle(double min, double max, double period, double time)
{
    // NOTE: Assumes the user set min = -max. Therefore, the mid value is 0.
    //       Function starts at mid value at t = 0
    //
    // Let mid = min + ((max - min) / 2)
    //
    // Triangle waveform output:
    //        starts at mid at t = 0,
    // S1: increases to max at t = T/4,
    // S2: decreases to mid at t = T/2
    // S3: decreases to min at t = 3T/4
    // S4: increases to mid at t = T

    double out = 0.0;
    double mid = min + ((max - min) / 2.0);

    // Calculate slopes
    double m_pos = (max - min) / (period / 2.0);
    double m_neg = -m_pos;

    // y-intercept
    double x1, y1;

    if (0.0 <= time && time < period / 4.0) {
        // State S1
        out = m_pos * time + mid;
    } else if (period / 4.0 <= time && time < period / 2.0) {
        // State S2
        // y = m(x - x1) + y1
        x1 = period / 4.0;
        y1 = max;
        out = m_neg * (time - x1) + y1;
    } else if (period / 2.0 <= time && time < 3.0 * period / 4.0) {
        // State S3
        // y = m(x - x1) + y1
        x1 = period / 2.0;
        y1 = mid;
        out = m_neg * (time - x1) + y1;
    } else {
        // State S4
        // y = m(x - x1) + y1
        x1 = 3.0 * period / 4.0;
        y1 = min;
        out = m_pos * (time - x1) + y1;
    }

    return out;
}

static inline double _square(double min, double max, double period, double time)
{
    // Output defaults to min value
    double out = min;

    // Output becomes max value for second half of period
    if (time >= period / 2.0) {
        out = max;
    }

    return out;
}

void injection_init(void)
{
    cmd_inj_register();
}

void injection_ctx_init(inj_ctx_t *ctx, char *name)
{
    ctx->id = next_ctx_id++;
    strncpy(ctx->name, name, INJ_MAX_NAME_LENGTH);
}

int injection_ctx_register(inj_ctx_t *ctx)
{
    // Don't let clients re-register their ctx
    if (ctx->registered) {
        return FAILURE;
    }

    // Mark as registered
    ctx->registered = 1;

    // Base case: there are no tasks in linked list
    if (inj_ctxs == NULL) {
        inj_ctxs = ctx;
        inj_ctxs->next = NULL;
        return SUCCESS;
    }

    // Find end of list
    inj_ctx_t *curr = inj_ctxs;
    while (curr->next != NULL)
        curr = curr->next;

    // Append new ctx to end of list
    curr->next = ctx;
    ctx->next = NULL;

    return SUCCESS;
}

int injection_ctx_unregister(inj_ctx_t *ctx)
{
    // Don't let clients unregister their already unregistered ctx
    if (!ctx->registered) {
        return FAILURE;
    }

    // Mark as unregistered
    ctx->registered = 0;

    // Make sure list isn't empty
    if (inj_ctxs == NULL) {
        return FAILURE;
    }

    // Special case: trying to remove the head of the list
    if (inj_ctxs->id == ctx->id) {
        inj_ctxs = inj_ctxs->next;
        return SUCCESS;
    }

    // Now we know that 'ctx' to remove is NOT first node

    inj_ctx_t *prev = NULL;
    inj_ctx_t *curr = inj_ctxs;

    // Find spot in linked list to remove ctx
    while (curr->id != ctx->id) {
        prev = curr;
        curr = curr->next;
    }

    // 'curr' is now the one we want to remove!

    prev->next = curr->next;

    return SUCCESS;
}

void injection_inj(double *output, inj_ctx_t *ctx, double Ts)
{
    if (!ctx->registered) {
        return;
    }

    double value = 0.0;

    switch (ctx->inj_func) {
    case CONST:
    {
        value = ctx->constant.value;
        break;
    }

    case NOISE:
    {
        // Generate random number between 0..1
        double r = (double) rand() / (double) RAND_MAX;

        // Make between -1.0 .. 1.0
        r = (2.0 * r) - 1.0;

        value = ctx->noise.gain * r;
        value += ctx->noise.offset;
        break;
    }

    case CHIRP:
    {
        ctx->curr_time += Ts;
        if (ctx->curr_time >= ctx->chirp.period) {
            ctx->curr_time = 0.0;
        }

        value = _chirp(
            PI2 * ctx->chirp.freqMin, PI2 * ctx->chirp.freqMax, ctx->chirp.gain, ctx->chirp.period, ctx->curr_time);
        break;
    }

    case TRIANGLE:
    {
        ctx->curr_time += Ts;
        if (ctx->curr_time >= ctx->triangle.period) {
            ctx->curr_time = 0.0;
        }

        value = _triangle(ctx->triangle.valueMin, ctx->triangle.valueMax, ctx->triangle.period, ctx->curr_time);
        break;
    }

    case SQUARE:
    {
        ctx->curr_time += Ts;
        if (ctx->curr_time >= ctx->square.period) {
            ctx->curr_time = 0.0;
        }

        value = _square(ctx->square.valueMin, ctx->square.valueMax, ctx->square.period, ctx->curr_time);
        break;
    }

    case NONE:
    default:
        // Injection function not set by user,
        // so don't do anything to the output signal
        return;
    }

    // Perform operation to do injection
    switch (ctx->operation) {
    case ADD:
        *output += value;
        break;
    case SUB:
        *output -= value;
        break;
    case SET:
        *output = value;
        break;
    }
}

inj_ctx_t *injection_find_ctx_by_name(char *name)
{
    inj_ctx_t *curr = inj_ctxs;

    // Try to find entry which is "name"
    while (curr != NULL) {
        if (strcmp(name, curr->name) == 0) {
            // Found it!
            return curr;
        }

        // Move to next entry in linked list
        curr = curr->next;
    }

    return NULL;
}

void injection_ctx_clear(inj_ctx_t *inj_ctx)
{
    inj_ctx->inj_func = NONE;
}

void injection_clear(void)
{
    inj_ctx_t *curr = inj_ctxs;

    // Operate on all registered contexts
    while (curr != NULL) {
        // Disable injection function
        curr->inj_func = NONE;

        // Move to next entry in linked list
        curr = curr->next;
    }
}

void injection_const(inj_ctx_t *ctx, inj_op_e op, double value)
{
    ctx->inj_func = CONST;
    ctx->operation = op;
    ctx->curr_time = 0.0;
    ctx->constant.value = value;
}

void injection_noise(inj_ctx_t *ctx, inj_op_e op, double gain, double offset)
{
    ctx->inj_func = NOISE;
    ctx->operation = op;
    ctx->curr_time = 0.0;
    ctx->noise.gain = gain;
    ctx->noise.offset = offset;
}

void injection_chirp(inj_ctx_t *ctx, inj_op_e op, double gain, double freqMin, double freqMax, double period)
{
    ctx->inj_func = CHIRP;
    ctx->operation = op;
    ctx->curr_time = 0.0;
    ctx->chirp.gain = gain;
    ctx->chirp.freqMin = freqMin;
    ctx->chirp.freqMax = freqMax;
    ctx->chirp.period = period;
}

void injection_triangle(inj_ctx_t *ctx, inj_op_e op, double valueMin, double valueMax, double period)
{
    ctx->inj_func = TRIANGLE;
    ctx->operation = op;
    ctx->curr_time = 0.0;
    ctx->triangle.valueMin = valueMin;
    ctx->triangle.valueMax = valueMax;
    ctx->triangle.period = period;
}

void injection_square(inj_ctx_t *ctx, inj_op_e op, double valueMin, double valueMax, double period)
{
    ctx->inj_func = SQUARE;
    ctx->operation = op;
    ctx->curr_time = 0.0;
    ctx->square.valueMin = valueMin;
    ctx->square.valueMax = valueMax;
    ctx->square.period = period;
}

// ***************************
// Code for running the state machine to
// list the registered injection contexts
// to the UART
// ***************************

typedef enum sm_states_list_e { LISTING = 1, REMOVE_TASK } sm_states_list_e;

typedef struct sm_ctx_list_t {
    sm_states_list_e state;
    task_control_block_t tcb;

    inj_ctx_t *curr;
} sm_ctx_list_t;

#define SM_LIST_UPDATES_PER_SEC (10000)
#define SM_LIST_INTERVAL_USEC   (USEC_IN_SEC / SM_LIST_UPDATES_PER_SEC)

void state_machine_list_callback(void *arg)
{
    sm_ctx_list_t *ctx = (sm_ctx_list_t *) arg;

    switch (ctx->state) {
    case LISTING:
        // Print entry
        cmd_resp_printf("%s\r\n", ctx->curr->name);

        // Move to next entry
        ctx->curr = ctx->curr->next;
        if (ctx->curr == NULL)
            ctx->state = REMOVE_TASK;
        break;

    case REMOVE_TASK:
        cmd_resp_printf("SUCCESS\r\n\n");
        scheduler_tcb_unregister(&ctx->tcb);
        break;

    default:
        // Can't happen
        HANG;
        break;
    }
}

static sm_ctx_list_t ctx_list;

void injection_list(void)
{
    if (inj_ctxs == NULL) {
        // Don't try to print empty list!
        return;
    }

    // Initialize the state machine context
    ctx_list.state = LISTING;
    ctx_list.curr = inj_ctxs;

    // Initialize the state machine callback tcb
    scheduler_tcb_init(&ctx_list.tcb, state_machine_list_callback, &ctx_list, "inj_list", SM_LIST_INTERVAL_USEC);
    scheduler_tcb_register(&ctx_list.tcb);
}

#endif // USER_CONFIG_ENABLE_INJECTION
