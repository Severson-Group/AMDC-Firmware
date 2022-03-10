#include "drv/eddy_current_sensor.h"
#include "usr/user_config.h"
#include "xil_io.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void eddy_current_sensor_init(void)
{
    printf("EDDY CURRENT SENSOR:\tInitializing...\n");

    // Set sampling rate to 20kHz
    eddy_current_sensor_set_sample_rate(EDDY_CURRENT_SENSOR_1_BASE_ADDR, 20000);

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
    eddy_current_sensor_set_sample_rate(EDDY_CURRENT_SENSOR_2_BASE_ADDR, 20000);
    eddy_current_sensor_set_sample_rate(EDDY_CURRENT_SENSOR_3_BASE_ADDR, 20000);
    eddy_current_sensor_set_sample_rate(EDDY_CURRENT_SENSOR_4_BASE_ADDR, 20000);
#endif
}

void eddy_current_sensor_enable(uint32_t base_addr)
{
    Xil_Out32(base_addr + (3 * sizeof(uint32_t)), 1);
}

void eddy_current_sensor_disable(uint32_t base_addr)
{
    Xil_Out32(base_addr + (3 * sizeof(uint32_t)), 0);
}

void eddy_current_sensor_set_sample_rate(uint32_t base_addr, double sample_rate)
{
    uint8_t divider = (uint8_t)(500000 / sample_rate);

    eddy_current_sensor_set_divider(base_addr, divider - 1);
}

void eddy_current_sensor_set_divider(uint32_t base_addr, uint8_t divider)
{
    Xil_Out32(base_addr + (2 * sizeof(uint32_t)), divider);
}

static double bits_to_voltage(uint32_t data)
{
    bool is_negative = 0x20000 & data;

    // Convert 2's compliment to positive data
    if (is_negative) {
        data = ~data;
        data += 1;
    }

    // Convert data to voltage (+/-5V)
    double resolution = 0.00003814697;              // 5V / 2^17
    double voltage = (0x1FFFF & data) * resolution; // 17-bit data

    if (is_negative) {
        voltage = -voltage;
    }

    return voltage;
}

double eddy_current_sensor_read_x_voltage(uint32_t base_addr)
{
    uint32_t x_data = Xil_In32(base_addr);

    return bits_to_voltage(x_data);
}

double eddy_current_sensor_read_y_voltage(uint32_t base_addr)
{
    uint32_t y_data = Xil_In32(base_addr + (1 * sizeof(uint32_t)));

    return bits_to_voltage(y_data);
}
