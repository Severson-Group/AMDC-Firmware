#include "commands.h"
#include <string.h>
#include <stdlib.h>
#include "scheduler.h"
#include "debug.h"
#include "defines.h"
#include "log.h"
#include "../bsp/bsp.h"
#include "../user/task_mc.h"

#define BUFFER_LENGTH	(512)
static char recv_buffer[BUFFER_LENGTH] = {0};
static int recv_buffer_idx = 0;

#define CMD_MAX_ARGS	(16)
static char *cmd_argv[CMD_MAX_ARGS];
static int cmd_argc = 0;
static void _parse_cmd(void);

static void _show_help(void);
static int _command_handler(char **argv, int argc);

// Internal functions for each command
static int cmd_MC(char **argv, int argc);
static int cmd_LOGR(char **argv, int argc);
static int cmd_LOGS(char **argv, int argc);
static int cmd_LOGT(char **argv, int argc);
static int cmd_LOGD(char **argv, int argc);
static int cmd_LOGE(char **argv, int argc);

typedef struct command_table_entry_t {
	char *cmd;
	char *desc;
	int (*cmd_function)(char**, int);
} command_table_entry_t;

#define NUM_COMMANDS	(6)
command_table_entry_t command_table[NUM_COMMANDS] = {
		{"MC",		"Usage: 'MC <rpms>' -- Commands speed to motion controller", cmd_MC},
		{"LOGR", 	"Usage: 'LOGR <log_var_idx> <name> <memory_addr> <samples_per_sec>' -- Register memory address for logging", cmd_LOGR},
		{"LOGS", 	"Usage: 'LOGS' -- Start logging", cmd_LOGS},
		{"LOGT", 	"Usage: 'LOGT' -- Terminate logging", cmd_LOGT},
		{"LOGD", 	"Usage: 'LOGD <log_var_idx>' -- Dump log data to console", cmd_LOGD},
		{"LOGE", 	"Usage: 'LOGE <log_var_idx>' -- Empty log for a previously logged variable", cmd_LOGE}
};

static task_control_block_t tcb;


void commands_init(void)
{
	printf("CMD:\tInitializing command task...\n");
	scheduler_tcb_init(&tcb, commands_callback, COMMANDS_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);

	debug_print("\r\n");
	_show_help();
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
			int err = _command_handler(cmd_argv, cmd_argc);

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
	debug_print("Available commands:\r\n");
	debug_print("-------------------\r\n");

	char msg[128];
	for (int i = 0; i < NUM_COMMANDS; i++) {
		snprintf(msg, 128, "%s: %s\r\n", command_table[i].cmd, command_table[i].desc);
		debug_print(msg);
	}

	debug_print("\r\n");
}

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

//
// Usage: 'MC <rpms>' -- Commands speed to motion controller
//
static int cmd_MC(char **argv, int argc)
{
	// Check correct number of arguments
	if (argc != 2) {
		// ERROR
		return INVALID_ARGUMENTS;
	}

	int arg1 = atoi(argv[1]);

	double rpms = (double) arg1;

	// Saturate commanded speed to +/- 600 RPM
	if (rpms >  600.0) rpms =  600.0;
	if (rpms < -600.0) rpms = -600.0;

	double rad_per_sec = rpms * PI / 30.0;

	task_mc_set_omega_star(rad_per_sec);

	return SUCCESS;
}

//
// Usage: 'LOGR <log_var_idx> <name> <memory_addr> <samples_per_sec>' -- Register memory address for logging
//
static int cmd_LOGR(char **argv, int argc)
{
	// Check correct number of arguments
	if (argc != 5) {
		// ERROR
		return INVALID_ARGUMENTS;
	}

	// Parse arg1: log_var_idx
	int log_var_idx = atoi(argv[1]);
	if (log_var_idx >= LOG_MAX_NUM_VARS) {
		// ERROR
		return INVALID_ARGUMENTS;
	}

	// Parse arg2: name
	char *name = argv[2];

	// Parse arg3: memory_addr
	void *memory_addr = (void *) atoi(argv[3]);

	// Parse arg4: samples_per_sec
	int samples_per_sec = atoi(argv[4]);
	if (samples_per_sec > 1000 || samples_per_sec <= 0) {
		// ERROR
		return INVALID_ARGUMENTS;
	}

	// Register the variable with the logging engine
	log_var_register(log_var_idx, name, memory_addr, samples_per_sec);

	return SUCCESS;
}

//
// Usage: 'LOGS' -- Start logging
//
static int cmd_LOGS(char **argv, int argc)
{
	// Check correct number of arguments
	if (argc != 1) {
		// ERROR
		return INVALID_ARGUMENTS;
	}

	log_start();

	return SUCCESS;
}

//
// Usage: 'LOGT' -- Terminate logging
//
static int cmd_LOGT(char **argv, int argc)
{
	// Check correct number of arguments
	if (argc != 1) {
		// ERROR
		return INVALID_ARGUMENTS;
	}

	log_stop();

	return SUCCESS;
}

//
// Usage: 'LOGD <log_var_idx>' -- Dump log data to console
//
static int cmd_LOGD(char **argv, int argc)
{
	// Check correct number of arguments
	if (argc != 2) {
		// ERROR
		return INVALID_ARGUMENTS;
	}

	// Parse arg1: log_var_idx
	int log_var_idx = atoi(argv[1]);
	if (log_var_idx >= LOG_MAX_NUM_VARS) {
		// ERROR
		return INVALID_ARGUMENTS;
	}

	log_var_dump_uart(log_var_idx);

	return SUCCESS;
}

//
// Usage: 'LOGE <log_var_idx>' -- Empty log for a previously logged variable
//
static int cmd_LOGE(char **argv, int argc)
{
	// Check correct number of arguments
	if (argc != 2) {
		// ERROR
		return INVALID_ARGUMENTS;
	}

	// Parse arg1: log_var_idx
	int log_var_idx = atoi(argv[1]);
	if (log_var_idx >= LOG_MAX_NUM_VARS) {
		// ERROR
		return INVALID_ARGUMENTS;
	}

	log_var_empty(log_var_idx);

	return SUCCESS;
}
