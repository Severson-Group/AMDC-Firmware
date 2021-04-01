#ifndef MOTHERBOARD_H
#define MOTHERBOARD_H

#include "drv/hardware_targets.h"
#include "sys/errors.h"
#include "usr/user_config.h"
#include <stdbool.h>
#include <stdint.h>

// Throw compiler error if the motherboard is not supported on the hardware target
// NOTE: this won't be an issue if the user code never includes this file!
#if USER_CONFIG_HARDWARE_TARGET != HW_TARGET_AMDC_REV_D
#error "SensorCard motherboard interface is only supported on the AMDC REV D hardware target."
#endif

#if USER_CONFIG_HARDWARE_TARGET == HW_TARGET_AMDC_REV_D
#define MOTHERBOARD_BASE_ADDR (0x43C70000)
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

void motherboard_init(uint32_t base_addr);

void motherboard_set_adc_sampling(uint32_t base_addr, bool enable);
void motherboard_request_new_data(uint32_t base_addr);

error_t motherboard_get_data(uint32_t base_addr, mb_channel_e channel, int32_t *out);

void motherboard_print_samples(uint32_t base_addr);
void motherboard_print_counters(uint32_t base_addr);

#endif // MOTHERBOARD_H
