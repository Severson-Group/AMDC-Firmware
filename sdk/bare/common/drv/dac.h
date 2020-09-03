#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D

#ifndef DAC_H
#define DAC_H

#include <stdint.h>

typedef enum {
    SYNC = 2,
    CONFIG = 3,
    GAIN = 4,
    TRIGGER = 5,
    BRDCAST = 6,
	DAC0 = 8,
	DAC1 = 9,
	DAC2 = 10,
	DAC3 = 11,
	DAC4 = 12,
	DAC5 = 13,
	DAC6 = 14,
	DAC7 = 15,
} dac_reg_t;

#define DAC_MAX_CODE      (4095)

void dac_init(void);

void dac_set_voltage(uint8_t ch, double voltage, double max, double min);

void dac_set_broadcast(double voltage, double max, double min);

void dac_set_trigger(void);

void dac_set_raw(dac_reg_t reg, uint32_t value);

#endif // DAC_H

#endif // USER_CONFIG_HARDWARE_TARGET
