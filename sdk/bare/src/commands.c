#include "commands.h"
#include "scheduler.h"
#include "../bsp/bsp.h"
#include "debug.h"
#include "defines.h"
#include <string.h>
#include <stdlib.h>
#include "task_mc.h"

#define BUFFER_LENGTH	(512)
static char recv_buffer[BUFFER_LENGTH] = {0};
static int recv_buffer_idx = 0;

#define CMD_MAX_ARGS	(16)
static char *cmd_argv[CMD_MAX_ARGS];
static int cmd_argc = 0;
static void _parse_cmd(void);

static void _show_help(void);

// Internal functions for each command
static int cmd_MC(char **argv, int argc);

typedef struct command_table_entry_t {
	char *cmd;
	char *desc;
	int (*cmd_function)(char**, int);
} command_table_entry_t;

#define NUM_COMMANDS	(1)
command_table_entry_t command_table[NUM_COMMANDS] = {
	{"MC",	"Usage: 'MC <rpms>' -- Commands speed to motion controller",	cmd_MC}
};


void commands_init(void)
{
	printf("CMD:\tInitializing command task...\n");
	scheduler_register_task(commands_callback, COMMANDS_INTERVAL_USEC);
}

void commands_callback(void)
{
	char c;
	int num_bytes = uart_recv(&c, 1);

	// If no user input, `num_bytes` == 0...
	if (num_bytes > 0) {

		// Don't include '\n' or '\r' in the command to the user
		if (c != '\r' && c != '\n') {
			// Append received char to buffer
			recv_buffer[recv_buffer_idx] = c;

			// Only accept so many characters per command...
			recv_buffer_idx++;
			if (recv_buffer_idx >= BUFFER_LENGTH) {
				HANG;
			}

			// Echo character back to terminal
			char cc[2];
			cc[0] = c;
			cc[1] = 0;
			debug_print(cc);
		}

		// Commands are delimited by the '\r' character

		if (c == '\r') {
			// End of the command!

			// Make console go to beginning of next line
			debug_print("\r\r\n");

			// Create data structures to send to application
			// i.e., the `argv` and `argc` variables
			_parse_cmd();

			// Send cmd to application
			command_handler(cmd_argv, cmd_argc);

			// Clear state for next command
			recv_buffer_idx = 0;
			memset(&recv_buffer[0], 0, BUFFER_LENGTH);
			cmd_argc = 0;
			for (int i = 0; i < CMD_MAX_ARGS; i++) cmd_argv[i] = NULL;
		}
	}
}

static void _parse_cmd(void)
{
	// First word is the first argv
	cmd_argv[0] = &recv_buffer[0];
	cmd_argc++;

	int cmd_char_len = strlen(recv_buffer);

	for (int i = 1; i < cmd_char_len; i++) {
		if (recv_buffer[i] == ' ') {
			// Replace spaces with NULLs
			recv_buffer[i] = 0;
		}

		// Look for the beginning of an argument:
		if (recv_buffer[i-1] == 0 && recv_buffer[i] != 0) {
			// We just ended one argument (and white space),
			// now sitting on the first char of the next one
			cmd_argv[cmd_argc++] = &recv_buffer[i];

			// Only allow so many arguments
			if (cmd_argc >= CMD_MAX_ARGS) {
				HANG;
			}
		}
	}
}

static void _show_help(void)
{
	debug_print("\r\n");
	debug_print("Available commands:\r\n");
	debug_print("-------------------\r\n");

	char msg[128];
	for (int i = 0; i < NUM_COMMANDS; i++) {
		snprintf(msg, 128, "%s: %s\r\n", command_table[i].cmd, command_table[i].desc);
		debug_print(msg);
	}

	debug_print("\r\n");
}

void command_handler(char **argv, int argc)
{
	for (int i = 0; i < NUM_COMMANDS; i++) {
		if (strcmp(argv[0], command_table[i].cmd) == 0) {
			// Found command to run!
			command_table[i].cmd_function(argv, argc);
			return;
		}
	}

	// Couldn't find command to run, so display help screen
	debug_print("ERROR: unrecognized command\r\n");
	_show_help();
}



int cmd_MC(char **argv, int argc)
{
	int arg1 = atoi(argv[1]);

	double rpms = (double) arg1;

	// Saturate commanded speed to 600 RPM
	if (rpms >  600.0) rpms =  600.0;
	if (rpms < -600.0) rpms = -600.0;

	double rad_per_sec = rpms * PI / 30.0;

	task_mc_set_omega_star(rad_per_sec);

	return SUCCESS;
}

