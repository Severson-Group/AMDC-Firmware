#include "commands.h"
#include "debug.h"
#include "defines.h"
#include "log.h"
#include "scheduler.h"
#include "../drv/encoder.h"
#include "../drv/uart.h"
#include <string.h>
#include <stdlib.h>

#define RECV_BUFFER_LENGTH	(512)
static char recv_buffer[RECV_BUFFER_LENGTH] = {0};
static int recv_buffer_idx = 0;

#define CMD_MAX_ARGS	(16)
static char *cmd_argv[CMD_MAX_ARGS];
static int cmd_argc = 0;
static int _parse_cmd(void);

static void _show_help(void);
static int _command_handler(char **argv, int argc);

// Head of linked list of commands
command_entry_t *cmds = NULL;

static task_control_block_t tcb;

void commands_init(void)
{
	printf("CMD:\tInitializing command task...\n");
	scheduler_tcb_init(&tcb, commands_callback, NULL, "command", COMMANDS_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);
}

void commands_callback(void *arg)
{
	int err;
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
			if (recv_buffer_idx >= RECV_BUFFER_LENGTH) {
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
			err = _parse_cmd();

			if (err != 0) {
				// Too many arguments in cmd!
				debug_print("INVALID ARGUMENTS\r\n\n");
				return;
			} else {
				// Send cmd to application
				err = _command_handler(cmd_argv, cmd_argc);

				// Display command status to user
				switch (err) {
				case SUCCESS:
					debug_print("SUCCESS\r\n\n");
					break;

				case FAILURE:
					debug_print("FAILURE\r\n\n");
					break;

				case INVALID_ARGUMENTS:
					debug_print("INVALID ARGUMENTS\r\n\n");
					break;

				case UNKNOWN_CMD:
					debug_print("UNKNOWN CMD\r\n\n");

					// Couldn't find command to run, so display help screen
					_show_help();
					break;

				default:
					debug_print("UNKNOWN ERROR\r\n\n");
					break;
				}
			}

			// Clear state for next command
			recv_buffer_idx = 0;
			memset(&recv_buffer[0], 0, RECV_BUFFER_LENGTH);
			cmd_argc = 0;
			for (int i = 0; i < CMD_MAX_ARGS; i++) cmd_argv[i] = NULL;
		}
	}
}

void commands_cmd_init(command_entry_t *cmd_entry,
		const char *cmd, const char *desc,
		command_help_t *help, int num_help_cmds,
		int (*cmd_function)(char**, int)
)
{
	cmd_entry->cmd = cmd;
	cmd_entry->desc = desc;
	cmd_entry->help = help;
	cmd_entry->num_help_cmds = num_help_cmds;
	cmd_entry->cmd_function = cmd_function;
	cmd_entry->next = NULL;
}

void commands_cmd_register(command_entry_t *cmd_entry)
{
	// Base case: there are no tasks in linked list
	if (cmds == NULL) {
		cmds = cmd_entry;
		cmds->next = NULL;
		return;
	}

	// Find end of list
	command_entry_t *curr = cmds;
	while (curr->next != NULL) curr = curr->next;

	// Append new cmd to end of list
	curr->next = cmd_entry;
	cmd_entry->next = NULL;
}

void commands_start_msg(void)
{
	debug_print("\r\n");
	_show_help();
}

// _parse_cmd
//
// Populates the global `argv` and `argc` variables
// which are passed to the actual command
//
static int _parse_cmd(void)
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
				return -1;
			}
		}
	}

	return 0;
}

// _show_help
//
// Displays the help message to the console
//
static void _show_help(void)
{
	debug_print("Available commands:\r\n");
	debug_print("-------------------\r\n");

	char msg[128];

	command_entry_t *c = cmds;
	while (c != NULL) {
		snprintf(msg, 128, "%s -- %s\r\n", c->cmd, c->desc);
		debug_print(msg);

		for (int j = 0; j < c->num_help_cmds; j++) {
			command_help_t *h = &c->help[j];
			snprintf(msg, 128, "\t%s -- %s\r\n", h->subcmd, h->desc);
			debug_print(msg);
		}

		c = c->next;
	}

	debug_print("\r\n");
}

// _command_handler
//
// Takes `argv` and `argc` and finds
// the command function to call.
//
int _command_handler(char **argv, int argc)
{
	command_entry_t *c = cmds;

	while (c != NULL) {
		if (strcmp(argv[0], c->cmd) == 0) {
			// Found command to run!
			return c->cmd_function(argv, argc);
		}

		c = c->next;
	}

	return UNKNOWN_CMD;
}
