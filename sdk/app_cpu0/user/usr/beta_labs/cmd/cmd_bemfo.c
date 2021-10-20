#ifdef APP_BETA_LABS

#include "usr/beta_labs/cmd/cmd_bemfo.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "usr/beta_labs/bemfo.h"
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES (1)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
    { "start", "Reset controller state" },
};

void cmd_bemfo_register(void)
{
    // Populate the command entry block
    commands_cmd_init(&cmd_entry, "bemfo", "BEMFO commands", cmd_help, NUM_HELP_ENTRIES, cmd_bemfo);

    // Register the command
    commands_cmd_register(&cmd_entry);
}

int cmd_bemfo(int argc, char **argv)
{
    // Handle 'start' sub-command
    if (argc == 2 && strcmp("start", argv[1]) == 0) {
        bemfo_start();
        return CMD_SUCCESS;
    }

    return CMD_INVALID_ARGUMENTS;
}

#endif // APP_BETA_LABS
