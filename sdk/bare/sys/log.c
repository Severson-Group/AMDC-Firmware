#include "log.h"
#include "debug.h"
#include "defines.h"
#include "scheduler.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define LOG_BUFFER_LENGTH	(LOG_VARIABLE_SAMPLE_DEPTH * sizeof(buffer_entry_t))

#define LOG_VAR_NAME_MAX_CHARS	(16)

typedef struct buffer_entry_t {
	uint32_t timestamp;
	uint32_t value;
} buffer_entry_t;

typedef struct log_var_t {
	char name[LOG_VAR_NAME_MAX_CHARS];
	void *addr;
	var_type_e type;

	uint32_t log_interval_usec;
	uint64_t last_logged_usec;

	int num_samples;
	buffer_entry_t buffer[LOG_BUFFER_LENGTH];
	int buffer_idx;
} log_var_t;

static log_var_t vars[LOG_MAX_NUM_VARS] = {0};

static uint8_t log_running;

static task_control_block_t tcb;


void log_init(void)
{
	// Register task which samples variables etc
	// NOTE: this runs at the base scheduler time quantum,
	//       or as fast as possible!
	scheduler_tcb_init(&tcb, log_callback, NULL, SYS_TICK_USEC);
	scheduler_tcb_register(&tcb);

	// Initialize all the variables to NULL address,
	// which indicates they aren't active
	for (int i = 0; i < LOG_MAX_NUM_VARS; i++) {
		vars[i].addr = NULL;
	}

	// Start with logging disabled
	log_stop();
}

void log_callback(void *arg)
{
	if (log_running == 0) {
		return;
	}

	for (uint8_t i = 0; i < LOG_MAX_NUM_VARS; i++) {
		log_var_t *v = &vars[i];

		if (v->addr == NULL) {
			// Variable not active for logging, so skip
			continue;
		}

		uint64_t elapsed_usec = scheduler_get_elapsed_usec();
		uint64_t usec_since_last_run = elapsed_usec - v->last_logged_usec;

		if (usec_since_last_run >= v->log_interval_usec) {
			// Time to log this variable!
			v->last_logged_usec = elapsed_usec;

			v->buffer[v->buffer_idx].timestamp = (uint32_t) elapsed_usec;

			if (v->type == INT) {
				v->buffer[v->buffer_idx].value = *((uint32_t *)v->addr);
			} else if (v->type == FLOAT) {
				float *f = (float *) &(v->buffer[v->buffer_idx].value);
				*f = *((float *)v->addr);
			} else if (v->type == DOUBLE) {
				float *f = (float *) &(v->buffer[v->buffer_idx].value);
				double value = *((double *)v->addr);
				*f = (float) value;
			}

			v->buffer_idx++;
			if (v->buffer_idx >= LOG_BUFFER_LENGTH) {
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
	log_running = 1;
}

void log_stop(void)
{
	log_running = 0;
}

uint8_t log_is_logging(void)
{
	return log_running;
}

void log_var_register(int idx, char* name, void *addr, uint32_t samples_per_sec, var_type_e type)
{
	// Sanity check variable idx
	if (idx < 0 || idx >= LOG_MAX_NUM_VARS) { HANG; }

	// Populate variable entry...
	strncpy(vars[idx].name, name, LOG_VAR_NAME_MAX_CHARS);
	vars[idx].addr = addr;
	vars[idx].type = type;

	// Calculate 'log_interval_usec' from samples per second
	vars[idx].log_interval_usec = USEC_IN_SEC / samples_per_sec;
	vars[idx].last_logged_usec = 0;
}

void log_var_empty(int idx)
{
	vars[idx].buffer_idx = 0;
	vars[idx].last_logged_usec = 0;
	vars[idx].num_samples = 0;
	memset(vars[idx].buffer, 0, LOG_BUFFER_LENGTH);
}







typedef enum sm_states_e {
	TITLE = 1,
	NUM_SAMPLES,
	HEADER,
	VARIABLES,
	FOOTER,
	REMOVE_TASK
} sm_states_e;

typedef struct sm_ctx_t {
	sm_states_e state;
	int var_idx;
	int sample_idx;
	task_control_block_t tcb;
} sm_ctx_t;

#define MSG_LENGTH		(128)

#define SM_UPDATES_PER_SEC		(10000)
#define SM_INTERVAL_USEC		(USEC_IN_SEC / SM_UPDATES_PER_SEC)

void state_machine(sm_ctx_t *ctx)
{
	char msg[MSG_LENGTH];
	log_var_t *v = &vars[ctx->var_idx];
	buffer_entry_t *e = &v->buffer[ctx->sample_idx];

	switch (ctx->state) {
	case TITLE:
		memset(msg, 0, MSG_LENGTH);
		snprintf(msg, MSG_LENGTH, "LOG OF VARIABLE: '%s'\r\n", v->name);
		debug_print(msg);

		ctx->state = NUM_SAMPLES;
		break;

	case NUM_SAMPLES:
		memset(msg, 0, MSG_LENGTH);
		snprintf(msg, MSG_LENGTH, "NUM SAMPLES: %d\r\n", v->num_samples);
		debug_print(msg);

		ctx->state = HEADER;
		break;

	case HEADER:
		debug_print("-------\r\n");
		debug_print("IDX\t\tTS\t\tVALUE\r\n");

		ctx->state = VARIABLES;
		break;

	case VARIABLES:
		memset(msg, 0, MSG_LENGTH);

		if (v->type == INT) {
			snprintf(msg, MSG_LENGTH, "%d\t\t%ld\t\t%ld\r\n", ctx->sample_idx, e->timestamp, e->value);
		} else if (v->type == FLOAT || v->type == DOUBLE) {
			float *f = (float *) &(e->value);
			snprintf(msg, MSG_LENGTH, "%d\t\t%ld\t\t%f\r\n", ctx->sample_idx, e->timestamp, *f);
		}

		debug_print(msg);

		ctx->sample_idx++;

		if (ctx->sample_idx >= LOG_VARIABLE_SAMPLE_DEPTH) {
			ctx->state = FOOTER;
		}
		break;

	case FOOTER:
		debug_print("-------\r\n");
		debug_print("\r\n");

		ctx->state = REMOVE_TASK;
		break;

	case REMOVE_TASK:
		scheduler_tcb_unregister(&ctx->tcb);
		break;

	default:
		// Can't happen
		HANG;
		break;
	}
}

static sm_ctx_t ctx;


void state_machine_callback(void *arg)
{
	state_machine(&ctx);
}

void log_var_dump_uart(int log_var_idx)
{
	// Initialize the state machine context
	ctx.state = TITLE;
	ctx.var_idx = log_var_idx;
	ctx.sample_idx = 0;

	// Initialize the state machine callback tcb
	scheduler_tcb_init(&ctx.tcb, state_machine_callback, NULL, SM_INTERVAL_USEC);
	scheduler_tcb_register(&ctx.tcb);
}
