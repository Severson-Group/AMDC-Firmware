#ifndef AMDS_H
#define AMDS_H

#include "drv/clock.h"
#include "drv/hardware_targets.h"
#include "usr/user_config.h"
#include <stdbool.h>
#include <stdint.h>

#include "xparameters.h"

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
#define AMDS_MAX_IP_CORES (XPAR_AMDC_AMDS_NUM_INSTANCES)
#define AMDS_1_BASE_ADDR  (XPAR_HIER_AMDS_AMDC_AMDS_0_S00_AXI_BASEADDR)
#endif

#if (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_F)
#define AMDS_MAX_IP_CORES (XPAR_AMDC_AMDS_NUM_INSTANCES)
#define AMDS_1_BASE_ADDR  (XPAR_HIER_GPIO_0_AMDC_AMDS_0_S00_AXI_BASEADDR)
#define AMDS_2_BASE_ADDR  (XPAR_HIER_GPIO_1_AMDC_AMDS_0_S00_AXI_BASEADDR)
#define AMDS_3_BASE_ADDR  (XPAR_HIER_GPIO_2_AMDC_AMDS_0_S00_AXI_BASEADDR)
#define AMDS_4_BASE_ADDR  (XPAR_HIER_GPIO_3_AMDC_AMDS_0_S00_AXI_BASEADDR)
#endif

// Offsets for relevant slave registers, to be added to one of the AMDS_N_BASE_ADDR
#define AMDS_CH_1_DATA_REG_OFFSET       (0)
#define AMDS_CH_2_DATA_REG_OFFSET       (1 * sizeof(uint32_t))
#define AMDS_CH_3_DATA_REG_OFFSET       (2 * sizeof(uint32_t))
#define AMDS_CH_4_DATA_REG_OFFSET       (3 * sizeof(uint32_t))
#define AMDS_CH_5_DATA_REG_OFFSET       (4 * sizeof(uint32_t))
#define AMDS_CH_6_DATA_REG_OFFSET       (5 * sizeof(uint32_t))
#define AMDS_CH_7_DATA_REG_OFFSET       (6 * sizeof(uint32_t))
#define AMDS_CH_8_DATA_REG_OFFSET       (7 * sizeof(uint32_t))
#define AMDS_DELAY_TIMER_REG_OFFSET     (8 * sizeof(uint32_t))
#define AMDS_CH_VALID_REG_OFFSET        (9 * sizeof(uint32_t))
#define AMDS_BYTES_VALID_REG_OFFSET     (10 * sizeof(uint32_t))
#define AMDS_BYTES_CORRUPT_REG_OFFSET   (11 * sizeof(uint32_t))
#define AMDS_BYTES_TIMED_OUT_REG_OFFSET (12 * sizeof(uint32_t))
#define AMDS_DATA_TIMED_OUT_REG_OFFSET  (13 * sizeof(uint32_t))

// Bit masks for use with amds_check_data_validity()
#define AMDS_CH_1_VALID_MASK 0x01
#define AMDS_CH_2_VALID_MASK 0x02
#define AMDS_CH_3_VALID_MASK 0x04
#define AMDS_CH_4_VALID_MASK 0x08
#define AMDS_CH_5_VALID_MASK 0x10
#define AMDS_CH_6_VALID_MASK 0x20
#define AMDS_CH_7_VALID_MASK 0x40
#define AMDS_CH_8_VALID_MASK 0x80

typedef enum {
    // Keep first channel index at 0!
    AMDS_CH_1 = 0,
    AMDS_CH_2,
    AMDS_CH_3,
    AMDS_CH_4,
    AMDS_CH_5,
    AMDS_CH_6,
    AMDS_CH_7,
    AMDS_CH_8,

    // Keep this as last entry!
    AMDS_NUM_CHANNELS,
} amds_channel_e;

static inline bool is_amds_channel_in_bounds(amds_channel_e channel)
{
    if (channel >= AMDS_CH_1 && channel < AMDS_NUM_CHANNELS) {
        return true;
    }

    return false;
}

static inline bool is_amds_port_in_bounds(int port)
{
    if (port >= 1 && port <= AMDS_MAX_IP_CORES) {
        return true;
    }

    return false;
}

static inline uint32_t amds_port_to_base_addr(int port)
{
    switch (port) {
    case 1:
        return AMDS_1_BASE_ADDR;

#if (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_F)
    case 2:
        return AMDS_2_BASE_ADDR;
    case 3:
        return AMDS_3_BASE_ADDR;
    case 4:
        return AMDS_4_BASE_ADDR;
#endif

    default:
        return 0;
    }
}

void amds_init(void);

uint8_t amds_check_data_validity(uint8_t port);
int amds_get_data(uint8_t port, amds_channel_e channel, int32_t *out);

void amds_print_data(uint8_t port);
void amds_print_counters(uint8_t port);
void amds_get_counters(uint8_t port, uint32_t *BV, uint32_t *BC, uint32_t *BT, uint32_t *DT);

int amds_get_trigger_to_edge_delay(uint8_t port, amds_channel_e channel, double *out);

#endif // AMDS_H
