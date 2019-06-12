#ifdef APP_PARAMS

#include "inverter.h"
#include "../../drv/io.h"
#include "../../drv/pwm.h"
#include <math.h>

static double dtc_dcomp = 0.0;
static double dtc_tau = 0.0;
static double dtc_Vdc = 0.0;

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

inline static double sign(double x)
{
	if (x > 0.0) return 1.0;
	if (x < 0.0) return -1.0;
	return 0.0;
}

void inverter_saturate_to_Vdc(double *voltage)
{
	io_led_color_t color = {0, 0, 0};
	if (saturate(-dtc_Vdc, dtc_Vdc, voltage) != 0) color.g = 255;
	io_led_set_c(0, 1, 0, &color);
}

void inverter_set_voltage(uint8_t pwm_idx, double voltage, double current)
{
	// voltage = -Vbus => d = 0.0
	// voltage =    0V => d = 0.5
	// voltage = +Vbus => d = 1.0
	double duty = 0.5 + (voltage / (2.0 * dtc_Vdc));

	// Calculate duty compensation
	double dcomp = 0.0;

	if (dtc_dcomp != 0.0 || dtc_tau != 0.0) {
		dcomp = sign(current) * dtc_dcomp * (1.0 - pow(M_E, -fabs(current) / dtc_tau));
	}

	pwm_set_duty(pwm_idx, duty + dcomp);
}

void inverter_set_dtc(double dcomp, double tau)
{
	dtc_dcomp = dcomp;
	dtc_tau = tau;
}

void inverter_set_Vdc(double Vdc)
{
	dtc_Vdc = Vdc;
}

#endif // APP_PARAMS
