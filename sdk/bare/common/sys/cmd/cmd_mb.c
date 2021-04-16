#include "usr/user_config.h"

#if USER_CONFIG_ENABLE_MOTHERBOARD_SUPPORT == 1

#include "drv/motherboard.h"
#include "sys/cmd/cmd_mb.h"
#include "sys/commands.h"
#include "sys/util.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry;

static command_help_t cmd_help[] = {
    { "adc <on|off>", "Controls motherboard ADC sampling" },
    { "tx", "Manually trigger motherboard to send latest ADC samples" },
    { "samples", "Display latest samples from MB" },
    { "counters", "Display debug counters from MB UART RX" },
};

void cmd_mb_register(void)
{
    commands_cmd_init(&cmd_entry, "mb", "Motherboard commands", cmd_help, ARRAY_SIZE(cmd_help), cmd_mb);

    commands_cmd_register(&cmd_entry);
}

int cmd_mb(int argc, char **argv)
{
    // Handle 'mb adc <on|off>' command
    if (argc == 3 && STREQ("adc", argv[1])) {
        if (STREQ("on", argv[2])) {
            motherboard_set_adc_sampling(true);
            return CMD_SUCCESS;
        }

        if (STREQ("off", argv[2])) {
            motherboard_set_adc_sampling(false);
            return CMD_SUCCESS;
        }
    }

    // Handle 'mb tx' command
    if (argc == 2 && STREQ("tx", argv[1])) {
        motherboard_request_new_data();
        return CMD_SUCCESS;
    }

    // Handle 'mb samples' command
    if (argc == 2 && STREQ("samples", argv[1])) {
        motherboard_print_samples();
        return CMD_SUCCESS;
    }

    // Handle 'mb counters' command
    if (argc == 2 && STREQ("counters", argv[1])) {
        motherboard_print_counters();
        return CMD_SUCCESS;
    }

    return CMD_INVALID_ARGUMENTS;
}

#endif // USER_CONFIG_ENABLE_MOTHERBOARD_SUPPORT
