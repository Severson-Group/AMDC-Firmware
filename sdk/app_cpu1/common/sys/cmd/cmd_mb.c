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
    { "<idx> adc <on|off>", "Controls motherboard ADC sampling" },
    { "<idx> tx", "Manually trigger motherboard to send latest ADC samples" },
    { "<idx> samples", "Display latest samples from MB" },
    { "<idx> counters", "Display debug counters from MB UART RX" },
};

void cmd_mb_register(void)
{
    commands_cmd_init(&cmd_entry, "mb", "Motherboard commands", cmd_help, ARRAY_SIZE(cmd_help), cmd_mb);

    commands_cmd_register(&cmd_entry);
}

int cmd_mb(int argc, char **argv)
{
    uint32_t base_addr = 0;
    if (argc >= 2) {
        int idx = atoi(argv[1]);
        if (!motherboard_is_valid_idx(idx)) {
            return CMD_INVALID_ARGUMENTS;
        }

        base_addr = motherboard_idx_to_base_addr(idx);
    }

    // Handle 'mb <idx> adc <on|off>' command
    if (argc == 4 && STREQ("adc", argv[2])) {
        if (STREQ("on", argv[3])) {
            motherboard_set_adc_sampling(base_addr, true);
            return CMD_SUCCESS;
        }

        if (STREQ("off", argv[3])) {
            motherboard_set_adc_sampling(base_addr, false);
            return CMD_SUCCESS;
        }
    }

    // Handle 'mb <idx> tx' command
    if (argc == 3 && STREQ("tx", argv[2])) {
        motherboard_request_new_data(base_addr);
        return CMD_SUCCESS;
    }

    // Handle 'mb <idx> samples' command
    if (argc == 3 && STREQ("samples", argv[2])) {
        motherboard_print_samples(base_addr);
        return CMD_SUCCESS;
    }

    // Handle 'mb <idx> counters' command
    if (argc == 3 && STREQ("counters", argv[2])) {
        motherboard_print_counters(base_addr);
        return CMD_SUCCESS;
    }

    return CMD_INVALID_ARGUMENTS;
}

#endif // USER_CONFIG_ENABLE_MOTHERBOARD_SUPPORT
