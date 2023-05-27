#include "drv/eddy_current_sensor.h"
#include "usr/user_config.h"
#include "xil_io.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void eddy_current_sensor_init(void)
{
    printf("EDDY CURRENT SENSOR:\tInitializing...\n");

    // Set eddy current sensors to sample on PWM high by default
    //   with an SCLK frequency of 3 MHz 
    eddy_current_sensor_trigger_on_pwm_high(EDDY_CURRENT_SENSOR_1_BASE_ADDR);
    eddy_current_sensor_set_timing(EDDY_CURRENT_SENSOR_1_BASE_ADDR, 2000, 270);

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
    eddy_current_sensor_trigger_on_pwm_high(EDDY_CURRENT_SENSOR_2_BASE_ADDR);
    eddy_current_sensor_set_timing(EDDY_CURRENT_SENSOR_2_BASE_ADDR, 2000, 270);
    eddy_current_sensor_trigger_on_pwm_high(EDDY_CURRENT_SENSOR_3_BASE_ADDR);
    eddy_current_sensor_set_timing(EDDY_CURRENT_SENSOR_3_BASE_ADDR, 2000, 270);
    eddy_current_sensor_trigger_on_pwm_high(EDDY_CURRENT_SENSOR_4_BASE_ADDR);
    eddy_current_sensor_set_timing(EDDY_CURRENT_SENSOR_4_BASE_ADDR, 2000, 270);
#endif
}

void eddy_current_sensor_trigger_on_pwm_high(uint32_t base_addr)
{
    // Get the current value of the config register and set the pwm_high trigger bit
    uint32_t config_reg_address = base_addr + (3 * sizeof(uint32_t));
    Xil_Out32(config_reg_address, (Xil_In32(config_reg_address) | 0x1));
}

void eddy_current_sensor_trigger_on_pwm_low(uint32_t base_addr)
{
    // Get the current value of the config register and set the pwm_low trigger bit
    uint32_t config_reg_address = base_addr + (3 * sizeof(uint32_t));
    Xil_Out32(config_reg_address, (Xil_In32(config_reg_address) | 0x2));
}

void eddy_current_sensor_trigger_on_pwm_both(uint32_t base_addr)
{
    // Get the current value of the config register and set both the pwm_high and pwm_low trigger bits
    uint32_t config_reg_address = base_addr + (3 * sizeof(uint32_t));
    Xil_Out32(config_reg_address, (Xil_In32(config_reg_address) | 0x3));
}

void eddy_current_sensor_trigger_on_pwm_clear(uint32_t base_addr)
{
    // Get the current value of the config register and clear both the pwm_high and pwm_low trigger bits
    uint32_t config_reg_address = base_addr + (3 * sizeof(uint32_t));
    Xil_Out32(config_reg_address, (Xil_In32(config_reg_address) & ~0x3));
}

void eddy_current_sensor_set_timing(uint32_t base_addr, uint32_t sclk_freq_khz, uint32_t propogation_delay_ns)
{
    // 10 MHz is max frequency (faster frequencies limited by diff/single transceivers)
    //   should give sclk_cnt = 10 axi cycles
    if (sclk_freq_khz > 10000) {
        sclk_freq_khz = 10000;
    }

    // 500 kHz is min frequency (slower frequencies won't complete in a PWM carrier cycle)
    //   should give sclk_cnt = 200 axi cycles
    if (sclk_freq_khz < 500) {
        sclk_freq_khz = 500;
    }

    // This is period in ns for one half of the sclk period
    // We want half a period since sclk_cnt is the number of AXI CLK cycles to wait before toggling SCLK
    uint32_t sclk_half_period_ns = (1000000 / sclk_freq_khz) / 2;

    uint32_t axi_period_ns = 1000 / AXI_CLK_FREQ_MHZ;

    uint32_t sclk_cnt = sclk_half_period_ns / axi_period_ns;

    Xil_Out32(base_addr + (2 * sizeof(uint32_t)), sclk_cnt);

    // SHIFT delayer
    //   Why is this needed? The Kaman adapter board's filtering introduces a significant
    //   propogation delay into the system. On the FPGA side, the SCLK signal
    //   being generated will fall, which is when we would like to sample the MISO line.
    //   However, the fall of SCLK will take a while to propogate through the adapter
    //   board (270ns for example) and then the valid data on the MISO lines will take a
    //   while (again, 270ns for example) to propogate back. In the example, this is
    //   a total round-trip propogation delay of 540ns. The actual delay depends on the
    //   RC filters used on the Kaman adapter board.
    //
    //   This code takes the ONE-WAY propogation delay in nanoseconds of the Kaman adapter
    //   board's filtering, doubles it for the round-trip propogation delay, and then
    //   adds half of the user-requested SCLK period so that the shifting occurs halfway
    //   through when the bit is valid. See the amdc_spi_master.v for details on how
    //   the shift signal is propogated through a shift register and then the appropriate delay
    //   is selected by the shift_index value calculated below.
    uint32_t delay_time = (2 * propogation_delay_ns) + (1 * sclk_half_period_ns);

    uint32_t shift_index = delay_time / axi_period_ns;

    if (shift_index > 255) {
        shift_index = 255;
    }

    Xil_Out32(base_addr + (4 * sizeof(uint32_t)), shift_index);
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
