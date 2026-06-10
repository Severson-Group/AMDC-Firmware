#include "usr/user_config.h"

#if USER_CONFIG_ENABLE_AMDS_SUPPORT == 1

#include "drv/amds.h"
#include "sys/cmd/cmd_amds.h"
#include "sys/commands.h"
#include "sys/util.h"
#include "xil_io.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry;

static command_help_t cmd_help[] = {
    { "<port> data", "Display latest data from all AMDS channels" },
    { "<port> valid", "Display the validity of all channels' latest data" },
    { "<port> counters", "Display debug counters from AMDS UART RX" },
    { "<port> delay", "Display delay time in microseconds from trigger to UART activity on each data line" },
};

void cmd_amds_register(void)
{
    commands_cmd_init(&cmd_entry, "amds", "AMDS commands", cmd_help, ARRAY_SIZE(cmd_help), cmd_amds);

    commands_cmd_register(&cmd_entry);
}

int cmd_amds(int argc, char **argv)
{
    uint8_t port = 0;

    if (argc >= 2) {
        port = atoi(argv[1]);
        if (!is_amds_port_in_bounds(port)) {
            return CMD_INVALID_ARGUMENTS;
        }
    }

    // Handle 'amds <port> data' command
    if (argc == 3 && STREQ("data", argv[2])) {
        amds_print_data(port);
        return CMD_SUCCESS;
    }

    // Handle 'amds <port> valid' command
    if (argc == 3 && STREQ("valid", argv[2])) {
        uint32_t valid_bits = amds_check_data_validity(port);

        for (int ch = 0; ch < 24; ch++) {
            uint32_t mask = (1 << ch); // Shift 1 by 'ch' positions
            uint32_t enabled = amds_get_enabled(port);

            // Only print if the channel is enabled for this port
            if (enabled & mask) {
                if (valid_bits & mask) {
                    cmd_resp_printf("Channel %i:\tValid data\r\n", ch);
                } else {
                    cmd_resp_printf("Channel %i:\tInvalid data\r\n", ch);
                }
            }
        }

        return CMD_SUCCESS;
    }

    // Handle 'amds <port> counters' command
    if (argc == 3 && STREQ("counters", argv[2])) {
        amds_print_counters(port);
        return CMD_SUCCESS;
    }

    // Handle 'amds <port> delay' command
    if (argc == 3 && STREQ("delay", argv[2])) {
        double delay = 0;
        amds_get_trigger_to_edge_delay(port, AMDS_CH_1, &delay);
        cmd_resp_printf("Data Line 0 Delay: %.3fus\r\n", delay);
        amds_get_trigger_to_edge_delay(port, AMDS_CH_5, &delay);
        cmd_resp_printf("Data Line 1 Delay: %.3fus\r\n", delay);

        return CMD_SUCCESS;
    }

    return CMD_INVALID_ARGUMENTS;
}

#endif // USER_CONFIG_ENABLE_AMDS_SUPPORT
