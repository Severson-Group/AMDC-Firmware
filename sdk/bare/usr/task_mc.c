#include "task_mc.h"
#include "task_cc.h"
#include "machine.h"
#include "../sys/defines.h"
#include "../sys/scheduler.h"
#include "../drv/encoder.h"
#include "../drv/io.h"
#include <stdint.h>

#define Wb		(MC_BANDWIDTH * PI2) // rad/s
#define Kp		(Jp * Wb)

#define MIN_CC_CURRENT	(-I_rated_dq) // Amps, in DQ frame
#define MAX_CC_CURRENT	(+I_rated_dq) // Amps, in DQ frame

static int32_t prev_steps = 0;
static double omega_star = 0.0;


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

void task_mc_set_omega_star(double my_omega_star)
{
	omega_star = my_omega_star;
}

static task_control_block_t tcb;


uint8_t task_mc_is_inited(void)
{
	return scheduler_tcb_is_registered(&tcb);
}

void task_mc_init(void)
{
	scheduler_tcb_init(&tcb, task_mc_callback, TASK_MC_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);
}

void task_mc_deinit(void)
{
	scheduler_tcb_unregister(&tcb);
}

void task_mc_callback(void)
{
	// ----------------------
	// Calculate raw encoder step delta
	// ----------------------

	int32_t curr_steps;
	encoder_get_steps(&curr_steps);

	int32_t step_delta = curr_steps - prev_steps;
	prev_steps = curr_steps;

	double theta_delta = (double)PI2 * ((double)step_delta / (double)ENCODER_PULSES_PER_REV); // rads
	double omega_m = theta_delta * (double)TASK_MC_UPDATES_PER_SEC; // rads / s

	// ------------------------
	// Filter omega
	// ------------------------

	// TODO: implement observer here!
	double omega_m_filtered = omega_m;


	// --------------
	// Run controller
	// --------------

	double omega_err = omega_star - omega_m_filtered;
	double T = omega_err * (double)Kp;
	double Iq_star = T / (double)Kt_HAT;


	// ----------------
	// Saturate Iq_star
	// ----------------

	io_led_color_t color = {0, 0, 0};
	if (saturate(MIN_CC_CURRENT, MAX_CC_CURRENT, &Iq_star) != 0) color.r = 255;
	io_led_set_c(1, 0, 0, &color);

	// ---------------
	// Send to CC task
	// ---------------

	task_cc_set_Iq_star(Iq_star);
}
