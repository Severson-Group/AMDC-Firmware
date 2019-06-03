#include "commands.h"
#include "debug.h"
#include "defines.h"
#include "log.h"
#include "scheduler.h"
#include "../usr/task_cc.h"
#include "../usr/task_mc.h"
#include "../drv/encoder.h"
#include "../drv/uart.h"
#include <string.h>
#include <stdlib.h>

#include "cmd/cmd_enc.h"
#include "cmd/cmd_log.h"
#include "../usr/cmd/cmd_cc.h"
#include "../usr/cmd/cmd_mc.h"

#define RECV_BUFFER_LENGTH	(512)
static char recv_buffer[RECV_BUFFER_LENGTH] = {0};
static int recv_buffer_idx = 0;

#define CMD_MAX_ARGS	(16)
static char *cmd_argv[CMD_MAX_ARGS];
static int cmd_argc = 0;
static int _parse_cmd(void);

static void _show_help(void);
static int _command_handler(char **argv, int argc);

typedef struct command_table_entry_t {
	char *cmd;
	char *desc;
	int (*cmd_function)(char**, int);
} command_table_entry_t;

#define NUM_COMMANDS	(4)
command_table_entry_t command_table[NUM_COMMANDS] = {
		{"log",	"Logging engine commands", cmd_log},
		{"cc",	"Current controller commands", cmd_cc},
		{"mc",	"Motion controller commands", cmd_mc},
		{"enc", "Encoder commands", cmd_enc}
};

typedef struct command_help_entry_t {
	int cmd_idx;
	char *subcmd;
	char *desc;
} command_help_entry_t;

#define NUM_HELP_ENTRIES	(15)
command_help_entry_t help_table[NUM_HELP_ENTRIES] = {
		{0, "reg <log_var_idx> <name> <memory_addr> <samples_per_sec> <type>", "Register memory address for logging"},
		{0, "start", "Start logging"},
		{0, "stop", "Stop logging"},
		{0, "dump <log_var_idx>", "Dump log data to console"},
		{0, "empty <log_var_idx>", "Empty log for a previously logged variable (stays registered)"},

		{1, "init", "Start current controller"},
		{1, "deinit", "Stop current controller"},
		{1, "Id* <milliamps>", "Command Id* to current controller"},
		{1, "Iq* <milliamps>", "Command Iq* to current controller"},
		{1, "offset <enc_pulses>", "Set DQ frame offset"},

		{2, "init", "Start motion controller"},
		{2, "deinit", "Stop motion controller"},
		{2, "rpm <rpms>", "Command speed to motion controller"},

		{3, "steps", "Read encoder steps from power-up"},
		{3, "pos", "Read encoder position"}
};

static task_control_block_t tcb;


void commands_init(void)
{
	printf("CMD:\tInitializing command task...\n");
	scheduler_tcb_init(&tcb, commands_callback, NULL, "command", COMMANDS_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);

	debug_print("\r\n");
	_show_help();
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

	for (int i = 0; i < NUM_COMMANDS; i++) {
		command_table_entry_t *c = &command_table[i];

		snprintf(msg, 128, "%s -- %s\r\n", c->cmd, c->desc);
		debug_print(msg);

		for (int j = 0; j < NUM_HELP_ENTRIES; j++) {
			command_help_entry_t *h = &help_table[j];

			if (h->cmd_idx == i) {
				snprintf(msg, 128, "\t%s -- %s\r\n", h->subcmd, h->desc);
				debug_print(msg);
			}
		}
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
	for (int i = 0; i < NUM_COMMANDS; i++) {
		if (strcmp(argv[0], command_table[i].cmd) == 0) {
			// Found command to run!
			return command_table[i].cmd_function(argv, argc);
		}
	}

	return UNKNOWN_CMD;
}
