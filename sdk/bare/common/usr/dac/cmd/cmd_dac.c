#ifdef APP_DAC

#include "usr/dac/cmd/cmd_dac.h"
#include "sys/commands.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/util.h"
#include "usr/dac/task_dac.h"
#include <stdlib.h>
#include <string.h>

// Stores command entry for command system module
static command_entry_t cmd_entry;

// Defines help content displayed for this command
// when user types "help" at command prompt
static command_help_t cmd_help[] = {
    { "init", "Start task" },
    { "deinit", "Stop task" },
    { "freq <frequency(Hz)>", "Set Frequency" },
    { "ch <channel> <voltage>", "Set channel to voltage" },
    { "reg <register> <value(in hex)>", "Write value to register" },
    { "trigger", "Trigger synchronous channels" },
    { "broadcast <voltage>", "Broadcast voltage" },
};

void cmd_dac_register(void)
{
    // Populate the command entry block
    //
    // Here is where you define the base command string: "dac"
    // and what function is called to handle command
    commands_cmd_init(&cmd_entry, "dac", "DAC application commands", cmd_help, ARRAY_SIZE(cmd_help), cmd_dac);

    // Register the command with the system
    commands_cmd_register(&cmd_entry);
}

int cmd_dac(int argc, char **argv)
{
    // This function is called every time the user types "dac"
    // followed by a space and any number of characters.

    // Handle 'init' sub-command
    //
    // First, verify correct number of arguments (2)
    // Second, verify second argument is "init"
    if (argc == 2 && strcmp("init", argv[1]) == 0) {
        if (task_dac_init() != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    // Handle 'deinit' sub-command
    //
    // First, verify correct number of arguments (2)
    // Second, verify second argument is "deinit"
    if (argc == 2 && strcmp("deinit", argv[1]) == 0) {
        if (task_dac_deinit() != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    // Handle 'freq' sub-command
    //
    // First, verify correct number of arguments (3)
    // Second, verify second argument is "freq"
    // Third, scan for a double
    if (argc == 3 && strcmp("freq", argv[1]) == 0) {
        double frequency;
        sscanf(argv[2], "%lf", &frequency);
        task_dac_frequency(frequency);
        return CMD_SUCCESS;
    }

    // Handle 'broadcast' sub-command
    //
    // First, verify correct number of arguments (3)
    // Second, verify second argument is "broadcast"
    // Third, scan for a double
    if (argc == 3 && strcmp("broadcast", argv[1]) == 0) {
        double voltage;
        sscanf(argv[2], "%lf", &voltage);

        if (abs(voltage) > 10)
            return CMD_INVALID_ARGUMENTS;

        task_dac_broadcast(voltage);
        return CMD_SUCCESS;
    }

    // Handle 'trigger' sub-command
    //
    // First, verify correct number of arguments (2)
    // Second, verify second argument is "trigger"
    if (argc == 2 && strcmp("trigger", argv[1]) == 0) {
        task_dac_trigger();
        return CMD_SUCCESS;
    }

    // Handle 'ch' sub-command
    //
    // First, verify correct number of arguments (4)
    // Second, verify second argument is "ch"
    // Third, scan for an int
    // Fourth, scan for a double
    if (argc == 4 && strcmp("ch", argv[1]) == 0) {
        int ch;
        sscanf(argv[2], "%i", &ch);

        if (ch < 0 || ch > 7)
            return CMD_INVALID_ARGUMENTS;

        double voltage;
        sscanf(argv[3], "%lf", &voltage);

        if (abs(voltage) > 10)
            return CMD_INVALID_ARGUMENTS;

        task_dac_set_voltage((uint8_t) ch, voltage);

        return CMD_SUCCESS;
    }

    // Handle 'reg' sub-command
    //
    // First, verify correct number of arguments (4)
    // Second, verify second argument is "reg"
    // Third, scan for an int
    // Fourth, scan for a hex string (uint32_t)
    if (argc == 4 && strcmp("reg", argv[1]) == 0) {
        int reg;
        sscanf(argv[2], "%i", &reg);

        if (reg < 0 || reg > 15)
            return CMD_INVALID_ARGUMENTS;

        uint32_t value;
        sscanf(argv[3], "%lX", &value);

        task_dac_set_reg((uint8_t) reg, value);

        return CMD_SUCCESS;
    }

    // At any point, if an error is detected in given input command,
    // simply return an error code (defined in sys/defines.h)
    //
    // The return statement below is used to catch all user input which
    // didn't match the if statements above. In general, to handle commands,
    // assume they are invalid. Only after checking if each argument is
    // valid should you trust them.
    //
    // Common error return values are: FAILURE, INVALID_ARGUMENTS
    return CMD_INVALID_ARGUMENTS;
}

#endif // APP_BLINK
