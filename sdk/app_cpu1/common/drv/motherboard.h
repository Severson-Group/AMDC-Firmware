#ifndef MOTHERBOARD_H
#define MOTHERBOARD_H

#include "drv/hardware_targets.h"
#include "usr/user_config.h"
#include <stdbool.h>
#include <stdint.h>

#include "xparameters.h"

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
#define MOTHERBOARD_MAX_IP_CORES (XPAR_AMDC_MOTHERBOARD_NUM_INSTANCES)
#define MOTHERBOARD_1_BASE_ADDR  (XPAR_HIER_AMDS_AMDC_MOTHERBOARD_0_S00_AXI_BASEADDR)
#endif

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
#define MOTHERBOARD_MAX_IP_CORES (XPAR_AMDC_MOTHERBOARD_NUM_INSTANCES)
#define MOTHERBOARD_1_BASE_ADDR  (XPAR_HIER_GPIO_0_HIER_AMDS_0_AMDC_MOTHERBOARD_0_S00_AXI_BASEADDR)
#define MOTHERBOARD_2_BASE_ADDR  (XPAR_HIER_GPIO_1_HIER_AMDS_0_AMDC_MOTHERBOARD_0_S00_AXI_BASEADDR)
#define MOTHERBOARD_3_BASE_ADDR  (XPAR_HIER_GPIO_2_HIER_AMDS_0_AMDC_MOTHERBOARD_0_S00_AXI_BASEADDR)
#define MOTHERBOARD_4_BASE_ADDR  (XPAR_HIER_GPIO_3_HIER_AMDS_0_AMDC_MOTHERBOARD_0_S00_AXI_BASEADDR)
#endif

typedef enum {
    // Keep first channel index at 0!
    MB_IN1 = 0,
    MB_IN2,
    MB_IN3,
    MB_IN4,
    MB_IN5,
    MB_IN6,
    MB_IN7,
    MB_IN8,

    // Keep this as last entry!
    MB_NUM_CHANNELS,
} mb_channel_e;

static inline bool motherboard_is_valid_channel(mb_channel_e channel)
{
    if (channel >= MB_IN1 && channel < MB_NUM_CHANNELS) {
        return true;
    }

    return false;
}

static inline bool motherboard_is_valid_idx(int idx)
{
    if (idx >= 0 && idx < MOTHERBOARD_MAX_IP_CORES) {
        return true;
    }

    return false;
}

static inline uint32_t motherboard_idx_to_base_addr(int idx)
{
    switch (idx) {
    case 0:
        return MOTHERBOARD_1_BASE_ADDR;

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
    case 1:
        return MOTHERBOARD_2_BASE_ADDR;
    case 2:
        return MOTHERBOARD_3_BASE_ADDR;
    case 3:
        return MOTHERBOARD_4_BASE_ADDR;
#endif

    default:
        return 0;
    }
}

void motherboard_init(void);

void motherboard_set_adc_sampling(uint32_t base_addr, bool enable);
void motherboard_request_new_data(uint32_t base_addr);

int motherboard_get_data(uint32_t base_addr, mb_channel_e channel, int32_t *out);

void motherboard_print_samples(uint32_t base_addr);
void motherboard_print_counters(uint32_t base_addr);

#endif // MOTHERBOARD_H
