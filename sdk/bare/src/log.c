#include "log.h"
#include "scheduler.h"
#include "defines.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "debug.h"

#define LOG_BUFFER_LENGTH	(LOG_VARIABLE_SAMPLE_DEPTH * sizeof(buffer_entry_t))

typedef struct buffer_entry_t {
	uint32_t timestamp;
	uint32_t value;
} buffer_entry_t;

typedef struct log_var_t {
	char *name;
	void *addr;

	uint32_t log_interval_usec;
	uint64_t last_logged_usec;

	int num_samples;
	buffer_entry_t buffer[LOG_BUFFER_LENGTH];
	int buffer_idx;
} log_var_t;

static log_var_t vars[LOG_MAX_NUM_VARS] = {0};

static uint8_t log_running;

void log_init(void)
{
	// Register task which samples variables etc
	// NOTE: this runs at the base scheduler time quantum,
	//       or as fast as possible!
	scheduler_register_task(log_callback, SYS_TICK_USEC);

	// Initialize all the variables to NULL address,
	// which indicates they aren't active
	for (int i = 0; i < LOG_MAX_NUM_VARS; i++) {
		vars[i].addr = NULL;
	}

	// Start with logging disabled
	log_stop();
}

void log_callback(void)
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
			v->buffer[v->buffer_idx].value = *((uint32_t *)v->addr);

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

void log_var_register(int idx, char* name, void *addr, uint32_t samples_per_sec)
{
	// Sanity check variable idx
	if (idx < 0 || idx >= LOG_MAX_NUM_VARS) { HANG; }

	// Append task to task list
	vars[idx].name = name;
	vars[idx].addr = addr;

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

#define MSG_LENGTH		(128)

void log_var_dump_uart(int log_var_idx)
{
	char msg[MSG_LENGTH];

	log_var_t *v = &vars[log_var_idx];

	memset(msg, 0, MSG_LENGTH);
	snprintf(msg, MSG_LENGTH, "LOG OF '%s'\r\n", v->name);
	debug_print(msg);

	memset(msg, 0, MSG_LENGTH);
	snprintf(msg, MSG_LENGTH, "NUM SAMPLES: %d\r\n", v->num_samples);
	debug_print(msg);

	memset(msg, 0, MSG_LENGTH);
	snprintf(msg, MSG_LENGTH, "IDX\t\tTS\t\tVALUE\r\n");
	debug_print(msg);

	for (int i = 0; i < LOG_VARIABLE_SAMPLE_DEPTH; i++) {
		buffer_entry_t *e = &v->buffer[i];

		memset(msg, 0, MSG_LENGTH);
		snprintf(msg, MSG_LENGTH, "%d\t\t%ld\t\t%ld\r\n", i, e->timestamp, e->value);
		debug_print(msg);
	}

	debug_print("-------\r\n");
}
