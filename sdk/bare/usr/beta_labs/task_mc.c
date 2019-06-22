#ifdef APP_BETA_LABS

#include "task_mc.h"
#include "task_cc.h"
#include "task_mo.h"
#include "../../sys/scheduler.h"
#include "../../sys/injection.h"
#include "../../drv/encoder.h"
#include "../../drv/io.h"
#include "../../drv/dac.h"
#include "machine.h"

// Tuning for motion controller
#define Ba		(0.3789345489605353)
#define Ksa		(1.9521469077445932)
#define Kisa	(1.6224059561087412)

#define Ts		(1.0 / TASK_MC_UPDATES_PER_SEC)

// Current limits which mc can command to cc
#define MIN_CC_CURRENT	(-I_rated_dq) // Amps, in DQ frame
#define MAX_CC_CURRENT	(+I_rated_dq) // Amps, in DQ frame

// Forward declarations
inline static int saturate(double min, double max, double *value);

// Static variables for controller
static double delta_theta_error_acc;
static double delta_theta_error_acc_acc;

// Injection contexts for motion controller
inj_ctx_t task_mc_inj_del_theta_star;

// Command for controller
static double delta_theta_star = 0.0;

static task_control_block_t tcb;

uint8_t task_mc_is_inited(void)
{
	return scheduler_tcb_is_registered(&tcb);
}

void task_mc_init(void)
{
	// Register scheduler task
	scheduler_tcb_init(&tcb, task_mc_callback, NULL, "mc", TASK_MC_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);

	// Initialize and register injection contexts
	injection_ctx_init(&task_mc_inj_del_theta_star, "del_theta*");
	injection_ctx_register(&task_mc_inj_del_theta_star);

	// Clear static variables for controller
	delta_theta_error_acc = 0.0;
	delta_theta_error_acc_acc = 0.0;
}

void task_mc_deinit(void)
{
	// Unregister scheduler task
	scheduler_tcb_unregister(&tcb);

	// Clear current command
	task_cc_set_Iq_star(0.0);

	// Unregister injection context
	injection_ctx_unregister(&task_mc_inj_del_theta_star);
}

void task_mc_callback(void *arg) {
	// Inject signal into delta_theta_star
	injection_inj(&delta_theta_star, &task_mc_inj_del_theta_star, Ts);

	double omega_m;
	task_mo_get_omega_m(&omega_m);

	double delta_theta_m = omega_m * Ts;

	// Run through block diagram
	double delta_theta_error = delta_theta_star - delta_theta_m;
	delta_theta_error_acc += delta_theta_error;
	delta_theta_error_acc_acc += delta_theta_error_acc;

	// Calculate commanded torque
	double term1 = (Ba / Ts * delta_theta_error);
	double term2 = (Ksa * delta_theta_error_acc);
	double term3 = (Kisa * Ts * delta_theta_error_acc_acc);
	double Tem_SFB_star = term1 + term2 + term3;

	// Convert to commanded current, Iq*
	double Iq_star = Tem_SFB_star / Kt_HAT;

	// Saturate Iq_star to limit
	io_led_color_t color = {0, 0, 0};
	if (saturate(MIN_CC_CURRENT, MAX_CC_CURRENT, &Iq_star) != 0) color.r = 255;
	io_led_set_c(1, 0, 0, &color);

	// Set value in current controller
	task_cc_set_Iq_star(Iq_star);
}

void task_mc_set_delta_theta_star(double omega_m)
{
	delta_theta_star = omega_m * Ts;
}

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

#endif // APP_BETA_LABS
