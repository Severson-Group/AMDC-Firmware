#ifdef APP_BETA_LABS

#include "task_mc.h"
#include "task_cc.h"
#include "task_mo.h"
#include "msf.h"
#include "mcff.h"
#include "../../sys/scheduler.h"
#include "../../sys/injection.h"
#include "../../drv/encoder.h"
#include "../../drv/io.h"
#include "../../drv/dac.h"
#include "machine.h"

// Tuning for motion controller with Ts = 0.00025
// Bandwidth is tuned to 20 Hz
#define Ba		(0.4700369723815065)
#define Ksa		(4.842871125235442)
#define Kisa	(4.496284614595447)

#define Ts		(1.0 / TASK_MC_UPDATES_PER_SEC)

// Current limits which mc can command to cc
#define MIN_CC_CURRENT	(-I_rated_dq) // Amps, in DQ frame
#define MAX_CC_CURRENT	(+I_rated_dq) // Amps, in DQ frame

// Forward declarations
inline static int saturate(double min, double max, double *value);
inline static double filter(double input);

// Static variables for controller
static double delta_theta_m_error_acc;
static double delta_theta_m_error_acc_acc;

// Injection contexts for motion controller
inj_ctx_t task_mc_inj_omega_m_star;
inj_ctx_t task_mc_inj_Td_star;

// Command for controller
static double omega_m_star = 0.0;
static uint8_t mcff_enabled = 0;

static task_control_block_t tcb;

// Logging variables
double LOG_omega_m_star = 0.0;
double LOG_omega_m = 0.0;
double LOG_T_sfb   = 0.0;
double LOG_T_cff   = 0.0;
double LOG_T_d     = 0.0;
double LOG_msf_w_m = 0.0;
double LOG_msf_w_m_dot = 0.0;

uint8_t task_mc_is_inited(void)
{
	return scheduler_tcb_is_registered(&tcb);
}

void task_mc_init(void)
{
	// Register scheduler task
	scheduler_tcb_init(&tcb, task_mc_callback, NULL, "mc", TASK_MC_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);

	// Initialize injection contexts
	injection_ctx_init(&task_mc_inj_omega_m_star, "omega_m*");
	injection_ctx_init(&task_mc_inj_Td_star, "Td*");

	// Register injection contexts
	injection_ctx_register(&task_mc_inj_omega_m_star);
	injection_ctx_register(&task_mc_inj_Td_star);

	// Clear static variables for controller
	delta_theta_m_error_acc = 0.0;
	delta_theta_m_error_acc_acc = 0.0;

	// Clear commanded speed
	omega_m_star = 0.0;

	// Initialize motion state filter
	msf_init(Ts);
}

void task_mc_deinit(void)
{
	// Unregister scheduler task
	scheduler_tcb_unregister(&tcb);

	// Clear current command
	task_cc_set_Iq_star(0.0);

	// Unregister injection context
	injection_ctx_unregister(&task_mc_inj_omega_m_star);
	injection_ctx_unregister(&task_mc_inj_Td_star);

	// Clear injection context
	injection_ctx_clear(&task_mc_inj_omega_m_star);
	injection_ctx_clear(&task_mc_inj_Td_star);

	// Clear static variables for controller
	delta_theta_m_error_acc = 0.0;
	delta_theta_m_error_acc_acc = 0.0;

	// Clear commanded speed
	omega_m_star = 0.0;
}

void task_mc_callback(void *arg) {
	// Inject signal into omega_m*
	injection_inj(&omega_m_star, &task_mc_inj_omega_m_star, Ts);

	// Get raw delta_theta_m
	double omega_m;
	task_mo_get_omega_m(&omega_m);
	double delta_theta_m = omega_m * Ts;

	// Calculate delta_theta* from omega_m* using MSF
	double delta_theta_m_star = msf_update(omega_m_star);

	// Motion Command Feed-Forward
	double Tem_cff = mcff_update(msf_get_omega_m(), msf_get_omega_m_dot());
	LOG_msf_w_m = msf_get_omega_m();
	LOG_msf_w_m_dot = msf_get_omega_m_dot();

	// Run through block diagram
	double delta_theta_m_error = delta_theta_m_star - delta_theta_m;
	delta_theta_m_error_acc += delta_theta_m_error;
	delta_theta_m_error_acc_acc += delta_theta_m_error_acc;

	// Calculate commanded torque
	double term1 = (Ba / Ts * delta_theta_m_error);
	double term2 = (Ksa * delta_theta_m_error_acc);
	double term3 = (Kisa * Ts * delta_theta_m_error_acc_acc);
	double Tem_SFB_star = term1 + term2 + term3;

	// Filter torque command
	double Tem_SFB_star_filtered = filter(Tem_SFB_star);

	// Create torque disturbance and inject signal into it
	double Td_star = 0.0;
	injection_inj(&Td_star, &task_mc_inj_Td_star, Ts);

	// Sum up Tem*
	double Tem_star_total = Tem_SFB_star_filtered + Tem_cff + Td_star;

	// Convert to commanded current, Iq*
	double Iq_star = Tem_star_total / Kt_HAT;

	// Saturate Iq_star to limit
	io_led_color_t color = {0, 0, 0};
	if (saturate(MIN_CC_CURRENT, MAX_CC_CURRENT, &Iq_star) != 0) color.r = 255;
	io_led_set_c(1, 0, 0, &color);

	// Set value in current controller
	task_cc_set_Iq_star(Iq_star);

	// Update logging variables
	LOG_omega_m_star = omega_m_star;
	LOG_omega_m = omega_m;
	LOG_T_sfb   = Tem_SFB_star_filtered;
	LOG_T_cff   = Tem_cff;
	LOG_T_d     = Td_star;
}

void task_mc_set_omega_m_star(double omega_m)
{
	omega_m_star = omega_m;
}

void task_mc_enabled_cff(uint32_t enabled)
{
	mcff_enabled = enabled;
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

//Tuning for LPF based on Ts = 0.00025
#define A_1Hz		(0.9984304367280448)
#define A_5Hz		(0.9921767802925615)
#define A_10Hz		(0.9844147633517137)
#define A_50Hz		(0.9244652503762558)
#define A_100Hz		(0.8546359991532334)
#define A_500Hz		(0.45593812776599624)

#define A (A_50Hz)

inline static double filter(double input)
{
	static double z1 = 0.0;

	double output = (input * (1 - A)) + z1;

	z1 = output * A;

	return output;
}

#endif // APP_BETA_LABS
