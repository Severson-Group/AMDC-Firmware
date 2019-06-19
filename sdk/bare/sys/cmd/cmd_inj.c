#include "cmd_inj.h"
#include "../defines.h"
#include "../commands.h"

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES	(1)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
		{"foo" , "bar"}
};

void cmd_inj_register(void)
{
	// Populate the command entry block
	commands_cmd_init(&cmd_entry,
			"inj", "Inject signals into system",
			cmd_help, NUM_HELP_ENTRIES,
			cmd_inj
	);

	// Register the command
	commands_cmd_register(&cmd_entry);
}

int cmd_inj(int argc, char **argv)
{
	return SUCCESS;
}
