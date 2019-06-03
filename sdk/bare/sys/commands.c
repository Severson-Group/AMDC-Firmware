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

#define BUFFER_LENGTH	(512)
static char recv_buffer[BUFFER_LENGTH] = {0};
static int recv_buffer_idx = 0;

#define CMD_MAX_ARGS	(16)
static char *cmd_argv[CMD_MAX_ARGS];
static int cmd_argc = 0;
static int _parse_cmd(void);

static void _show_help(void);
static int _command_handler(char **argv, int argc);

// Internal functions for each command
static int cmd_log(char **argv, int argc);
static int cmd_cc(char **argv, int argc);
static int cmd_mc(char **argv, int argc);
static int cmd_enc(char **argv, int argc);

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
			memset(&recv_buffer[0], 0, BUFFER_LENGTH);
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



//
// Handles the 'enc' command
// and all sub-commands
//
static int cmd_enc(char **argv, int argc)
{
	char msg[128];

	// Handle 'steps' sub-command
	if (strcmp("steps", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		int32_t steps;
		encoder_get_steps(&steps);

		snprintf(msg, 128, "steps: %ld\r\n", steps);
		debug_print(msg);

		return SUCCESS;
	}

	// Handle 'pos' sub-command
	if (strcmp("pos", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		uint32_t position;
		encoder_get_position(&position);

		snprintf(msg, 128, "pos: %ld\r\n", position);
		debug_print(msg);

		return SUCCESS;
	}

	return INVALID_ARGUMENTS;
}

//
// Handles the 'cc' command
// and all sub-commands
//
static int cmd_cc(char **argv, int argc)
{
	// Handle 'init' sub-command
	if (strcmp("init", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure mc task was not already inited
		if (task_cc_is_inited()) return FAILURE;

		task_cc_init();
		return SUCCESS;
	}

	// Handle 'deinit' sub-command
	if (strcmp("deinit", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure mc task was already inited
		if (!task_cc_is_inited()) return FAILURE;

		task_cc_deinit();
		task_cc_set_Id_star(0.0);
		task_cc_set_Iq_star(0.0);
		return SUCCESS;
	}

	// Handle 'Iq*' sub-command
	if (strcmp("Iq*", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		// Pull out iq argument
		double Iq_star = (double) atoi(argv[2]);
		Iq_star /= 1000.0;

		task_cc_set_Iq_star(Iq_star);
		return SUCCESS;
	}

	// Handle 'Id*' sub-command
	if (strcmp("Id*", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		// Pull out iq argument
		double Id_star = (double) atoi(argv[2]);
		Id_star /= 1000.0;

		task_cc_set_Id_star(Id_star);
		return SUCCESS;
	}

	// Handle 'offset' sub-command
	if (strcmp("offset", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		// Pull out offset argument
		int32_t offset = atoi(argv[2]);

		task_cc_set_dq_offset(offset);
		return SUCCESS;
	}

	return INVALID_ARGUMENTS;
}

//
// Handles the 'mc' command
// and all sub-commands
//
static int cmd_mc(char **argv, int argc)
{
	// Handle 'init' sub-command
	if (strcmp("init", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure mc task was not already inited
		if (task_mc_is_inited()) return FAILURE;

		task_mc_init();
		return SUCCESS;
	}

	// Handle 'deinit' sub-command
	if (strcmp("deinit", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure mc task was already inited
		if (!task_mc_is_inited()) return FAILURE;

		task_mc_deinit();
		task_mc_set_omega_star(0.0);
		task_cc_set_Id_star(0.0);
		task_cc_set_Iq_star(0.0);
		return SUCCESS;
	}

	// Handle 'rpm' sub-command
	if (strcmp("rpm", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		// Pull out rpm argument
		double rpms = (double) atoi(argv[2]);

		// Saturate commanded speed to +/- 600 RPM
		if (rpms >  600.0) rpms =  600.0;
		if (rpms < -600.0) rpms = -600.0;

		double rad_per_sec = rpms * PI / 30.0;

		task_mc_set_omega_star(rad_per_sec);
		return SUCCESS;
	}

	return INVALID_ARGUMENTS;
}

//
// Handles the 'log' command
// and all sub-commands
//
static int cmd_log(char **argv, int argc)
{
	// Handle 'reg' sub-command
	if (strcmp("reg", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 7) return INVALID_ARGUMENTS;

		// Parse arg1: log_var_idx
		int log_var_idx = atoi(argv[2]);
		if (log_var_idx >= LOG_MAX_NUM_VARS || log_var_idx < 0) {
			// ERROR
			return INVALID_ARGUMENTS;
		}

		// Parse arg2: name
		char *name = argv[3];

		// Parse arg3: memory_addr
		void *memory_addr = (void *) atoi(argv[4]);

		// Parse arg4: samples_per_sec
		int samples_per_sec = atoi(argv[5]);
		if (samples_per_sec > 1000 || samples_per_sec <= 0) {
			// ERROR
			return INVALID_ARGUMENTS;
		}

		// Parse arg5: type
		var_type_e type;
		if (strcmp("int", argv[6]) == 0) {
			type = INT;
		} else if (strcmp("float", argv[6]) == 0) {
			type = FLOAT;
		} else if (strcmp("double", argv[6]) == 0) {
			type = DOUBLE;
		} else {
			// ERROR
			return INVALID_ARGUMENTS;
		}

		// Register the variable with the logging engine
		log_var_register(log_var_idx, name, memory_addr, samples_per_sec, type);
		return SUCCESS;
	}

	// Handle 'start' sub-command
	if (strcmp("start", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure log was stopped before this
		if (log_is_logging()) return FAILURE;

		log_start();
		return SUCCESS;
	}

	// Handle 'stop' sub-command
	if (strcmp("stop", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure log was running before this
		if (!log_is_logging()) return FAILURE;

		log_stop();
		return SUCCESS;
	}

	// Handle 'dump' sub-command
	if (strcmp("dump", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		// Ensure logging was stopped before this
		if (log_is_logging()) return FAILURE;

		// Parse arg1: log_var_idx
		int log_var_idx = atoi(argv[2]);
		if (log_var_idx >= LOG_MAX_NUM_VARS || log_var_idx < 0) {
			// ERROR
			return INVALID_ARGUMENTS;
		}

		log_var_dump_uart(log_var_idx);
		return SUCCESS;
	}

	// Handle 'empty' sub-command
	if (strcmp("empty", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		// Parse arg1: log_var_idx
		int log_var_idx = atoi(argv[2]);
		if (log_var_idx >= LOG_MAX_NUM_VARS || log_var_idx < 0) {
			// ERROR
			return INVALID_ARGUMENTS;
		}

		log_var_empty(log_var_idx);
		return SUCCESS;
	}

	return INVALID_ARGUMENTS;
}
