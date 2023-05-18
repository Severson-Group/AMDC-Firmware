#include "drv/eddy_current_sensor.h"
#include "usr/user_config.h"
#include "xil_io.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void eddy_current_sensor_init(void)
{
    printf("EDDY CURRENT SENSOR:\tInitializing...\n");

    // Set eddy current sensors to sample on both PWM high and PWM low by default
    eddy_current_sensor_trigger_on_pwm_both(EDDY_CURRENT_SENSOR_1_BASE_ADDR);

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
    eddy_current_sensor_trigger_on_pwm_both(EDDY_CURRENT_SENSOR_2_BASE_ADDR);
    eddy_current_sensor_trigger_on_pwm_both(EDDY_CURRENT_SENSOR_3_BASE_ADDR);
    eddy_current_sensor_trigger_on_pwm_both(EDDY_CURRENT_SENSOR_4_BASE_ADDR);
#endif
}

void eddy_current_sensor_enable(uint32_t base_addr)
{
    // Get the current value of the config register and set the enable bit
    config_reg_address = base_addr + (3 * sizeof(uint32_t))
    Xil_Out32(config_reg_address, (Xil_In32(config_reg_address) | 0x1));
}

void eddy_current_sensor_disable(uint32_t base_addr)
{
    // Get the current value of the config register and clear the enable bit
    config_reg_address = base_addr + (3 * sizeof(uint32_t))
    Xil_Out32(config_reg_address, (Xil_In32(config_reg_address) & ~0x1));
}

void eddy_current_sensor_trigger_on_pwm_high(uint32_t base_addr)
{
    // Get the current value of the config register and set the pwm_high trigger bit
    config_reg_address = base_addr + (3 * sizeof(uint32_t))
    Xil_Out32(config_reg_address, (Xil_In32(config_reg_address) | 0x2));
}

void eddy_current_sensor_trigger_on_pwm_low(uint32_t base_addr)
{
    // Get the current value of the config register and set the pwm_low trigger bit
    config_reg_address = base_addr + (3 * sizeof(uint32_t))
    Xil_Out32(config_reg_address, (Xil_In32(config_reg_address) | 0x4));
}

void eddy_current_sensor_trigger_on_pwm_both(uint32_t base_addr)
{
    // Get the current value of the config register and set both the pwm_high and pwm_low trigger bits
    config_reg_address = base_addr + (3 * sizeof(uint32_t))
    Xil_Out32(config_reg_address, (Xil_In32(config_reg_address) | 0x6));
}

void eddy_current_sensor_trigger_on_pwm_clear(uint32_t base_addr)
{
    // Get the current value of the config register and clear both the pwm_high and pwm_low trigger bits
    config_reg_address = base_addr + (3 * sizeof(uint32_t))
    Xil_Out32(config_reg_address, (Xil_In32(config_reg_address) & ~0x6));  
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
