#include "pwm.h"
#include "xil_io.h"
#include <stdio.h>

#define PWM_BASE_ADDR (0x43C20000)

static uint8_t carrier_divisor;
static uint16_t carrier_max;
static uint16_t deadtime;

// switching_freq = (200e6 / divisor) / (2*carrier_max)
// or
// carrier_max = ((200e6 / divisor) / (switching_freq)) / 2

void pwm_init(void)
{
    printf("PWM:\tInitializing...\n");

    pwm_toggle_reset();

    pwm_set_switching_freq(60000.0);
    pwm_set_deadtime_ns(500);

    // Turn off all PWM outputs
    for (int i = 0; i < 24; i++) {
        pwm_set_duty_raw(i, 0);
    }
}

void pwm_toggle_reset(void)
{
    // Toggles RST on all inverter outputs for 1 ms
    pwm_set_all_rst(0xFF);
    for (int i = 0; i < 83250; i++) asm("nop");
    pwm_set_all_rst(0x00);
    for (int i = 0; i < 83250; i++) asm("nop");
    pwm_set_all_rst(0xFF);
}

void pwm_set_switching_freq(double freq_hz)
{
    // NOTE: freq_hz can be in range:
    // 1526Hz ... ~100MHz

    // Always set carrier_divisor to 0... anything else reduces resolution!
    pwm_set_carrier_divisor(0);

    // Calculate what the carrier_max should be to achieve the right switching freq
    carrier_max = (uint16_t) (((200e6 / (carrier_divisor + 1)) / (freq_hz)) / 2);
    pwm_set_carrier_max(carrier_max);
}

void pwm_set_duty(uint8_t idx, double duty)
{
    if (duty >= 1.0) {
        pwm_set_duty_raw(idx, carrier_max);
    } else if (duty <= 0.0) {
        pwm_set_duty_raw(idx, 0);
    } else {
        pwm_set_duty_raw(idx, duty * carrier_max);
    }
}

void pwm_set_duty_raw(uint8_t idx, uint16_t value)
{
    // Write to offset 0 to control PWM 0
    Xil_Out32(PWM_BASE_ADDR + (idx * sizeof(uint32_t)), value);
}

void pwm_set_carrier_divisor(uint8_t divisor)
{
    carrier_divisor = divisor;

    // Write to slave reg 24 to set triangle carrier clk divisor
    Xil_Out32(PWM_BASE_ADDR + (24 * sizeof(uint32_t)), divisor);
}

void pwm_set_carrier_max(uint16_t max)
{
    carrier_max = max;

    // Write to slave reg 25 to set triangle carrier max value
    Xil_Out32(PWM_BASE_ADDR + (25 * sizeof(uint32_t)), max);
}

void pwm_set_deadtime_ns(uint16_t time_ns)
{
    // Convert time in ns to FPGA clock cycles
    deadtime = time_ns / 5;

    // NOTE: FPGA enforces minimum register value of 5
    // This should help prevent shoot-through events.

    // Write to slave reg 26 to set deadtime value
    Xil_Out32(PWM_BASE_ADDR + (26 * sizeof(uint32_t)), deadtime);
}


void pwm_get_all_flt_temp(uint8_t *flt_temp)
{
    uint32_t value;

    // Offset 28 is flt_temp
    value = Xil_In32(PWM_BASE_ADDR + (28 * sizeof(uint32_t)));

    // Only look at bottom 8 bits
    value &= 0x000000FF;

    *flt_temp = (uint8_t) value;
}

void pwm_get_all_flt_desat(uint8_t *flt_desat)
{
    uint32_t value;

    // Offset 29 is flt_desat
    value = Xil_In32(PWM_BASE_ADDR + (29 * sizeof(uint32_t)));

    // Only look at bottom 8 bits
    value &= 0x000000FF;

    *flt_desat = (uint8_t) value;
}

void pwm_get_all_rdy(uint8_t *rdy)
{
    uint32_t value;

    // Offset 30 is rdy
    value = Xil_In32(PWM_BASE_ADDR + (30 * sizeof(uint32_t)));

    // Only look at bottom 8 bits
    value &= 0x000000FF;

    *rdy = (uint8_t) value;
}

void pwm_set_all_rst(uint8_t rst)
{
    uint32_t value = 0;
    value |= (uint32_t) rst;

    // Offset 27 is rst output reg
    Xil_Out32(PWM_BASE_ADDR + (27 * sizeof(uint32_t)), value);
}

void pwm_get_status(uint8_t idx, pwm_status_t *status)
{
    // Read status signals from hardware
    uint8_t flt_temp, flt_desat, rdy;
    pwm_get_all_flt_temp(&flt_temp);
    pwm_get_all_flt_desat(&flt_desat);
    pwm_get_all_rdy(&rdy);

    uint8_t bit_mask = (1 << idx);

    status->fault_temp  = (flt_temp  & bit_mask) ? 1 : 0;
    status->fault_desat = (flt_desat & bit_mask) ? 1 : 0;
    status->ready       = (rdy       & bit_mask) ? 1 : 0;
}
