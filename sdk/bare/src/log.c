#include "log.h"
#include "scheduler.h"
#include "defines.h"
#include <stdio.h>
#include <stdint.h>


#define LOG_BUFFER_LENGTH	(1024)

typedef struct buffer_entry_t {
	int var_idx;
	uint64_t value;
} buffer_entry_t;

static int buffer_idx = 0;
static buffer_entry_t buffer[LOG_BUFFER_LENGTH];

#define MAX_NUM_LOG_VARS (16)

typedef struct log_var_t {
	char *name;
	log_var_type_e type;
	uint64_t log_interval_usec;
	uint64_t last_logged_usec;
	void *addr;
} log_var_t;

static log_var_t vars[MAX_NUM_LOG_VARS];
static uint8_t num_vars = 0;

void log_init(void)
{
	// Register task which samples variables etc
	// NOTE: this runs at the base scheduler time quantum,
	//       or as fast as possible!
	scheduler_register_task(log_callback, SYS_TICK_USEC);
	scheduler_register_task(log_dump_callback, USEC_IN_SEC / 100);
}

void log_register_var(char* name, void *addr, log_var_type_e type, uint32_t interval_usec)
{
	// If we are full of tasks, die here
	if (num_vars >= MAX_NUM_LOG_VARS) { HANG; }

	// Append task to task list
	vars[num_vars].name = name;
	vars[num_vars].type = type;
	vars[num_vars].addr = addr;
	vars[num_vars].log_interval_usec = interval_usec;
	vars[num_vars].last_logged_usec = 0;

	num_vars++;
}

void log_callback(void)
{
	for (uint8_t i = 0; i < num_vars; i++) {
		log_var_t *v = &vars[i];

		uint64_t elapsed_usec = scheduler_get_elapsed_usec();
		uint64_t usec_since_last_run = elapsed_usec - v->last_logged_usec;

		if (usec_since_last_run >= v->log_interval_usec) {
			// Time to log this variable!

			buffer_entry_t entry;
			entry.var_idx = i;
			entry.value = *((uint64_t*)v->addr);

			buffer[buffer_idx] = entry;

			v->last_logged_usec = elapsed_usec;

			buffer_idx++;
			if (buffer_idx >= LOG_BUFFER_LENGTH) {
				buffer_idx = 0;
			}
		}
	}
}

int count = 0;
#define DUMP_EVERY_N_SECS	(10)
void log_dump_callback(void)
{
	count++;

	if (count >= DUMP_EVERY_N_SECS * 100) {
		count = 0;
		// Time to dump buffer

		log_dump_uart();
	}
}

void log_dump_uart(void)
{
	if (num_vars == 0) {
		return;
	}

	printf("-----------\n");

	for (int i = 0; i < LOG_BUFFER_LENGTH; i++) {
		buffer_entry_t *entry = &buffer[(buffer_idx + i) % LOG_BUFFER_LENGTH];

		log_var_t *log_var = &vars[entry->var_idx];

		switch (log_var->type) {
		case S_INTEGER:
			printf("LOG:\t%s\t%d\t%ld\n", log_var->name, i, (int32_t) entry->value);
			break;

		case U_INTEGER:
			printf("LOG:\t%s\t%d\t%lu\n", log_var->name, i, (uint32_t) entry->value);
			break;

		case FLOAT:
			printf("LOG:\t%s\t%d\t%f\n", log_var->name, i, (float) entry->value);
			break;

		case DOUBLE:
			printf("LOG:\t%s\t%d\t%f\n", log_var->name, i, (double) entry->value);
			break;

		default:
			// ERROR!
			break;
		}
	}
}
