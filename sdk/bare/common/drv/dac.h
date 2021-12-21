#ifndef DAC_H
#define DAC_H

#include <stdint.h>

typedef enum {
    DAC_REG_SYNC = 2,
    DAC_REG_CONFIG = 3,
    DAC_REG_GAIN = 4,
    DAC_REG_TRIGGER = 5,
    DAC_REG_BRDCAST = 6,
    DAC_REG_DAC0 = 8,
    DAC_REG_DAC1 = 9,
    DAC_REG_DAC2 = 10,
    DAC_REG_DAC3 = 11,
    DAC_REG_DAC4 = 12,
    DAC_REG_DAC5 = 13,
    DAC_REG_DAC6 = 14,
    DAC_REG_DAC7 = 15,
} dac_reg_t;

#define DAC_MAX_CODE (4095)

void dac_init(void);

void dac_set_voltage(uint8_t ch, double voltage);

void dac_set_broadcast(double voltage);

void dac_set_trigger(void);

void dac_set_raw(dac_reg_t reg, uint32_t value);

#endif // DAC_H
