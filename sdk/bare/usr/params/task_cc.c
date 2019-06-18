#ifdef APP_PARAMS

#include "task_cc.h"
#include "inverter.h"
#include "machine.h"
#include "cmd/cmd_cc.h"
#include "../../sys/debug.h"
#include "../../sys/defines.h"
#include "../../sys/scheduler.h"
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

static double Id_star = 0.0;
static double Iq_star = 0.0;

static int32_t dq_offset = 9550; // 9661 from beta-axis injection

static double controller_bw = 1.0;

static double Id_err_acc = 0.0;
static double Iq_err_acc = 0.0;

static double theta_da = 0.0;

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
}

void task_cc_deinit(void)
{
	scheduler_tcb_unregister(&tcb);
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

static void _get_omega_e_avg(double *output) {
	static double latched_output = 0.0;

	static int32_t last_steps = 0;
	static int count = 0;
	if (++count > 50) {
		count = 0;
		int32_t steps;
		encoder_get_steps(&steps);
		int32_t delta = steps - last_steps;
		double rads = PI2 * ((double) delta / (double) (1 << ENCODER_PULSES_PER_REV_BITS));
		rads /= 50.0;

		last_steps = steps;

		// Update log variables
		*output = rads * (double) TASK_CC_UPDATES_PER_SEC * POLE_PAIRS;
		latched_output = *output;
	} else {
		*output = latched_output;
	}

	// TODO: when we implement the motion controller,
	// we will have a real estimate of speed...
	//
	// For now, we are doing testing at 0 speed, so just assume that.
//	*output = 0.0;
}


// Chirp function
//
// Generates the chirp signal value given:
// - time: current time instant
// - w1:   low freq (rad)
// - w2:   high freq (rad)
// - A:    amplitude
// - M:    time period
static inline double _chirp(double w1, double w2, double A, double M, double time)
{
	double out;
	out = A * cos(w1 * time + (w2 - w1) * time * time / (2 * M));
	return out;
}

typedef struct cc_inj_func_constant_t {
	double gain;
} cc_inj_func_constant_t;

typedef struct cc_inj_func_noise_t {
	double gain;
	double offset;
} cc_inj_func_noise_t;

typedef struct cc_inj_func_chirp_t {
	double gain;
	double freqMin;
	double freqMax;
	double period;
} cc_inj_func_chirp_t;

typedef struct cc_inj_ctx_t {
	cc_inj_func_e inj_func;
	cc_inj_op_e operation;

	cc_inj_func_constant_t constant;
	cc_inj_func_noise_t noise;
	cc_inj_func_chirp_t chirp;

	double curr_time;
} cc_inj_ctx_t;

// Injection contexts for system
cc_inj_ctx_t cc_inj_ctx_Id;
cc_inj_ctx_t cc_inj_ctx_Iq;
cc_inj_ctx_t cc_inj_ctx_Vd;
cc_inj_ctx_t cc_inj_ctx_Vq;

static void _inject_signal(double *output, cc_inj_ctx_t *inj_ctx)
{
	double value = 0.0;

	switch (inj_ctx->inj_func) {
	case CONST:
		value = inj_ctx->constant.gain;
		break;

	case NOISE:
	{
		// Generate random number between 0..1
		double r = (double) rand() / (double) RAND_MAX;

		// Make between -1.0 .. 1.0
		r = (2.0  * r) - 1.0;

		value = inj_ctx->noise.gain * r;
		value += inj_ctx->noise.offset;
		break;
	}
	case CHIRP:
	{
		inj_ctx->curr_time += Ts;
		if (inj_ctx->curr_time >= inj_ctx->chirp.period) {
			inj_ctx->curr_time = 0.0;
		}

		value = _chirp(
				PI2 * inj_ctx->chirp.freqMin,
				PI2 * inj_ctx->chirp.freqMax,
				inj_ctx->chirp.gain,
				inj_ctx->chirp.period,
				inj_ctx->curr_time
				);
		break;
	}

	case NONE:
	default:
		// Injection function not set by user,
		// so don't do anything to the output signal
		return;
	}

	// Perform operation to do injection
	switch (inj_ctx->operation) {
	case SET:
		*output = value;
		break;
	case ADD:
		*output += value;
		break;
	}
}

void task_cc_callback(void *arg)
{
	// -------------------
	// Inject signals into Idq*
	// (constants, chirps, noise, etc)
	// -------------------
	_inject_signal(&Id_star, &cc_inj_ctx_Id);
	_inject_signal(&Iq_star, &cc_inj_ctx_Iq);


	// -------------------
	// Update theta_da
	// -------------------
	_get_theta_da(&theta_da);


	// ------------------------------
	// Update omega_e_avg in rads/sec
	// ------------------------------
	double omega_e_avg;
	_get_omega_e_avg(&omega_e_avg);


	// ----------------------
	// Get current values
	// ----------------------
	double Iabc[3];
	_get_Iabc(Iabc);


	// ---------------------
	// Convert ABC to DQ
	// ---------------------
	double Idq0[3];
	transform_dqz(theta_da, Iabc, Idq0);


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
//	Vd_star = (Kp_d * Id_err) + (Ki_d * Ts * Id_err_acc) - (omega_e_avg * Kp_q * Ts * Iq_err_acc);
	Vd_star = (Kp_d * Id_err) + (Ki_d * Ts * Id_err_acc);

	// q-axis
	double Iq_err;
	double Vq_star;
	Iq_err = Iq_star - Iq;
	Iq_err_acc += Iq_err;
//	Vq_star = (Kp_q * Iq_err) + (Ki_q * Ts * Iq_err_acc) + (omega_e_avg * Kp_d * Ts * Id_err_acc) + (omega_e_avg * Lambda_pm_HAT);
	Vq_star = (Kp_q * Iq_err) + (Ki_q * Ts * Iq_err_acc);


	// -------------------
	// Inject signals into Vdq*
	// (constants, chirps, noise, etc)
	// -------------------
	_inject_signal(&Vd_star, &cc_inj_ctx_Vd);
	_inject_signal(&Vq_star, &cc_inj_ctx_Vq);


	// --------------------------------
	// Perform inverse DQ transform of Vdq_star
	// --------------------------------

	double Vabc_star[3];
	double Vdq0[3];
	Vdq0[0] = Vd_star;
	Vdq0[1] = Vq_star;
	Vdq0[2] = 0.0;
	transform_dqz_inverse(theta_da, Vabc_star, Vdq0);


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

static void _find_inj_ctx(cc_inj_value_e value, cc_inj_axis_e axis, cc_inj_ctx_t **inj_ctx)
{
	switch (value) {
	case CURRENT:
		switch (axis) {
		case D_AXIS:
			*inj_ctx = &cc_inj_ctx_Id;
			break;
		case Q_AXIS:
			*inj_ctx = &cc_inj_ctx_Iq;
			break;
		}
		break;

	case VOLTAGE:
		switch (axis) {
		case D_AXIS:
			*inj_ctx = &cc_inj_ctx_Vd;
			break;
		case Q_AXIS:
			*inj_ctx = &cc_inj_ctx_Vq;
			break;
		}
		break;
	}
}

void task_cc_inj_clear(void)
{
	cc_inj_ctx_Id.inj_func = NONE;
	cc_inj_ctx_Iq.inj_func = NONE;
	cc_inj_ctx_Vd.inj_func = NONE;
	cc_inj_ctx_Vq.inj_func = NONE;
}

void task_cc_inj_const(cc_inj_value_e value, cc_inj_axis_e axis, cc_inj_op_e op, double gain)
{
	cc_inj_ctx_t *inj_ctx;
	_find_inj_ctx(value, axis, &inj_ctx);

	inj_ctx->inj_func = CONST;
	inj_ctx->operation = op;
	inj_ctx->constant.gain = gain;
}

void task_cc_inj_noise(cc_inj_value_e value, cc_inj_axis_e axis, cc_inj_op_e op, double gain, double offset)
{
	cc_inj_ctx_t *inj_ctx;
	_find_inj_ctx(value, axis, &inj_ctx);

	inj_ctx->inj_func = NOISE;
	inj_ctx->operation = op;
	inj_ctx->noise.gain = gain;
	inj_ctx->noise.offset = offset;
}

void task_cc_inj_chirp(cc_inj_value_e value, cc_inj_axis_e axis, cc_inj_op_e op,
		double gain, double freqMin, double freqMax, double period)
{
	cc_inj_ctx_t *inj_ctx;
	_find_inj_ctx(value, axis, &inj_ctx);

	inj_ctx->inj_func = CHIRP;
	inj_ctx->operation = op;
	inj_ctx->chirp.gain = gain;
	inj_ctx->chirp.freqMin = freqMin;
	inj_ctx->chirp.freqMax = freqMax;
	inj_ctx->chirp.period = period;
}

#endif // APP_PARAMS
