#ifdef APP_CONTROLLER

#include "usr/controller/cmd/cmd_ctrl.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "sys/util.h"
#include "usr/controller/task_controller.h"
#include "drv/pwm.h"
#include <stdlib.h>
#include <string.h>
#include "drv/amds.h"
#include "xil_io.h"

// Stores command entry for command system module
static command_entry_t cmd_entry;

// Defines help content displayed for this command
// when user types "help" at command prompt
static command_help_t cmd_help[] = {
    { "init", "Start task" },
    { "deinit", "Stop task" },
    { "freq <freq>", "Set frequency of voltage output (rad/s)" },
    { "amplitude <amp>", "Set amplitude of voltage output (0 to 1)" },
	{ "print amds", "print raw AMDS channels" },
	{ "read <channel>", "read data out of channel" },
};

void cmd_ctrl_register(void)
{
    commands_cmd_init(&cmd_entry, "ctrl", "Controller commands",
                        cmd_help, ARRAY_SIZE(cmd_help), cmd_ctrl);
    commands_cmd_register(&cmd_entry);
}

int cmd_ctrl(int argc, char **argv)
{
    if (argc == 2 && STREQ("read", argv[1])) {
		uint32_t test = Xil_In32(AMDS_1_BASE_ADDR + AMDS_CH_VALID_REG_OFFSET);
		printf("\nValid = 0x%08lX\r\n", test);  // should be 0xDEADBEEF

    	return CMD_SUCCESS;
    }

    if (argc == 4 && STREQ("set", argv[1]) && STREQ("enable", argv[2])) {

    }

	if (argc == 2 && STREQ("init", argv[1])) {
        if (task_controller_init() != SUCCESS) {
            return CMD_FAILURE;
        }
        if (pwm_enable() != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    if (argc == 2 && STREQ("deinit", argv[1])) {
        if (task_controller_deinit() != SUCCESS) {
            return CMD_FAILURE;
        }
        if (pwm_disable() != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    if (argc == 3 && STREQ("freq", argv[1])) {
        double new_freq = strtod(argv[2], NULL);

        if (task_controller_set_frequency(new_freq) != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    if (argc == 3 && STREQ("amplitude", argv[1])) {
        double new_amplitude = strtod(argv[2], NULL);

        if (task_controller_set_amplitude(new_amplitude) != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    if (argc == 3 && STREQ("print", argv[1]) && STREQ("amds", argv[2])) {
    	task_controller_get_raw();
		return CMD_SUCCESS;
	}

    if (argc == 3 && STREQ("read", argv[1])) {
		double channel = strtod(argv[2], NULL);

		if (task_controller_get_data(channel) != SUCCESS) {
			return CMD_FAILURE;
		}

		return CMD_SUCCESS;
	}

    return CMD_INVALID_ARGUMENTS;
}

#endif // APP_CONTROLLER
