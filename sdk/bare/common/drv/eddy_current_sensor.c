#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D

#include "drv/eddy_current_sensor.h"
#include "xil_io.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define EDDY_CURRENT_SENSOR_BASE_ADDR (0x43C80000)

void eddy_current_sensor_init(void)
{
    printf("EDDY CURRENT SENSOR:\tInitializing...\n");

    // Set sampling rate to 20kHz
    eddy_current_sensor_set_sample_rate(20000);
    eddy_current_sensor_enable();
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

double eddy_current_sensor_bits_to_voltage(uint32_t data)
{
    bool is_negative = 0x20000 & data;

    // Convert 2's compliment to positive data
    if (is_negative) {
    	data = ~data;
    	data += 1;
    }

    // Convert data to voltage (+/-5V)
    double resolution = 0.00003814697; 					// 5V / 2^17
    double voltage = (0x1FFFF & data) * resolution;		// 17-bit data

    if (is_negative)
    	return -voltage;

    return voltage;
}

double eddy_current_sensor_read_x_voltage(void)
{
    uint32_t x_data = Xil_In32(EDDY_CURRENT_SENSOR_BASE_ADDR);

    return eddy_current_sensor_bits_to_voltage(x_data);
}

double eddy_current_sensor_read_y_voltage(void)
{
    uint32_t y_data = Xil_In32(EDDY_CURRENT_SENSOR_BASE_ADDR + (1 * sizeof(uint32_t)));

    return eddy_current_sensor_bits_to_voltage(y_data);
}

#endif // USER_CONFIG_HARDWARE_TARGET
