#include "drv/pwm.h"
#include "drv/hardware_targets.h"
#include "sys/defines.h"
#include "usr/user_config.h"
#include "xgpiops.h"
#include "xil_io.h"
#include <stdbool.h>
#include <stdio.h>

static int pwm_set_carrier_divisor(uint8_t divisor);
static int pwm_set_carrier_max(uint16_t max);
static int pwm_set_duty_raw(pwm_channel_e channel, uint16_t value);

// Store current state of FPGA registers
static uint8_t carrier_divisor;
static uint16_t carrier_max;
static uint16_t deadtime;

// Store current state of user params
static uint16_t now_deadtime;
static double now_fsw;

// switching_freq = (200e6 / divisor) / (2*carrier_max)
// or
// carrier_max = ((200e6 / divisor) / (switching_freq)) / 2

static XGpioPs Gpio;
static const uint32_t pin_PS_DRIVE_EN_MIO = 44;

static void setup_pin_PS_DRIVE_EN(void)
{
    int Status;
    XGpioPs_Config *GPIOConfigPtr;

    // GPIO Initialization
    GPIOConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
    Status = XGpioPs_CfgInitialize(&Gpio, GPIOConfigPtr, GPIOConfigPtr->BaseAddr);
    if (Status != XST_SUCCESS) {
        // Just hang here if error...
        while (1) {
        }
    }

    // Set the PS_DRIVE_EN MIO pin as an output
    XGpioPs_SetDirectionPin(&Gpio, pin_PS_DRIVE_EN_MIO, 1);

    // Start the pin as off
    XGpioPs_WritePin(&Gpio, pin_PS_DRIVE_EN_MIO, 0);

    // Enable the PS_DRIVE_EN MIO pin
    XGpioPs_SetOutputEnablePin(&Gpio, pin_PS_DRIVE_EN_MIO, 1);
}

void pwm_init(void)
{
    printf("PWM:\tInitializing...\n");

    setup_pin_PS_DRIVE_EN();

    // Default to no switching (all PWM outputs are logic LOW)
    // Opens all switches...
    pwm_disable();

    // Set the duty latching mode using value in user_config.h
    pwm_set_duty_latching_mode();

    pwm_toggle_reset();

    pwm_set_switching_freq(PWM_DEFAULT_SWITCHING_FREQ_HZ);
    pwm_set_deadtime_ns(PWM_DEFAULT_DEADTIME_NS);

    // Initialize all PWMs to 50% duty output.
    // Since PWM is disabled, this has no effect now,
    // but if user enables PWM without updating registers,
    // 50% will appear at outputs.
    pwm_set_all_duty_midscale();

    // Initialize the PWM mux block in the FPGA to basic pass-through
    uint32_t mux_config_data[48];
    for (int i = 0; i < 48; i++) {
        mux_config_data[i] = i;
    }
    pwm_mux_set_all_pins(mux_config_data);
}

/*
 * Sets the duty latching mode based off of the value in user_config.h
 * DUTY RATIO UPDATE MODES:
 * Mode 0: Update duty ratios at next timing manager trigger (default)
 * Mode 1: Update duty ratios at next PWM carrier peak/valley
 * Mode 2: Update duty ratios immediately (next FPGA clock rise)
 */
int pwm_set_duty_latching_mode(void)
{
    // Only allow PWM configuration changes when switching is off
    if (pwm_is_enabled()) {
        return FAILURE;
    }

    uint32_t config_reg_addr = PWM_BASE_ADDR + PWM_CONFIG_REG_OFFSET;

#if USER_CONFIG_PWM_DUTY_LATCHING_MODE == 0
    // Set slv_reg31[5:4] = 2'b00
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) & 0xFFFFFFCF));
#elif USER_CONFIG_PWM_DUTY_LATCHING_MODE == 1
    // Set slv_reg31[5:4] = 2'b01
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) | 0x00000010));
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) & 0xFFFFFFDF));
#elif USER_CONFIG_PWM_DUTY_LATCHING_MODE == 2
    // Set slv_reg31[5:4] = 2'b10
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) | 0x00000020));
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) & 0xFFFFFFEF));
#else
#error Invalid configuration for PWM Duty Latching Mode
#endif

    return SUCCESS;
}

void pwm_set_all_duty_midscale(void)
{
    for (int i = 0; i < PWM_NUM_CHANNELS; i++) {
        pwm_set_duty(i, 0.5);
    }
}

void pwm_toggle_reset(void)
{
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
}

void pwm_set_all_rst(uint8_t rst)
{
    uint32_t value = 0;
    value |= (uint32_t) rst;

    // Offset 27 is rst output reg
    Xil_Out32(PWM_BASE_ADDR + PWM_RESETS_REG_OFFSET, value);
}

// Hardware disabling of PWM was added to REV E hardware
#if (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_F)

static bool is_pwm_enable_hw_enabled = false;

int pwm_enable_hw(bool en)
{
    int err = FAILURE;

    if (en) {
        if (!is_pwm_enable_hw_enabled) {
            XGpioPs_WritePin(&Gpio, pin_PS_DRIVE_EN_MIO, 1);
            is_pwm_enable_hw_enabled = true;
            err = SUCCESS;
        }
    } else {
        if (is_pwm_enable_hw_enabled) {
            XGpioPs_WritePin(&Gpio, pin_PS_DRIVE_EN_MIO, 0);
            is_pwm_enable_hw_enabled = false;
            err = SUCCESS;
        }
    }

    return err;
}

#endif

int pwm_enable(void)
{
    uint32_t config_reg_addr = PWM_BASE_ADDR + PWM_CONFIG_REG_OFFSET;

    if (pwm_is_enabled()) {
        return FAILURE;
    }

    // Set slv_reg31[0] LSB to enable PWM switching
    Xil_Out32(config_reg_addr, Xil_In32(config_reg_addr) | 0x00000001);

    return SUCCESS;
}

int pwm_disable(void)
{
    uint32_t config_reg_addr = PWM_BASE_ADDR + PWM_CONFIG_REG_OFFSET;

    if (!pwm_is_enabled()) {
        return FAILURE;
    }

    // Clear slv_reg31[0] LSB to disable PWM switching
    Xil_Out32(config_reg_addr, Xil_In32(config_reg_addr) & 0xFFFFFFFE);

    return SUCCESS;
}

bool pwm_is_enabled(void)
{
    uint32_t reg31 = Xil_In32(PWM_BASE_ADDR + PWM_CONFIG_REG_OFFSET);

    // LSB of reg 31 is enable bit for PWM
    return reg31 & 0x00000001;
}

int pwm_set_switching_freq(double freq_hz)
{
    // Only allow PWM configuration changes when switching is off
    if (pwm_is_enabled()) {
        return FAILURE;
    }

    // Based on FPGA, freq_hz can be in range: 1526Hz ... ~100MHz
    // For sanity, we limit this to: 2kHz to 2MHz
    if (freq_hz < PWM_MIN_SWITCHING_FREQ_HZ || freq_hz > PWM_MAX_SWITCHING_FREQ_HZ) {
        return FAILURE;
    }

    // Always set carrier_divisor to 0... anything else reduces resolution!
    pwm_set_carrier_divisor(0);

    // Calculate what the carrier_max should be to achieve the right switching freq
    carrier_max = (uint16_t)(((200e6 / (carrier_divisor + 1)) / (freq_hz)) / 2);
    pwm_set_carrier_max(carrier_max);

    // Store current freq so we can access later
    now_fsw = freq_hz;

    return SUCCESS;
}

int pwm_set_deadtime_ns(uint16_t time_ns)
{
    // Only allow PWM configuration changes when switching is off
    if (pwm_is_enabled()) {
        return FAILURE;
    }

    // FPGA only supports deadtime reg value from 5 to 2^16 - 1 (naturally supported using uint16_t)
    // Deadtime in ns is just reg value * 5. So, minimum deadtime is 25ns.

    // Ensure requested deadtime is >= 25 ns
    if (time_ns < PWM_MIN_DEADTIME_NS) {
        // Throw error so user knows this didn't work!
        return FAILURE;
    }

    // Convert time in ns to FPGA clock cycles
    deadtime = time_ns / 5;

    // NOTE: FPGA enforces minimum register value of 5
    // This should help prevent shoot-through events.

    // Write to slave reg 26 to set deadtime value
    Xil_Out32(PWM_BASE_ADDR + PWM_DEADTIME_REG_OFFSET, deadtime);

    // Store current deadtime so we can access later
    now_deadtime = time_ns;

    return SUCCESS;
}

double pwm_get_switching_freq(void)
{
    return now_fsw;
}

uint16_t pwm_get_deadtime_ns(void)
{
    return now_deadtime;
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

static int pwm_set_duty_raw(pwm_channel_e channel, uint16_t value)
{
    if (!pwm_is_valid_channel(channel)) {
        return FAILURE;
    }

    // Write to offset 0 to control PWM 0
    Xil_Out32(PWM_BASE_ADDR + (channel * sizeof(uint32_t)), value);

    return SUCCESS;
}

static int pwm_set_carrier_divisor(uint8_t divisor)
{
    // Only allow PWM configuration changes when switching is off
    if (pwm_is_enabled()) {
        return FAILURE;
    }

    // FPGA only supports divisor from 0 to 255 (naturally supported using uint8_t)

    carrier_divisor = divisor;

    // Write to slave reg 24 to set triangle carrier clk divisor
    Xil_Out32(PWM_BASE_ADDR + PWM_CARRIER_CLK_DIV_REG_OFFSET, divisor);

    return SUCCESS;
}

static int pwm_set_carrier_max(uint16_t max)
{
    // Only allow PWM configuration changes when switching is off
    if (pwm_is_enabled()) {
        return FAILURE;
    }

    // FPGA only supports carrier max from 0 to 2^16 - 1 (naturally supported using uint16_t)

    carrier_max = max;

    // Write to slave reg 25 to set triangle carrier max value
    Xil_Out32(PWM_BASE_ADDR + PWM_CARRIER_MAX_REG_OFFSET, max);

    // Since we updated carrier max value, reset PWMs to new 50%
    pwm_set_all_duty_midscale();

    return SUCCESS;
}

// NOTE: we assume config is an array of length >= 48
int pwm_mux_set_all_pins(uint32_t *config)
{
    // Only allow PWM configuration changes when switching is off
    if (pwm_is_enabled()) {
        return FAILURE;
    }

    for (int i = 0; i < (PWM_NUM_CHANNELS * 2); i++) {
        Xil_Out32(PWM_MUX_BASE_ADDR + (i * sizeof(uint32_t)), config[i]);
    }

    return SUCCESS;
}

int pwm_mux_set_one_pin(uint32_t pwm_pin_idx, uint32_t config)
{
    // Only allow PWM configuration changes when switching is off
    if (pwm_is_enabled()) {
        return FAILURE;
    }

    if (pwm_pin_idx < 0 || pwm_pin_idx >= 48) {
        return FAILURE;
    }

    Xil_Out32(PWM_MUX_BASE_ADDR + (pwm_pin_idx * sizeof(uint32_t)), config);

    return SUCCESS;
}

/* Enables or disables switching of a single leg of an inverter
 * All legs (PWM channels) are enabled by default in the FPGA
 */
int pwm_set_leg_enabled(pwm_channel_e channel, bool enabled)
{
    // Only allow PWM configuration changes when switching is off
    if (pwm_is_enabled()) {
        return FAILURE;
    }

    uint32_t leg_enable_reg_addr = PWM_BASE_ADDR + PWM_LEG_ENABLE_REG_OFFSET;
    uint32_t channel_bit = 0x000000001 << channel;

    if (enabled) {
        // Enabling this leg
        Xil_Out32(leg_enable_reg_addr, Xil_In32(leg_enable_reg_addr) | channel_bit);
    } else {
        // Disabling this leg
        Xil_Out32(leg_enable_reg_addr, Xil_In32(leg_enable_reg_addr) & ~channel_bit);
    }

    return SUCCESS;
}

/* Reverses the top/bottom gate drive signals to a single leg of an inverter
 * All legs (PWM channels) are, by default, NOT reversed in the FPGA
 */
int pwm_set_leg_reversed(pwm_channel_e channel, bool reversed)
{
    // Only allow PWM configuration changes when switching is off
    if (pwm_is_enabled()) {
        return FAILURE;
    }

    uint32_t leg_reverse_reg_addr = PWM_BASE_ADDR + PWM_LEG_REVERSE_REG_OFFSET;
    uint32_t channel_bit = 0x000000001 << channel;

    if (reversed) {
        // Reversing this leg
        Xil_Out32(leg_reverse_reg_addr, Xil_In32(leg_reverse_reg_addr) | channel_bit);
    } else {
        // Disabling this leg
        Xil_Out32(leg_reverse_reg_addr, Xil_In32(leg_reverse_reg_addr) & ~channel_bit);
    }

    return SUCCESS;
}
