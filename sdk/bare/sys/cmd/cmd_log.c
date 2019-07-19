#include "../../usr/user_defines.h"

#ifndef DISABLE_LOGGING

#include "cmd_log.h"
#include "../commands.h"
#include "../defines.h"
#include "../log.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES	(5)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
    {"reg <log_var_idx> <name> <memory_addr> <samples_per_sec> <type>", "Register memory address for logging"},
    {"start", "Start logging"},
    {"stop", "Stop logging"},
    {"dump <log_var_idx>", "Dump log data to console"},
    {"empty <log_var_idx>", "Empty log for a previously logged variable (stays registered)"}
};

void cmd_log_register(void)
{
    // Populate the command entry block
    commands_cmd_init(&cmd_entry,
            "log", "Logging engine commands",
            cmd_help, NUM_HELP_ENTRIES,
            cmd_log
    );

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
        if (samples_per_sec > LOG_UPDATES_PER_SEC || samples_per_sec <= 0) {
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

#endif // DISABLE_LOGGING
