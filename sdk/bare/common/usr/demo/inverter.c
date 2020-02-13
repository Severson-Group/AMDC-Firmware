#ifdef APP_DEMO

#include "usr/demo/inverter.h"
#include "drv/io.h"
#include "drv/pwm.h"
#include <math.h>

// Don't init to 0 since we divide by this! (User should override this value)
static double inverter_Vdc = 1.0;

inline static int saturate(double min, double max, double *value) {
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

void inverter_saturate_to_Vdc(double *phase_voltage)
{
    io_led_color_t color = {0, 0, 0};
    if (saturate(-inverter_Vdc / 2.0, inverter_Vdc / 2.0, phase_voltage) != 0) color.g = 255;
    io_led_set_c(0, 1, 0, &color);
}

void inverter_set_voltage(uint8_t pwm_idx, double phase_voltage)
{
    double duty = 0.5 + (phase_voltage / inverter_Vdc);
    pwm_set_duty(pwm_idx, duty);
}

void inverter_set_Vdc(double Vdc)
{
    inverter_Vdc = Vdc;
}

double inverter_get_Vdc(void)
{
    return inverter_Vdc;
}

#endif // APP_DEMO
