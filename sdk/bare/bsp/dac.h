#ifndef DAC_H
#define DAC_H

#include <stdint.h>

#define DAC_FULL_SCALE	(4095)

void dac_init(void);

void dac_set_voltage(uint8_t idx, uint16_t value);
void dac_set_led(uint8_t idx, uint8_t state);

#endif // DAC_H
