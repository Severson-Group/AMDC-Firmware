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

    // setup amds for default link configuration:
    // --> single amds (no daisy chain), all sensor cards enabled
    uint32_t mask = 0x000000FF;

    amds_set_enabled(0, mask);
    amds_set_enabled(1, mask);
    amds_set_enabled(2, mask);
    amds_set_enabled(3, mask);

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
uint32_t amds_check_data_validity(uint8_t port)
{
    uint32_t base_addr = amds_port_to_base_addr(port);

    if (base_addr == 0) {
        // This means an invalid port argument was passed, return 0 for all data invalid
        return 0;
    }

    return Xil_In32(base_addr + AMDS_CH_VALID_REG_OFFSET);
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

    uint32_t enabled = amds_get_enabled(port);

    if (!is_amds_channel_in_bounds(channel) || !(enabled & (1 << channel))) {
        return FAILURE;
    } else {
        *out = (int32_t) (Xil_In32(base_addr + channel * sizeof(uint32_t)));
        return SUCCESS;
    }
}

/* This function retrieves the raw voltage measured by the ADC for a given AMDS port, channel, and card type
 *
 * port:      the GPIO port number the AMDS mainboard is connected to
 * channel:   AMDS_CH_N, where N is the channel (card number) whose data is of interest
 * card:      AMDS_CARD_TYPE, from enum amds_card_t (low voltage, high voltage, current revb, current revc)
 * out:       a double pointer in which to place the retrieved data
 *
 * IMPORTANT: data placed in 'out' is NOT guaranteed to be valid. To check the validity of a
 *            channel's data, a separate call must be placed to amds_check_data_validity(),
 *            which reports the validity of all channels' data
 *
 */
int amds_get_voltage(uint8_t port, amds_channel_e channel, amds_card_t card, double *out)
{
    int32_t outInt = 0;
    int status = amds_get_data(port, channel, &outInt);
    switch (card) {
    case AMDS_LOW_VOLTAGE_CARD:
        *out = (4.096 / 32768) * outInt;
        break;
    case AMDS_HIGH_VOLTAGE_CARD:
        *out = (5.0 / 65536) * (outInt & 0x0000FFFF);
        break;
    case AMDS_CURRENT_CARD_REVB:
        *out = (5.0 / 65536) * (outInt & 0x0000FFFF);
        break;
    case AMDS_CURRENT_CARD:
        *out = (4.5 / 65536) * (outInt & 0x0000FFFF);
        break;
    default:
        return status | FAILURE;
    }
    return status | SUCCESS;
}

/* This function converts ADC voltage readings into a measurement of the sensed signal
 *
 * voltage:           a voltage sample from the AMDS obtained using amds_get_voltage()
 * offset:            a constant offset to be subtracted from raw voltage (user calibrated)
 * gain:              a gain factor to apply to measurement (user calibrated)
 * out:               a double pointer in which to place the retrieved data
 *
 * Default values for offset and gain for different AMDS card types can be found in the header file
 *
 * See
 * https://docs.amdc.dev/accessories/amds/sensor-cards/index.html
 * for detailed conversion information
 *
 */
int amds_convert_voltage(double voltage, double offset, double gain, double *out)
{
    *out = (voltage - offset) * gain;
    return SUCCESS;
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
        uint32_t enabled = amds_get_enabled(port);

        for (int i = 0; i < 24; i++) {
            if (!(enabled & (1 << i))) {
                continue;
            }
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

/**
 * This function retrieves the values of the AMDS Driver Channel Enable Register for a given GPIO port
 * The channel enable register is mapped with the MSb referring to channel 24 and the LSb for channel 1
 * For example: 0b100010001000100010001 (0x00111111) means channels 1, 5, 9, 13, 17, 21 are active, 
 * everything else is disabled.
 */
uint32_t amds_get_enabled(uint8_t port) {
    uint32_t base_addr = amds_port_to_base_addr(port);

    return Xil_In32(base_addr + AMDS_CH_ENABLE_REG_OFFSET);
}

/**
 * This function sets the values of the AMDS Driver Channel Enable Register for a given GPIO port
 * The channel enable register is mapped with the MSb referring to channel 24 and the LSb for channel 1
 * For example: 0b100010001000100010001 (0x00111111) means channels 1, 5, 9, 13, 17, 21 are active, 
 * everything else is disabled (this match active_sensor_mask in the AMDS firmware). 
 * This register is used to determine when all data has been received from the AMDS. This information is
 * needed to assert the sensor_done status to the timing manager. Is a channel is disabled, the AMDC does
 * not wait for that channel's data to arrive.
 */
void amds_set_enabled(uint8_t port, uint32_t mask) {
    uint32_t base_addr = amds_port_to_base_addr(port);

    Xil_Out32(base_addr + AMDS_CH_ENABLE_REG_OFFSET, mask);  // enable 24 channels
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

        if ((channel >= AMDS_CH_1 && channel <= AMDS_CH_4) ||
            (channel >= AMDS_CH_9 && channel <= AMDS_CH_12) ||
            (channel >= AMDS_CH_17 && channel <= AMDS_CH_20)) {
            // Delay time in us for data line 0
            *out = (double) (delay_cycles_both_lines & 0xFFFF) / CLOCK_FPGA_CLK_FREQ_MHZ;
            return SUCCESS;
        } else if ((channel >= AMDS_CH_5 && channel <= AMDS_CH_8) ||
                   (channel >= AMDS_CH_13 && channel <= AMDS_CH_16) ||
                   (channel >= AMDS_CH_21 && channel <= AMDS_CH_24)) {
            // Delay time in us for data line 1
            *out = (double) (delay_cycles_both_lines >> 16) / CLOCK_FPGA_CLK_FREQ_MHZ;
            return SUCCESS;
        } else {
            return FAILURE;
        }
    }
}

#endif // USER_CONFIG_ENABLE_AMDS_SUPPORT
