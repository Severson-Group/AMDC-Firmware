#include "inverter.h"
#include "../../drv/io.h"
#include "../../drv/pwm.h"

double LOG_dcomp = 0.0;

static double dtc_dcomp = 0.0;
static double dtc_current = 0.0;
static double dtc_dcompamp = 0.0;

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

void inverter_init(void)
{

}

void inverter_saturate_to_Vdc(double *voltage)
{
	io_led_color_t color = {0, 0, 0};
	if (saturate(-VOLTAGE_DC_BUS, VOLTAGE_DC_BUS, voltage) != 0) color.g = 255;
	io_led_set_c(0, 1, 0, &color);
}

void inverter_set_voltage(uint8_t pwm_idx, double voltage, double current)
{
	// voltage = -Vbus => d = 0.0
	// voltage =    0V => d = 0.5
	// voltage = +Vbus => d = 1.0
	double duty = 0.5 + (voltage / (2.0 * VOLTAGE_DC_BUS));

	// Calculate duty compensation
	double dcomp = 0.0;
	if (dtc_current != 0.0 && dtc_dcomp != 0.0) {
		if (current < -dtc_current) {
			dcomp = -dtc_dcomp;
		} else if (current > dtc_current) {
			dcomp = dtc_dcomp;
		} else {
			dcomp = (dtc_dcomp / dtc_current) * current;
		}
	}

	LOG_dcomp = duty;

	pwm_set_duty(pwm_idx, duty + dcomp);
}

void inverter_set_dtc(double dcomp, double current)
{
	dtc_current = current;
	dtc_dcomp = dcomp;
}
