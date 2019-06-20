#ifdef APP_BETA_LABS

#include "cmd_mc.h"
#include "../task_mc.h"
#include "../../../sys/commands.h"
#include "../../../sys/defines.h"
#include <string.h>


static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES	(1)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
		{"foo", "bar"},
};

void cmd_mc_register(void)
{
	// Populate the command entry block
	commands_cmd_init(&cmd_entry,
			"mc", "Motion controller commands",
			cmd_help, NUM_HELP_ENTRIES,
			cmd_mc
	);

	// Register the command
	commands_cmd_register(&cmd_entry);
}

int cmd_mc(int argc, char **argv) {

	// Handle 'init' sub-command
	if (argc == 2 && strcmp("init", argv[1]) == 0) {
		// Make sure mc task was not already inited
		if (task_mc_is_inited()) return FAILURE;

		task_mc_init();
		return SUCCESS;
	}

	// Handle 'deinit' sub-command
	if (argc == 2 && strcmp("deinit", argv[1]) == 0) {
		// Make sure mc task was already inited
		if (!task_mc_is_inited()) return FAILURE;

		task_mc_deinit();
		return SUCCESS;
	}

	return INVALID_ARGUMENTS;
}

#endif // APP_BETA_LABS
