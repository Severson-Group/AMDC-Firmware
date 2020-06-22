#include "cmd_help.h"
#include "sys/commands.h"
#include <stdint.h>
#include <string.h>

static command_entry_t cmd_entry;

void cmd_help_register(void)
{
    // Populate the command entry block
    commands_cmd_init(&cmd_entry, "help", "Display this help message", NULL, 0, cmd_help);

    // Register the command
    commands_cmd_register(&cmd_entry);
}

//
// Handles the 'help' command
//
int cmd_help(int argc, char **argv)
{
    if (argc > 1) {
        return CMD_INVALID_ARGUMENTS;
    }

    commands_display_help();

    return CMD_SUCCESS_QUIET;
}
