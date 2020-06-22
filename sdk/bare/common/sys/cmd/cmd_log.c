#include "usr/user_defines.h"

#ifndef DISABLE_LOGGING

#include "cmd_log.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "sys/log.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES (7)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
    {"reg <log_var_idx> <name> <memory_addr> <samples_per_sec> <type>", "Register memory address for logging"},
    {"unreg <log_var_idx>", "Unregister variable slot"},
    {"start", "Start logging"},
    {"stop", "Stop logging"},
    {"dump <log_var_idx>", "Dump log data to console"},
    {"empty <log_var_idx>", "Empty log for a previously logged variable (stays registered)"},
	{"info", "Print status of logging engine"}
};

void cmd_log_register(void)
{
    // Populate the command entry block
    commands_cmd_init(&cmd_entry, "log", "Logging engine commands", cmd_help, NUM_HELP_ENTRIES, cmd_log);

    // Register the command
    commands_cmd_register(&cmd_entry);
}

//
// Handles the 'log' command
// and all sub-commands
//
int cmd_log(int argc, char **argv)
{
    // Handle 'reg' sub-command
    if (strcmp("reg", argv[1]) == 0) {
        // Check correct number of arguments
        if (argc != 7)
            return CMD_INVALID_ARGUMENTS;

        // Parse arg1: log_var_idx
        int log_var_idx = atoi(argv[2]);
        if (log_var_idx >= LOG_MAX_NUM_VARS || log_var_idx < 0) {
            // ERROR
            return CMD_INVALID_ARGUMENTS;
        }

        // Parse arg2: name
        char *name = argv[3];

        // Parse arg3: memory_addr
        void *memory_addr = (void *) atoi(argv[4]);

        // Parse arg4: samples_per_sec
        int samples_per_sec = atoi(argv[5]);
        if (samples_per_sec > LOG_UPDATES_PER_SEC || samples_per_sec <= 0) {
            // ERROR
            return CMD_INVALID_ARGUMENTS;
        }

        // Parse arg5: type
        var_type_e type;
        if (strcmp("int", argv[6]) == 0) {
            type = LOG_INT;
        } else if (strcmp("float", argv[6]) == 0) {
            type = LOG_FLOAT;
        } else if (strcmp("double", argv[6]) == 0) {
            type = LOG_DOUBLE;
        } else {
            // ERROR
            return CMD_INVALID_ARGUMENTS;
        }

        // Register the variable with the logging engine
        int err = log_var_register(log_var_idx, name, memory_addr, samples_per_sec, type);
        if (err != SUCCESS) {
        	return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

	// Handle 'unreg' sub-command
	if (strcmp("unreg", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return CMD_INVALID_ARGUMENTS;

		// Parse log_var_idx arg
		int log_var_idx = atoi(argv[2]);
		if (log_var_idx >= LOG_MAX_NUM_VARS || log_var_idx < 0) {
			// ERROR
			return CMD_INVALID_ARGUMENTS;
		}

		// Register the variable with the logging engine
		int err = log_var_unregister(log_var_idx);
		if (err != SUCCESS) {
			return CMD_FAILURE;
		}

		return CMD_SUCCESS;
	}

    // Handle 'start' sub-command
    if (strcmp("start", argv[1]) == 0) {
        // Check correct number of arguments
        if (argc != 2) return CMD_INVALID_ARGUMENTS;

        // Make sure log was stopped before this
        if (log_is_logging()) return CMD_FAILURE;

        log_start();
        return CMD_SUCCESS;
    }

    // Handle 'stop' sub-command
    if (strcmp("stop", argv[1]) == 0) {
        // Check correct number of arguments
        if (argc != 2)
            return CMD_INVALID_ARGUMENTS;

        // Make sure log was running before this
        if (!log_is_logging())
            return CMD_FAILURE;

        log_stop();
        return CMD_SUCCESS;
    }

    // Handle 'dump' sub-command
    if (strcmp("dump", argv[1]) == 0) {
        // Check correct number of arguments
        if (argc != 3)
            return CMD_INVALID_ARGUMENTS;

        // Ensure logging was stopped before this
        if (log_is_logging())
            return CMD_FAILURE;

        // Parse arg1: log_var_idx
        int log_var_idx = atoi(argv[2]);
        if (log_var_idx >= LOG_MAX_NUM_VARS || log_var_idx < 0) {
            // ERROR
            return CMD_INVALID_ARGUMENTS;
        }

        int err = log_var_dump_uart(log_var_idx);
        if (err != SUCCESS) {
        	return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    // Handle 'empty' sub-command
    if (strcmp("empty", argv[1]) == 0) {
        // Check correct number of arguments
        if (argc != 3)
            return CMD_INVALID_ARGUMENTS;

        // Parse arg1: log_var_idx
        int log_var_idx = atoi(argv[2]);
        if (log_var_idx >= LOG_MAX_NUM_VARS || log_var_idx < 0) {
            // ERROR
            return CMD_INVALID_ARGUMENTS;
        }

        int err = log_var_empty(log_var_idx);
        if (err != SUCCESS) {
        	return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    // Handle 'info' sub-command
    if (strcmp("info", argv[1]) == 0) {
        // Check correct number of arguments
        if (argc != 2) return CMD_INVALID_ARGUMENTS;

        int err = log_print_info();
        if (err != SUCCESS) {
        	return CMD_FAILURE;
        }

    	return CMD_SUCCESS;
    }

    return CMD_INVALID_ARGUMENTS;
}

#endif // DISABLE_LOGGING
