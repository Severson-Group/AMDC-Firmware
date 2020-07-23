#ifdef APP_BETA_LABS

#include "usr/beta_labs/inverter.h"
#include "drv/io.h"
#include "drv/pwm.h"
#include <math.h>

static double dtc_dcomp = 0.0;
static double dtc_currLimit = 0.0;

// Don't init to 0 since we divide by this! (User should override this value)
static double inverter_Vdc = 1.0;

inline static int saturate(double min, double max, double *value)
{
    if (*value < min) {
        // Lower bound saturation
        *value = min;
        return -1;
    } else if (*value > max) {
        // Upper bound saturation
        *value = max;
        return 1;
    } else {
        // No saturation
        return 0;
    }
}

void inverter_saturate_to_Vdc(double *voltage)
{
    io_led_color_t color = { 0, 0, 0 };
    if (saturate(-inverter_Vdc, inverter_Vdc, voltage) != 0)
        color.g = 255;
    io_led_set_c(0, 1, 0, &color);
}

void inverter_set_voltage(uint8_t pwm_idx, double voltage, double current)
{
    double duty = 0.5 + (voltage / inverter_Vdc);

    // Calculate duty compensation
    double dcomp = 0.0;

    if (dtc_dcomp != 0.0 && dtc_currLimit != 0.0) {
        if (current < -dtc_currLimit) {
            dcomp = -dtc_dcomp;
        } else if (current > dtc_currLimit) {
            dcomp = dtc_dcomp;
        } else {
            dcomp = (dtc_dcomp / dtc_currLimit) * current;
        }
    }

    pwm_set_duty(pwm_idx, duty + dcomp);
}

void inverter_set_dtc(double dcomp, double currLimit)
{
    dtc_dcomp = dcomp;
    dtc_currLimit = currLimit;
}

void inverter_set_Vdc(double Vdc)
{
    inverter_Vdc = Vdc;
}

double inverter_get_Vdc(void)
{
    return inverter_Vdc;
}

#endif // APP_BETA_LABS
