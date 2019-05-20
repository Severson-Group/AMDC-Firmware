#include "debug.h"
#include "scheduler.h"
#include <stdio.h>
#include "xil_printf.h"
#include "defines.h"


#define OUTPUT_BUFFER_LENGTH	(4096)

static char output_buffer[OUTPUT_BUFFER_LENGTH];
static int output_idx = 0;

// Used to mark position in output_buffer of char which needs to output
// -1 means no output needed
// >= 0 means print out that idx
static int print_idx = -1;

void debug_init(void)
{
	printf("DB:\tInitializing debug task...\n");
	scheduler_register_task(debug_callback, DEBUG_INTERVAL_USEC);

	// Clear output buffer
	for (int i = 0; i < OUTPUT_BUFFER_LENGTH; i++) {
		output_buffer[i] = 0;
	}
}

void debug_callback(void)
{
	if (print_idx != -1) {
		// Have work to do!

		char c = output_buffer[print_idx];
		outbyte(c);

		print_idx++;
		if (print_idx >= OUTPUT_BUFFER_LENGTH) {
			print_idx = 0;
		}

		// Check if done outputting data
		if (output_buffer[print_idx] == 0) {
			print_idx = -1;
		}
	}
}

void _append_to_output_buffer(char c)
{
	output_buffer[output_idx] = c;

	output_idx++;
	if (output_idx >= OUTPUT_BUFFER_LENGTH) {
		output_idx = 0;
	}
}

void debug_print(char *msg, int length)
{
	// Copy contents into circular output buffer

	print_idx = output_idx;

	for (int i = 0; i < length; i++) {
		_append_to_output_buffer(msg[i]);
	}

	// Add NULL termination
	_append_to_output_buffer(0);
}
