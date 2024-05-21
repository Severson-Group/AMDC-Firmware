#include "usr/user_config.h"

#if USER_CONFIG_ENABLE_AMDS_SUPPORT == 1

#include "drv/amds.h"
#include "sys/cmd/cmd_amds.h"
#include "sys/commands.h"
#include "sys/util.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry;

static command_help_t cmd_help[] = {
    { "<port> data", "Display latest data from all AMDS channels" },
    { "<port> valid", "Display the validity of all channels' latest data" },
    { "<port> counters", "Display debug counters from AMDS UART RX" },
};

void cmd_amds_register(void)
{
    commands_cmd_init(&cmd_entry, "amds", "AMDS commands", cmd_help, ARRAY_SIZE(cmd_help), cmd_amds);

    commands_cmd_register(&cmd_entry);
}

int cmd_amds(int argc, char **argv)
{
    uint32_t base_addr = 0;

    if (argc >= 2) {
        int port = atoi(argv[1]);
        if (!amds_port_in_bounds(port)) {
            return CMD_INVALID_ARGUMENTS;
        }

        base_addr = amds_port_to_base_addr(port);
    }

    // Handle 'amds <port> data' command
    if (argc == 3 && STREQ("data", argv[2])) {
        amds_print_data(base_addr);
        return CMD_SUCCESS;
    }

    // Handle 'amds <port> valid' command
    if (argc == 3 && STREQ("valid", argv[2])) {
        uint8_t valid_bits = amds_check_data_validity(base_addr);

        uint8_t mask = 0x01;
        uint8_t channel = 1;

        while (mask) {
            if (valid_bits & mask)
                cmd_resp_printf("Channel %i: Valid data\r\n", channel);
            else
                cmd_resp_printf("Channel %i: Invalid data\r\n", channel);

            mask = mask << 1;
            channel++;
        }

        return CMD_SUCCESS;
    }

    // Handle 'amds <port> counters' command
    if (argc == 3 && STREQ("counters", argv[2])) {
        amds_print_counters(base_addr);
        return CMD_SUCCESS;
    }

    return CMD_INVALID_ARGUMENTS;
}

#endif // USER_CONFIG_ENABLE_AMDS_SUPPORT
