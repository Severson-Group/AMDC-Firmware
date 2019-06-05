#include "task_dtc.h"
#include "../../sys/scheduler.h"
#include "../../sys/defines.h"
#include "../../sys/debug.h"
#include "../../drv/io.h"
#include "../../drv/pwm.h"
#include "../../drv/dac.h"
#include "machine.h"

#define Wb				(DTC_BANDWIDTH * PI2) // rad/s
#define Ts				(1.0 / TASK_DTC_UPDATES_PER_SEC)
#define Kp				(Wb * Ld_HAT)
#define Ki				((Rs_HAT / Ld_HAT) * Kp)

static double Ia_star =  1.0;
static double Ib_star =  -1.0;

static double Ia_err_acc = 0.0;
static double Ib_err_acc = 0.0;

static task_control_block_t tcb;

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

void task_dtc_init(void)
{
	printf("DTC:\tInitializing DTC task...\n");

	scheduler_tcb_init(&tcb, task_dtc_callback, NULL, "dtc", TASK_DTC_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);
}

static void _get_Iabc(double *Iabc)
{
	// Read from ADCs
	float Iabc_f[3];
	analog_getf(CC_PHASE_A_ADC, &Iabc_f[0]);
	analog_getf(CC_PHASE_B_ADC, &Iabc_f[1]);
	analog_getf(CC_PHASE_C_ADC, &Iabc_f[2]);

	// Convert ADC values to raw currents
	Iabc[0] = ((double) Iabc_f[0] * ADC_TO_AMPS_PHASE_A_GAIN) + ADC_TO_AMPS_PHASE_A_OFFSET;
	Iabc[1] = ((double) Iabc_f[1] * ADC_TO_AMPS_PHASE_B_GAIN) + ADC_TO_AMPS_PHASE_B_OFFSET;
	Iabc[2] = ((double) Iabc_f[2] * ADC_TO_AMPS_PHASE_C_GAIN) + ADC_TO_AMPS_PHASE_C_OFFSET;
}

void task_dtc_callback(void *arg)
{
	// Get currents
	double Iabc[3];
	_get_Iabc(Iabc);

	// PI stationary frame current regulator
	// on Ia, Ib

	double Ia = Iabc[0];
	double Ib = Iabc[1];

	// Ia
	double Ia_err;
	double Va_star;
	Ia_err = Ia_star - Ia;
	Ia_err_acc += Ia_err;
	Va_star = (Kp * Ia_err) + (Ki * Ts * Ia_err_acc);

	// q-axis
	double Ib_err;
	double Vb_star;
	Ib_err = Ib_star - Ib;
	Ib_err_acc += Ib_err;
	Vb_star = (Kp * Ib_err) + (Ki * Ts * Ib_err_acc);


//	// ------------------------------------
//	// Output to DAC
//	// ------------------------------------
//	double dac1 = (Va_star / DTC_BUS_VOLTAGE);
//	double dac2 = (Vb_star / DTC_BUS_VOLTAGE);
//	dac_set_voltage(0, 100*dac1 * DAC_FULL_SCALE);
//	dac_set_voltage(1, 100*dac2 * DAC_FULL_SCALE);

#if 1

	static int counter = 0;
	const static int SAMPLES_PER_SEC = 5;

	counter++;
	if (counter >= TASK_DTC_UPDATES_PER_SEC / SAMPLES_PER_SEC) {
		counter = 0;

		char msg[256];
		snprintf(msg, 256, "%f\t%f\r\n", Ia_err_acc, Ib_err_acc);

		debug_print(msg);
	}

#endif


	// ------------------------------------
	// Saturate Vab to CC_BUS_VOLTAGE
	// ------------------------------------
	io_led_color_t color = {0, 0, 0};
	if (saturate(-DTC_BUS_VOLTAGE, DTC_BUS_VOLTAGE, &Va_star) != 0) color.g = 255;
	if (saturate(-DTC_BUS_VOLTAGE, DTC_BUS_VOLTAGE, &Vb_star) != 0) color.g = 255;
	io_led_set_c(0, 1, 0, &color);

	// --------------------------------------
	// (5) Write voltages out to PWM hardware
	// --------------------------------------

	// Vabc = -Vbus => d = 0.0
	// Vabc =    0V => d = 0.5
	// Vabc = +Vbus => d = 1.0

	pwm_set_duty(CC_PHASE_A_PWM_LEG_IDX, (((Va_star / DTC_BUS_VOLTAGE) + 1.0) / 2.0));
	pwm_set_duty(CC_PHASE_B_PWM_LEG_IDX, (((Vb_star / DTC_BUS_VOLTAGE) + 1.0) / 2.0));
}
