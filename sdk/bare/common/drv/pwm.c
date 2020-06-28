#include "drv/pwm.h"
#include "sys/defines.h"
#include "xil_io.h"
#include <stdbool.h>
#include <stdio.h>

#define PWM_BASE_ADDR (0x43C20000)

static uint8_t carrier_divisor;
static uint16_t carrier_max;
static uint16_t deadtime;

// switching_freq = (200e6 / divisor) / (2*carrier_max)
// or
// carrier_max = ((200e6 / divisor) / (switching_freq)) / 2

bool pwm_is_valid_channel(pwm_channel_e channel)
{
    if (channel >= PWM_OUT1 && channel < PWM_NUM_CHANNELS) {
        return true;
    }

    return false;
}

void pwm_init(void)
{
    printf("PWM:\tInitializing...\n");

    pwm_toggle_reset();

    pwm_set_switching_freq(PWM_DEFAULT_SWITCHING_FREQ_HZ);
    pwm_set_deadtime_ns(PWM_DEFAULT_DEADTIME_NS);

    // Turn off all PWM outputs
    for (int i = 0; i < PWM_NUM_CHANNELS; i++) {
        pwm_set_duty_raw(i, 0);
    }
}

void pwm_toggle_reset(void)
{
#if HARDWARE_TARGET == 3

    // Toggles RST on all inverter outputs for 1 ms
    pwm_set_all_rst(0xFF);
    for (int i = 0; i < 83250; i++) {
        asm("nop");
    }
    pwm_set_all_rst(0x00);
    for (int i = 0; i < 83250; i++) {
        asm("nop");
    }
    pwm_set_all_rst(0xFF);

#endif // HARDWARE_TARGET
}

int pwm_set_switching_freq(double freq_hz)
{
    // Based on FPGA, freq_hz can be in range: 1526Hz ... ~100MHz
    // For sanity, we limit this to: 2kHz to 2MHz
    if (freq_hz < 2e3 || freq_hz > 2e6) {
        return FAILURE;
    }

    // Always set carrier_divisor to 0... anything else reduces resolution!
    pwm_set_carrier_divisor(0);

    // Calculate what the carrier_max should be to achieve the right switching freq
    carrier_max = (uint16_t)(((200e6 / (carrier_divisor + 1)) / (freq_hz)) / 2);
    pwm_set_carrier_max(carrier_max);

    return SUCCESS;
}

int pwm_set_duty(pwm_channel_e channel, double duty)
{
    if (!pwm_is_valid_channel(channel)) {
        return FAILURE;
    }

    if (duty >= 1.0) {
        pwm_set_duty_raw(channel, carrier_max);
    } else if (duty <= 0.0) {
        pwm_set_duty_raw(channel, 0);
    } else {
        pwm_set_duty_raw(channel, duty * carrier_max);
    }

    return SUCCESS;
}

int pwm_set_duty_raw(pwm_channel_e channel, uint16_t value)
{
    if (!pwm_is_valid_channel(channel)) {
        return FAILURE;
    }

    // Write to offset 0 to control PWM 0
    Xil_Out32(PWM_BASE_ADDR + (channel * sizeof(uint32_t)), value);

    return SUCCESS;
}

int pwm_set_carrier_divisor(uint8_t divisor)
{
    // FPGA only supports divisor from 0 to 255 (naturally supported using uint8_t)

    carrier_divisor = divisor;

    // Write to slave reg 24 to set triangle carrier clk divisor
    Xil_Out32(PWM_BASE_ADDR + (24 * sizeof(uint32_t)), divisor);

    return SUCCESS;
}

int pwm_set_carrier_max(uint16_t max)
{
    // FPGA only supports carrier max from 0 to 2^16 - 1 (naturally supported using uint16_t)

    carrier_max = max;

    // Write to slave reg 25 to set triangle carrier max value
    Xil_Out32(PWM_BASE_ADDR + (25 * sizeof(uint32_t)), max);

    return SUCCESS;
}

int pwm_set_deadtime_ns(uint16_t time_ns)
{
    // FPGA only supports deadtime reg value from 5 to 2^16 - 1 (naturally supported using uint16_t)
    // Deadtime in ns is just reg value * 5. So, minimum deadtime is 25ns.

    if (deadtime < 25) {
        // Throw error so user knows this didn't work!
        return FAILURE;
    }

    // Convert time in ns to FPGA clock cycles
    deadtime = time_ns / 5;

    // NOTE: FPGA enforces minimum register value of 5
    // This should help prevent shoot-through events.

    // Write to slave reg 26 to set deadtime value
    Xil_Out32(PWM_BASE_ADDR + (26 * sizeof(uint32_t)), deadtime);

    return SUCCESS;
}

#if HARDWARE_TARGET == 3

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

int pwm_get_status(pwm_channel_e channel, pwm_status_t *status)
{
    if (!pwm_is_valid_channel(channel)) {
        return FAILURE;
    }

    // Read status signals from hardware
    uint8_t flt_temp, flt_desat, rdy;
    pwm_get_all_flt_temp(&flt_temp);
    pwm_get_all_flt_desat(&flt_desat);
    pwm_get_all_rdy(&rdy);

    uint8_t bit_mask = (1 << channel);

    status->fault_temp = (flt_temp & bit_mask) ? 1 : 0;
    status->fault_desat = (flt_desat & bit_mask) ? 1 : 0;
    status->ready = (rdy & bit_mask) ? 1 : 0;

    return SUCCESS;
}

#endif // HARDWARE_TARGET
