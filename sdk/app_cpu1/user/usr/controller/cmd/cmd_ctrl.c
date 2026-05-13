#ifdef APP_CONTROLLER

#include "usr/controller/cmd/cmd_ctrl.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "sys/util.h"
#include "usr/controller/task_controller.h"
#include "drv/pwm.h"
#include "drv/timing_manager.h"
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
	{ "get data", "Read sensor card data from 3 FBC boards" },
	{ "get timing", "Get the timing of sensor acquisition for AMDS1" },
};

void cmd_ctrl_register(void)
{
    commands_cmd_init(&cmd_entry, "ctrl", "Controller commands",
                        cmd_help, ARRAY_SIZE(cmd_help), cmd_ctrl);
    commands_cmd_register(&cmd_entry);
}

int cmd_ctrl(int argc, char **argv)
{
    if (argc == 3 && STREQ("read", argv[1]) && STREQ("enable", argv[2])) {
		uint32_t enable = Xil_In32(AMDS_1_BASE_ADDR + AMDS_CH_ENABLE_REG_OFFSET);
		cmd_resp_printf("\nEnable: 0x%08lX\r\n", enable);

    	return CMD_SUCCESS;
    }

    if (argc == 3 && STREQ("get", argv[1]) && STREQ("timing", argv[2])) {
		double timing = timing_manager_get_time_per_sensor(AMDS_1);
		cmd_resp_printf("\nAMDS 1 Timing: %f\r\n", timing);

		return CMD_SUCCESS;
	}

    if (argc == 3 && STREQ("set", argv[1]) && STREQ("enable", argv[2])) {
		uint32_t reg_addr = AMDS_1_BASE_ADDR + AMDS_CH_ENABLE_REG_OFFSET;
		uint32_t enable = 0x00FFFFFF;
		Xil_Out32(reg_addr, enable);  // enable 24 channels

		return CMD_SUCCESS;
    }

    // Handle 'get data' sub-command
	//
	if (argc == 3 && strcmp("get", argv[1]) == 0 && strcmp("data", argv[2]) == 0) {
		double out_current1, out_voltage1, out_current2, out_voltage2, out_current3, out_voltage3;
		if (task_controller_get_fbc_data(&out_current1, &out_voltage1, &out_current2, &out_voltage2, &out_current3, &out_voltage3) != SUCCESS) {
			return CMD_FAILURE;
		}
		cmd_resp_printf("\nFBC 1 Current Sensor Reading: %04f\r\n", out_current1);
		cmd_resp_printf("FBC 1 High Voltage Sensor Reading: %04f\r\n", out_voltage1);
		cmd_resp_printf("\nFBC 2 Current Sensor Reading: %04f\r\n", out_current2);
		cmd_resp_printf("FBC 2 High Voltage Sensor Reading: %04f\r\n", out_voltage2);
		cmd_resp_printf("\nFBC 3 Current Sensor Reading: %04f\r\n", out_current3);
		cmd_resp_printf("FBC 3 High Voltage Sensor Reading: %04f\r\n", out_voltage3);

		return CMD_SUCCESS;
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
