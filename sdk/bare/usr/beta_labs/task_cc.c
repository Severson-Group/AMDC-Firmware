#ifdef APP_BETA_LABS

#include "task_cc.h"
#include "inverter.h"
#include "task_mo.h"
#include "co.h"
#include "machine.h"
#include "cmd/cmd_cc.h"
#include "../../sys/debug.h"
#include "../../sys/defines.h"
#include "../../sys/scheduler.h"
#include "../../sys/injection.h"
#include "../../sys/transform.h"
#include "../../drv/analog.h"
#include "../../drv/encoder.h"
#include "../../drv/io.h"
#include "../../drv/dac.h"
#include "../../drv/pwm.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define Wb		(controller_bw * PI2) // rad/s
#define Ts		(1.0 / TASK_CC_UPDATES_PER_SEC)
#define Kp_d	(Wb * Ld_HAT)
#define Kp_q	(Wb * Lq_HAT)
#define Ki_d	((Rs_HAT / Ld_HAT) * Kp_d)
#define Ki_q	((Rs_HAT / Lq_HAT) * Kp_q)

// Variables for logging
double LOG_Id = 0.0;
double LOG_Iq = 0.0;
double LOG_Id_star = 0.0;
double LOG_Iq_star = 0.0;
double LOG_Vd_star = 0.0;
double LOG_Vq_star = 0.0;
double LOG_omega_e_avg  = 0.0;
double LOG_Id_hat_next = 0.0;
double LOG_Iq_hat_next = 0.0;
double LOG_theta_e_enc = 0.0;
double LOG_theta_e_hat = 0.0;
double LOG_Esal_d_hat = 0.0;
double LOG_Esal_q_hat = 0.0;

// Commands for Id and Iq -- Idq*
static double Id_star = 0.0;
static double Iq_star = 0.0;

static int32_t dq_offset = 9980;

// Note: user should override this initial value
static double controller_bw = 1.0;

static uint8_t theta_src_use_encoder = 1;

// Static variables for controller
static double Id_err_acc;
static double Iq_err_acc;
static double theta_e_enc;
static double theta_e_hat;

// Injection contexts for
// current controller
inj_ctx_t cc_inj_ctx_Id_star;
inj_ctx_t cc_inj_ctx_Iq_star;
inj_ctx_t cc_inj_ctx_Vd_star;
inj_ctx_t cc_inj_ctx_Vq_star;

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

static task_control_block_t tcb;


uint8_t task_cc_is_inited(void)
{
	return scheduler_tcb_is_registered(&tcb);
}

void task_cc_init(void)
{
	// Register task with scheduler
	scheduler_tcb_init(&tcb, task_cc_callback, NULL, "cc", TASK_CC_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);

	// Initialize all cc signal injection points
	injection_ctx_init(&cc_inj_ctx_Id_star, "Id*");
	injection_ctx_init(&cc_inj_ctx_Iq_star, "Iq*");
	injection_ctx_init(&cc_inj_ctx_Vd_star, "Vd*");
	injection_ctx_init(&cc_inj_ctx_Vq_star, "Vq*");

	// Register all cc signal injection points
	injection_ctx_register(&cc_inj_ctx_Id_star);
	injection_ctx_register(&cc_inj_ctx_Iq_star);
	injection_ctx_register(&cc_inj_ctx_Vd_star);
	injection_ctx_register(&cc_inj_ctx_Vq_star);

	// Clear controller static variables
	Id_err_acc = 0.0;
	Iq_err_acc = 0.0;
	theta_e_enc = 0.0;
}

void task_cc_deinit(void)
{
	// Unregister task with scheduler
	scheduler_tcb_unregister(&tcb);

	// Unregister all cc signal injection points
	injection_ctx_unregister(&cc_inj_ctx_Id_star);
	injection_ctx_unregister(&cc_inj_ctx_Iq_star);
	injection_ctx_unregister(&cc_inj_ctx_Vd_star);
	injection_ctx_unregister(&cc_inj_ctx_Vq_star);

	// Clear all injection points
	injection_ctx_clear(&cc_inj_ctx_Id_star);
	injection_ctx_clear(&cc_inj_ctx_Iq_star);
	injection_ctx_clear(&cc_inj_ctx_Vd_star);
	injection_ctx_clear(&cc_inj_ctx_Vq_star);

	// Clear controller static variables
	Id_err_acc = 0.0;
	Iq_err_acc = 0.0;
	theta_e_enc = 0.0;
}

static void _get_theta_da(double *theta_da)
{
	// Get raw encoder position
	uint32_t position;
	encoder_get_position(&position);

	// Add offset (align to DQ frame)
	position += dq_offset;

	while (position >= ENCODER_PULSES_PER_REV) {
		position -= ENCODER_PULSES_PER_REV;
	}

	// Convert to radians
	*theta_da = (double)PI2 * ((double)position / (double)ENCODER_PULSES_PER_REV);

	// Multiple by pole pairs to convert mechanical to electrical degrees
	*theta_da *= POLE_PAIRS;

	// Mod by 2 pi
	while (*theta_da > PI2) {
		*theta_da -= PI2;
	}
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

void task_cc_callback(void *arg)
{
	// -------------------
	// Inject signals into Idq*
	// (constants, chirps, noise, etc)
	// -------------------
	injection_inj(&Id_star, &cc_inj_ctx_Id_star, Ts);
	injection_inj(&Iq_star, &cc_inj_ctx_Iq_star, Ts);


	// -------------------
	// Update theta_e from encoder
	// -------------------
	_get_theta_da(&theta_e_enc);


	// -------------------
	// Pick theta_e source for rest of system
	// -------------------
	double theta_e = 0.0;
	if (theta_src_use_encoder) {
		theta_e = theta_e_enc;
	} else {
		theta_e = theta_e_hat;
	}



	// ------------------------------
	// Update omega_e_avg in rads/sec
	// ------------------------------
	double omega_e_avg;
	task_mo_get_omega_e(&omega_e_avg);
	// TODO: update this to use theta_e_hat...

	// ----------------------
	// Get current values
	// ----------------------
	double Iabc[3];
	_get_Iabc(Iabc);


	// ---------------------
	// Convert ABC to DQ
	// ---------------------
	double Ixyz[3];  // alpha beta gamma currents
	double Idq0[3]; // d q 0 currents
	transform_clarke(TRANS_DQZ_C_INVARIANT_POWER, Iabc, Ixyz);
	transform_park(theta_e, Ixyz, Idq0);


	// -----------------------------
	// Run through block diagram of CVCR to get Vdq_star
	// -----------------------------

	double Id = Idq0[0];
	double Iq = Idq0[1];

	// d-axis
	double Id_err;
	double Vd_star;
	Id_err = Id_star - Id;
	Id_err_acc += Id_err;
	Vd_star = (Kp_d * Id_err) + (Ki_d * Ts * Id_err_acc) - (omega_e_avg * Kp_q * Ts * Iq_err_acc);

	// q-axis
	double Iq_err;
	double Vq_star;
	Iq_err = Iq_star - Iq;
	Iq_err_acc += Iq_err;
	Vq_star = (Kp_q * Iq_err) + (Ki_q * Ts * Iq_err_acc) + (omega_e_avg * Kp_d * Ts * Id_err_acc) + (omega_e_avg * Lambda_pm_HAT);


	// -------------------
	// Inject signals into Vdq*
	// (constants, chirps, noise, etc)
	// -------------------
	injection_inj(&Vd_star, &cc_inj_ctx_Vd_star, Ts);
	injection_inj(&Vq_star, &cc_inj_ctx_Vq_star, Ts);


	// --------------------------------
	// Perform inverse DQ transform of Vdq_star
	// --------------------------------

	double Vabc_star[3];
	double Vdq0[3];
	Vdq0[0] = Vd_star;
	Vdq0[1] = Vq_star;
	Vdq0[2] = 0.0;
	transform_dqz_inverse(TRANS_DQZ_C_INVARIANT_POWER,
			theta_e, Vabc_star, Vdq0);


	// ------------------------------------
	// Saturate Vabc_star to inverter bus voltage
	// ------------------------------------

	inverter_saturate_to_Vdc(&Vabc_star[0]);
	inverter_saturate_to_Vdc(&Vabc_star[1]);
	inverter_saturate_to_Vdc(&Vabc_star[2]);


	// --------------------------------------
	// Write voltages out to PWM hardware
	// --------------------------------------

	inverter_set_voltage(CC_PHASE_A_PWM_LEG_IDX, Vabc_star[0], Iabc[0]);
	inverter_set_voltage(CC_PHASE_B_PWM_LEG_IDX, Vabc_star[1], Iabc[1]);
	inverter_set_voltage(CC_PHASE_C_PWM_LEG_IDX, Vabc_star[2], Iabc[2]);


	// -------------------
	// Store LOG variables
	// -------------------
	LOG_omega_e_avg = omega_e_avg;
	LOG_Vd_star = Vd_star;
	LOG_Vq_star = Vq_star;
	LOG_Id_star = Id_star;
	LOG_Iq_star = Iq_star;
	LOG_Id = Id;
	LOG_Iq = Iq;



	// -------------------
	// Self-sensing stuff:
	// -------------------

	// Back EMF filter thing
	co_update(Idq0, Vdq0, omega_e_avg);
	double Id_fund_hat_next, Iq_fund_hat_next;
	co_get_Idq_hat(&Id_fund_hat_next, &Iq_fund_hat_next);

	LOG_Id_hat_next = Id_fund_hat_next;
	LOG_Iq_hat_next = Iq_fund_hat_next;

	// Pull out Esal (sync ref frame)
	double Esal_d_hat, Esal_q_hat;
	co_get_Esal_hat(&Esal_d_hat, &Esal_q_hat);

	// TODO: show that Esal on q is there, not d
	// TODO: Esal_q should be around (lambda_pm * omega_e)

	// Convert Esal to alpha-beta ref frame
	// (NOTE: this is hacking the transform_park(...) function,
	//        since I didn't have an inverse one. Just rotate by -theta)
	double Esal_dq0[3];
	double Esal_xyz[3];
	Esal_dq0[0] = Esal_d_hat;
	Esal_dq0[1] = Esal_q_hat;
	Esal_dq0[2] = 0.0;
	transform_park(-theta_e, Esal_dq0, Esal_xyz);
	double Esal_alpha = Esal_xyz[0];
	double Esal_beta  = Esal_xyz[1];

	LOG_Esal_d_hat = Esal_alpha;
	LOG_Esal_q_hat = Esal_beta;

	// TODO: take arctan of Esal and see if that is about theta_e
	// Should work downish to 2-3Hz
	theta_e_hat = -atan2(Esal_alpha, Esal_beta);
	theta_e_hat += PI;

	LOG_theta_e_hat = theta_e_hat;
	LOG_theta_e_enc = theta_e_enc;

	dac_set_output(0, theta_e_enc, 0.0, PI2);
	dac_set_output(1, theta_e_hat, 0.0, PI2);
}

void task_cc_clear(void)
{
	Id_star = 0.0;
	Iq_star = 0.0;

	inverter_set_voltage(CC_PHASE_A_PWM_LEG_IDX, 0.0, 0.0);
	inverter_set_voltage(CC_PHASE_B_PWM_LEG_IDX, 0.0, 0.0);
	inverter_set_voltage(CC_PHASE_C_PWM_LEG_IDX, 0.0, 0.0);
}

void task_cc_set_dq_offset(int32_t offset) {
	dq_offset = offset;
}

void task_cc_set_bw(double bw)
{
	controller_bw = bw;
}

void task_cc_set_Iq_star(double value) {
	Iq_star = value;
}

void task_cc_set_Id_star(double value) {
	Id_star = value;
}

void task_cc_set_theta_src(uint8_t use_encoder)
{
	theta_src_use_encoder = use_encoder;
}

#endif // APP_BETA_LABS
