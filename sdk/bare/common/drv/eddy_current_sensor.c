#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D

#include "drv/eddy_current_sensor.h"
#include "xil_io.h"
#include <stdint.h>
#include <stdio.h>

#define EDDY_CURRENT_SENSOR_BASE_ADDR (0x43C80000)

void eddy_current_sensor_init(void)
{
    printf("EDDY CURRENT SENSOR:\tInitializing...\n");

    // Set sampling rate to 20kHz
    eddy_current_sensor_set_sample_rate(20000);
}

void eddy_current_sensor_enable(void)
{
    Xil_Out32(EDDY_CURRENT_SENSOR_BASE_ADDR + (3 * sizeof(uint32_t)), 1);
}

void eddy_current_sensor_disable(void)
{
    Xil_Out32(EDDY_CURRENT_SENSOR_BASE_ADDR + (3 * sizeof(uint32_t)), 0);
}

void eddy_current_sensor_set_sample_rate(double sample_rate)
{
    uint8_t divider = (uint8_t)(500000 / sample_rate);

    eddy_current_sensor_set_divider(divider - 1);
}

void eddy_current_sensor_set_divider(uint8_t divider)
{
    Xil_Out32(EDDY_CURRENT_SENSOR_BASE_ADDR + (2 * sizeof(uint32_t)), divider);
}

double eddy_current_sensor_read_x_voltage(void)
{
    uint32_t x_data = Xil_In32(EDDY_CURRENT_SENSOR_BASE_ADDR);

    double resolution = 0.000038141;                                   // 5V / 2^17
    double voltage = (0x1FFFF & x_data) * resolution;                  // 17-bits of positional data
    double signed_voltage = 0x20000 & x_data ? voltage * -1 : voltage; // 18th bit determines sign

    return signed_voltage;
}

double eddy_current_sensor_read_y_voltage(void)
{
    uint32_t y_data = Xil_In32(EDDY_CURRENT_SENSOR_BASE_ADDR + (1 * sizeof(uint32_t)));

    double resolution = 0.000038141;                                   // 5V / 2^17
    double voltage = (0x1FFFF & y_data) * resolution;                  // 17-bits of positional data
    double signed_voltage = 0x20000 & y_data ? voltage * -1 : voltage; // 18th bit determines sign

    return signed_voltage;
}

#endif // USER_CONFIG_HARDWARE_TARGET
