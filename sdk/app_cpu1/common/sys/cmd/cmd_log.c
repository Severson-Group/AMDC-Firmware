#include "usr/user_config.h"

#if USER_CONFIG_ENABLE_LOGGING == 1

#include "cmd_log.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "sys/log.h"
#include "sys/util.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry;

static command_help_t cmd_help[] = {
    { "reg <log_var_idx> <name> <memory_addr> <samples_per_sec> <type>", "Register memory address for logging" },
    { "unreg <log_var_idx>", "Unregister variable slot" },
    { "start", "Start logging" },
    { "stop", "Stop logging" },
    { "dump <uart|eth> <bin|text> <log_var_idx>", "Dump log data to host" },
    { "empty <log_var_idx>", "Empty log for a previously logged variable (stays registered)" },
    { "empty_all", "Empty all slots" },
    { "info", "Print status of logging engine" },
    { "stream <start|stop> <log_var_idx> <socket_id>", "Stream variable to TCP socket" },
    { "stream synctime", "Resets stream timing to align output" },
};

void cmd_log_register(void)
{
    // Populate the command entry block
    commands_cmd_init(&cmd_entry, "log", "Logging engine commands", cmd_help, ARRAY_SIZE(cmd_help), cmd_log);

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
        if (log_var_idx >= LOG_MAX_NUM_VARIABLES || log_var_idx < 0) {
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
        if (argc != 3)
            return CMD_INVALID_ARGUMENTS;

        // Parse log_var_idx arg
        int log_var_idx = atoi(argv[2]);
        if (log_var_idx >= LOG_MAX_NUM_VARIABLES || log_var_idx < 0) {
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
        if (argc != 2)
            return CMD_INVALID_ARGUMENTS;

        // Make sure log was stopped before this
        if (log_is_logging())
            return CMD_FAILURE;

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
    if (argc == 5 && strcmp("dump", argv[1]) == 0) {
        // Ensure logging was stopped before this
        if (log_is_logging())
            return CMD_FAILURE;

        // Parse dump method
        int dump_method;
        if (STREQ("uart", argv[2])) {
            dump_method = 1;
        } else if (STREQ("eth", argv[2])) {
            dump_method = 2;
        } else {
            return CMD_INVALID_ARGUMENTS;
        }

        // Parse dump format
        int dump_format;
        if (STREQ("text", argv[3])) {
            dump_format = 1;
        } else if (STREQ("bin", argv[3])) {
            dump_format = 2;
        } else {
            return CMD_INVALID_ARGUMENTS;
        }

        // Parse log_var_idx
        int log_var_idx = atoi(argv[4]);
        if (log_var_idx >= LOG_MAX_NUM_VARIABLES || log_var_idx < 0) {
            // ERROR
            return CMD_INVALID_ARGUMENTS;
        }

        int err;
        if (dump_format == 1) {
            err = log_var_dump_ascii(log_var_idx, dump_method);
        } else {
            err = log_var_dump_binary(log_var_idx, dump_method);
        }

        if (err != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS_QUIET;
    }

    // Handle 'empty' sub-command
    if (strcmp("empty", argv[1]) == 0) {
        // Check correct number of arguments
        if (argc != 3)
            return CMD_INVALID_ARGUMENTS;

        // Parse arg1: log_var_idx
        int log_var_idx = atoi(argv[2]);
        if (log_var_idx >= LOG_MAX_NUM_VARIABLES || log_var_idx < 0) {
            // ERROR
            return CMD_INVALID_ARGUMENTS;
        }

        int err = log_var_empty(log_var_idx);
        if (err != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    // Handle 'empty_all' sub-command
    if (argc == 2 && strcmp("empty_all", argv[1]) == 0) {
        int err = log_var_empty_all();
        if (err != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    // Handle 'info' sub-command
    if (strcmp("info", argv[1]) == 0) {
        // Check correct number of arguments
        if (argc != 2)
            return CMD_INVALID_ARGUMENTS;

        int err = log_print_info();
        if (err != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS_QUIET;
    }

    // Handle 'stream' sub-command
    // log stream <start|stop> <log_var_idx> <socket_id>
    if (argc == 5 && STREQ("stream", argv[1])) {
        // Parse log_var_idx
        int log_var_idx = atoi(argv[3]);
        if (log_var_idx >= LOG_MAX_NUM_VARIABLES || log_var_idx < 0) {
            // ERROR
            return CMD_INVALID_ARGUMENTS;
        }

        // Parse socket_id
        int socket_id = atoi(argv[4]);
        if (socket_id < 0 || socket_id > 4) {
            return CMD_INVALID_ARGUMENTS;
        }

        int err = FAILURE;
        bool enable = false;
        if (STREQ("start", argv[2])) {
            enable = true;
        } else if (STREQ("stop", argv[2])) {
            enable = false;
        } else {
            return CMD_INVALID_ARGUMENTS;
        }

        err = log_stream(enable, log_var_idx, socket_id);

        if (err == SUCCESS) {
            return CMD_SUCCESS;
        } else {
            return CMD_FAILURE;
        }
    }

    if (argc == 3 && STREQ("stream", argv[1]) && STREQ("synctime", argv[2])) {
        log_stream_synctime();
        return CMD_SUCCESS;
    }

    return CMD_INVALID_ARGUMENTS;
}

#endif // USER_CONFIG_ENABLE_LOGGING
