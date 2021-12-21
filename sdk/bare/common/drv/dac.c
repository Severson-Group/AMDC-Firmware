#include "drv/dac.h"
#include "xil_io.h"
#include <stdint.h>
#include <stdio.h>

#define DAC_BASE_ADDR (0x43C60000)

void dac_init(void)
{
    printf("DAC:\tInitializing...\n");

    // Default Reference divider and each channel Gain to 2/2
    dac_set_raw(DAC_REG_GAIN, 0x000401FF);

    // Default all channels to be synchronously triggered
    dac_set_raw(DAC_REG_SYNC, 0x0002FFFF);

    // Default all channel values to 0V (redundant to power on reset)
    dac_set_broadcast(0);
}

void dac_set_voltage(uint8_t ch, double voltage)
{
    double range = 20;
    uint32_t value = (voltage + 10) * DAC_MAX_CODE / range;

    dac_set_raw(ch + 0x8, (ch + 0x8) << 16 | (value << 4 & 0xFFF0));
}

void dac_set_broadcast(double voltage)
{
    double range = 20;
    uint32_t value = (voltage + 10) * DAC_MAX_CODE / range;

    dac_set_raw(0x6, (0x6) << 16 | (value << 4 & 0xFFF0));
}

void dac_set_trigger(void)
{
    Xil_Out32(DAC_BASE_ADDR + (DAC_REG_TRIGGER * sizeof(uint32_t)), 0x00000010);
}

void dac_set_raw(dac_reg_t reg, uint32_t value)
{
    Xil_Out32(DAC_BASE_ADDR + (reg * sizeof(uint32_t)), value);
}
