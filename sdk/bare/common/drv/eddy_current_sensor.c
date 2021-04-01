#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if USER_CONFIG_HARDWARE_TARGET == HW_TARGET_AMDC_REV_D

#include "drv/eddy_current_sensor.h"
#include "xil_io.h"
#include <stdint.h>
#include <stdio.h>

#define EDDY_CURRENT_SENSOR_BASE_ADDR (0x43C80000)

static void eddy_current_sensor_set_divider(uint8_t divider);

void eddy_current_sensor_init(void)
{
    printf("EDDY CURRENT SENSOR:\tInitializing...\n");

    // Set sampling rate to 20kHz
    eddy_current_sensor_set_sample_rate_hz(20000);
}

void eddy_current_sensor_enable(void)
{
    Xil_Out32(EDDY_CURRENT_SENSOR_BASE_ADDR + (3 * sizeof(uint32_t)), 1);
}

void eddy_current_sensor_disable(void)
{
    Xil_Out32(EDDY_CURRENT_SENSOR_BASE_ADDR + (3 * sizeof(uint32_t)), 0);
}

void eddy_current_sensor_set_sample_rate_hz(double sample_rate_hz)
{
    uint8_t divider = (uint8_t)(500000 / sample_rate_hz);

    eddy_current_sensor_set_divider(divider - 1);
}

static void eddy_current_sensor_set_divider(uint8_t divider)
{
    Xil_Out32(EDDY_CURRENT_SENSOR_BASE_ADDR + (2 * sizeof(uint32_t)), divider);
}

int32_t eddy_current_sensor_get_x_bits(void)
{
    return Xil_In32(EDDY_CURRENT_SENSOR_BASE_ADDR);
}

int32_t eddy_current_sensor_get_y_bits(void)
{
    return Xil_In32(EDDY_CURRENT_SENSOR_BASE_ADDR + (1 * sizeof(uint32_t)));
}

#endif // USER_CONFIG_HARDWARE_TARGET
