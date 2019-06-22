#include "../usr/user_defines.h"

#ifndef DISABLE_INJECTION

#include "injection.h"
#include "debug.h"
#include "scheduler.h"
#include "defines.h"
#include "cmd/cmd_inj.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Linked list of all registered injection contexts
static inj_ctx_t *inj_ctxs = NULL;

// Used to give each registered ctx a unique ID
static int next_ctx_id = 0;

// Chirp function
//
// Generates the chirp signal value given:
// - time: current time instant
// - w1:   low freq (rad)
// - w2:   high freq (rad)
// - A:    amplitude
// - M:    time period
static inline double _chirp(double w1, double w2, double A, double M, double time)
{
	double out;
	out = A * cos(w1 * time + (w2 - w1) * time * time / (2 * M));
	return out;
}


static inline double _ramp(double min, double max, double period, double time)
{
	double out;

	if (time <= period / 2.0) {
		// Ramp function is rising
		double t = time;

		double m = (max - min) / (period / 2.0);
		double b = min;
		out = m * t + b;
	} else {
		// Ramp function is falling
		double t = time - (period / 2.0);

		double m = (min - max) / (period / 2.0);
		double b = max;
		out = m * t + b;
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

void injection_ctx_register(inj_ctx_t *ctx)
{
	// Don't let clients re-register their ctx
	if (ctx->registered) {
		HANG;
	}

	// Mark as registered
	ctx->registered = 1;

	// Base case: there are no tasks in linked list
	if (inj_ctxs == NULL) {
		inj_ctxs = ctx;
		inj_ctxs->next = NULL;
		return;
	}

	// Find end of list
	inj_ctx_t *curr = inj_ctxs;
	while (curr->next != NULL) curr = curr->next;

	// Append new ctx to end of list
	curr->next = ctx;
	ctx->next = NULL;
}

void injection_ctx_unregister(inj_ctx_t *ctx)
{
	// Don't let clients unregister their already unregistered ctx
	if (!ctx->registered) {
		HANG;
	}

	// Mark as unregistered
	ctx->registered = 0;

	// Make sure list isn't empty
	if (inj_ctxs == NULL) {
		HANG;
	}

	// Special case: trying to remove the head of the list
	if (inj_ctxs->id == ctx->id) {
		inj_ctxs = inj_ctxs->next;
		return;
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
		r = (2.0  * r) - 1.0;

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
				PI2 * ctx->chirp.freqMin,
				PI2 * ctx->chirp.freqMax,
				ctx->chirp.gain,
				ctx->chirp.period,
				ctx->curr_time
				);
		break;
	}

	case RAMP:
	{
		ctx->curr_time += Ts;
		if (ctx->curr_time >= ctx->ramp.period) {
			ctx->curr_time = 0.0;
		}

		value = _ramp(
				ctx->ramp.valueMin,
				ctx->ramp.valueMax,
				ctx->ramp.period,
				ctx->curr_time
				);
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

void injection_clear(void) {
	inj_ctx_t *curr = inj_ctxs;

	// Operate on all registered contexts
	while (curr != NULL) {
		// Disable injection function
		curr->inj_func = NONE;

		// Move to next entry in linked list
		curr = curr->next;
	}
}

void injection_const(inj_ctx_t *ctx, inj_op_e op, double value) {
	ctx->inj_func = CONST;
	ctx->operation = op;
	ctx->curr_time = 0.0;
	ctx->constant.value = value;
}

void injection_noise(inj_ctx_t *ctx, inj_op_e op, double gain, double offset) {
	ctx->inj_func = NOISE;
	ctx->operation = op;
	ctx->curr_time = 0.0;
	ctx->noise.gain = gain;
	ctx->noise.offset = offset;
}

void injection_chirp(inj_ctx_t *ctx, inj_op_e op, double gain, double freqMin, double freqMax, double period) {
	ctx->inj_func = CHIRP;
	ctx->operation = op;
	ctx->curr_time = 0.0;
	ctx->chirp.gain = gain;
	ctx->chirp.freqMin = freqMin;
	ctx->chirp.freqMax = freqMax;
	ctx->chirp.period = period;
}

void injection_ramp(inj_ctx_t *ctx, inj_op_e op, double valueMin, double valueMax, double period)
{
	ctx->inj_func = RAMP;
	ctx->operation = op;
	ctx->curr_time = 0.0;
	ctx->ramp.valueMin = valueMin;
	ctx->ramp.valueMax = valueMax;
	ctx->ramp.period = period;
}



// ***************************
// Code for running the state machine to
// list the registered injection contexts
// to the UART
// ***************************

typedef enum sm_states_list_e {
	LISTING = 1,
	REMOVE_TASK
} sm_states_list_e;

typedef struct sm_ctx_list_t {
	sm_states_list_e state;
	task_control_block_t tcb;

	inj_ctx_t *curr;
} sm_ctx_list_t;


#define SM_LIST_UPDATES_PER_SEC		(10000)
#define SM_LIST_INTERVAL_USEC		(USEC_IN_SEC / SM_LIST_UPDATES_PER_SEC)

void state_machine_list_callback(void *arg)
{
	sm_ctx_list_t *ctx = (sm_ctx_list_t *) arg;

	switch (ctx->state) {
	case LISTING:
		// Print entry
		debug_printf("%s\r\n", ctx->curr->name);

		// Move to next entry
		ctx->curr = ctx->curr->next;
		if (ctx->curr == NULL) ctx->state = REMOVE_TASK;
		break;

	case REMOVE_TASK:
		debug_printf("\r\n");
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

#endif // DISABLE_INJECTION
