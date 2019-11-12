#ifndef DAC_H
#define DAC_H

#include <stdint.h>

#define DAC_FULL_SCALE          (4095)
#define DAC_HALF_FULL_SCALE     (2047)

void dac_init(void);

void dac_set_sclk_div(uint32_t div);

void dac_set_output(uint8_t idx, double value, double min, double max);

void dac_set_voltage_raw(uint8_t idx, uint16_t value);
void dac_set_led(uint8_t idx, uint8_t state);

#endif // DAC_H
