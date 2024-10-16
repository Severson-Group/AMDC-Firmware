#include "usr/user_config.h"

#if (USER_CONFIG_ENABLE_AMDS_SUPPORT == 1)

#include "drv/amds.h"
#include "sys/cmd/cmd_amds.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "sys/util.h"
#include "xil_io.h"
#include <stdbool.h>
#include <stdint.h>

void amds_init(void)
{
    xil_printf("AMDS: Initializing...\r\n");

    cmd_amds_register();
}

/* Returns a byte where each bit represents the validity of the most recently-returned data
 * for a single channel on this AMDC. bit[0] is for channel 1, bit[1] is for channel 2...
 * A '1' bit implies the data is valid, while a '0' bit implies invalid data for that channel
 * A return value of '255' implies all channels are valid
 * To check the validity of a single channel, mask the return value like so:
 *    uint8_t valid = amds_check_data_validity(port);
 *    if (valid & AMDS_CH_X_VALID_MASK != 0) {...};
 *
 * port: the GPIO port number the AMDS mainboard is connected to
 *
 * NOTE: Because the AMDS mainboard firmware does not do anything different for unpopulated
 *       SensorCards, the validity of those channels is meaningless
 */
uint8_t amds_check_data_validity(uint8_t port)
{
    uint32_t base_addr = amds_port_to_base_addr(port);

    if (base_addr == 0) {
        // This means an invalid port argument was passed, return 0 for all data invalid
        return 0;
    }

    return Xil_In8(base_addr + AMDS_CH_VALID_REG_OFFSET);
}

/* Retrieves the raw ADC data for a single channel on a single AMDS.
 *
 * port:      the GPIO port number the AMDS mainboard is connected to
 * channel:   AMDS_CH_N, where N is the channel (card number) whose data is of interest
 * out:       an int32_t pointer in which to place the retrieved data
 *
 * IMPORTANT: data placed in 'out' is NOT guaranteed to be valid. To check the validity of a
 *            channel's data, a separate call must be placed to amds_check_data_validity(),
 *            which reports the validity of all channels' data
 */
int amds_get_data(uint8_t port, amds_channel_e channel, int32_t *out)
{
    uint32_t base_addr = amds_port_to_base_addr(port);

    if (base_addr == 0) {
        // This means an invalid port argument was passed
        return FAILURE;
    }

    if (!is_amds_channel_in_bounds(channel)) {
        return FAILURE;
    } else {
        *out = (int32_t)(Xil_In32(base_addr + channel * sizeof(uint32_t)));
        return SUCCESS;
    }
}

void amds_print_data(uint8_t port)
{
    uint32_t base_addr = amds_port_to_base_addr(port);

    if (base_addr == 0) {
        // This means an invalid port argument was passed
        cmd_resp_printf("AMDS: Invalid Port Argument\r\n");
    } else {
        // Cast the address to a pointer for array-like access
        volatile uint32_t *arr_base_addr = (volatile uint32_t *) base_addr;

        for (int i = 0; i < 8; i++) {
            uint32_t val = arr_base_addr[i];
            cmd_resp_printf("CH_%i: %04X\r\n", i + 1, val);
        }
    }
}

void amds_print_counters(uint8_t port)
{
    uint32_t base_addr = amds_port_to_base_addr(port);

    if (base_addr == 0) {
        // This means an invalid port argument was passed
        cmd_resp_printf("AMDS: Invalid Port Argument\r\n");
    } else {
        cmd_resp_printf("Valid: %08X\r\n", Xil_In32(base_addr + AMDS_BYTES_VALID_REG_OFFSET));
        cmd_resp_printf("Corrupt: %08X\r\n", Xil_In32(base_addr + AMDS_BYTES_CORRUPT_REG_OFFSET));
        cmd_resp_printf("Timed out bytes: %08X\r\n", Xil_In32(base_addr + AMDS_BYTES_TIMED_OUT_REG_OFFSET));
        cmd_resp_printf("Timed out data: %08X\r\n", Xil_In32(base_addr + AMDS_DATA_TIMED_OUT_REG_OFFSET));
    }
}

/**
 * This function retrieves the values of the AMDS Driver Debug Counters for a given GPIO port
 * Counter values are returned via integer pointers, which are passed in as arguments:
 * - BV: Bytes Valid
 * - BC: Bytes Corrupt
 * - BT: Bytes Timed-Out
 * - DT: Data Timed-Out
 */
void amds_get_counters(uint8_t port, uint32_t *BV, uint32_t *BC, uint32_t *BT, uint32_t *DT)
{
    uint32_t base_addr = amds_port_to_base_addr(port);

    if (base_addr == 0) {
        // This means an invalid port argument was passed
        cmd_resp_printf("AMDS: Invalid Port Argument\r\n");
    } else {
        // Read valid bytes counter if user requested it
        if (BV != NULL) {
            *BV = Xil_In32(base_addr + AMDS_BYTES_VALID_REG_OFFSET);
        }

        // Read corrupt bytes counter if user requested it
        if (BC != NULL) {
            *BC = Xil_In32(base_addr + AMDS_BYTES_CORRUPT_REG_OFFSET);
        }

        // Read bytes timed out counter if user requested it
        if (BT != NULL) {
            *BT = Xil_In32(base_addr + AMDS_BYTES_TIMED_OUT_REG_OFFSET);
        }

        // Read data timed out counter if user requested it
        if (DT != NULL) {
            *DT = Xil_In32(base_addr + AMDS_DATA_TIMED_OUT_REG_OFFSET);
        }
    }
}

int amds_get_trigger_to_edge_delay(uint8_t port, amds_channel_e channel, double *out)
{
    uint32_t base_addr = amds_port_to_base_addr(port);

    if (base_addr == 0) {
        // This means an invalid port argument was passed
        return FAILURE;
    } else {
        // This register contains the FPGA cycle delay for both data line 0 and data line 1
        // Data line 0 is bits [15:0] and Data line 1 is bits [31:16]
        uint32_t delay_cycles_both_lines = Xil_In32(base_addr + AMDS_DELAY_TIMER_REG_OFFSET);

        if (channel >= AMDS_CH_1 && channel <= AMDS_CH_4) {
            // Delay time in us for data line 0
            *out = (double) (delay_cycles_both_lines & 0xFFFF) / CLOCK_FPGA_CLK_FREQ_MHZ;
            return SUCCESS;
        } else if (channel >= AMDS_CH_5 && channel <= AMDS_CH_8) {
            // Delay time in us for data line 1
            *out = (double) (delay_cycles_both_lines >> 16) / CLOCK_FPGA_CLK_FREQ_MHZ;
            return SUCCESS;
        } else {
            return FAILURE;
        }
    }
}

#endif // USER_CONFIG_ENABLE_AMDS_SUPPORT
