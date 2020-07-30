#ifndef MOTHERBOARD_H
#define MOTHERBOARD_H

#include "drv/hardware_targets.h"
#include "usr/user_config.h"
#include <stdbool.h>
#include <stdint.h>

// Throw compiler error if the motherboard is not supported on the hardware target
// NOTE: this won't be an issue if the user code never includes this file!
#if USER_CONFIG_HARDWARE_TARGET != AMDC_REV_D
#error "SensorCard motherboard interface is only supported on the AMDC REV D hardware target."
#endif

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
#define MOTHERBOARD_BASE_ADDR (0x43C40000)
#endif

void motherboard_init(uint32_t base_addr);

void motherboard_set_adc_sampling(bool enable);
void motherboard_request_new_data(void);

#endif // MOTHERBOARD_H
