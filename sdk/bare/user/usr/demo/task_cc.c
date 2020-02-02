#ifdef APP_DEMO

#include "usr/demo/task_cc.h"
#include "usr/demo/inverter.h"
#include "sys/scheduler.h"
#include "sys/transform.h"
#include "drv/analog.h"
#include <stdint.h>

typedef struct pwm_config_t {
	uint8_t pwm_chnl;
} pwm_config_t;

typedef struct adc_config_t {
	uint8_t adc_chnl;
	double adc_gain;
	double adc_offset;
} adc_config_t;

typedef struct vec_dq_t {
	double d;
	double q;
} vec_dq_t;

typedef struct vec_abc_t {
	double a;
	double b;
	double c;
} vec_abc_t;

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

// Configuration set by user for ADC/PWM
static adc_config_t adc_config[3] = {0};
static pwm_config_t pwm_config[3] = {0};

// Tunings for controller
static vec_dq_t Kp = {0};
static vec_dq_t Ki = {0};

// Period between controller updates
const double Ts =  1.0 / TASK_CC_UPDATES_PER_SEC;

// Controller references
static double cc_Id_star = 0.0;
static double cc_Iq_star = 0.0;
static double cc_omega_e = 0.0;

// Default DC link voltage to 1V -- user will override this
static double cc_vdc = 1.0;

// Controller state
static vec_dq_t Idq_err = {0};
static vec_dq_t Idq_err_acc = {0};
static double theta_e = 0.0;

void task_cc_init(void)
{
    scheduler_tcb_init(&tcb, task_cc_callback, NULL, "cc", TASK_CC_INTERVAL_USEC);
    scheduler_tcb_register(&tcb);
}

void task_cc_deinit(void)
{
    scheduler_tcb_unregister(&tcb);
}

void task_cc_callback(void *arg)
{
	// ---------------------
	// Update position based on user specified speed
	// ---------------------
	theta_e += cc_omega_e * Ts;
	if (theta_e > PI2) theta_e -= PI2;
	if (theta_e < -PI2) theta_e += PI2;

	// ---------------------
	// Read currents from ADC inputs
	// ---------------------

    // Read from ADCs
    float Iabc_f[3];
    for (uint8_t i = 0; i < 3; i++) {
    	analog_getf(adc_config[i].adc_chnl, &Iabc_f[i]);
    }

    // Convert ADC values to raw currents
	vec_abc_t Iabc;
    Iabc.a = ((double) Iabc_f[0] * adc_config[0].adc_gain) + adc_config[0].adc_offset;
    Iabc.b = ((double) Iabc_f[1] * adc_config[1].adc_gain) + adc_config[1].adc_offset;
    Iabc.c = ((double) Iabc_f[2] * adc_config[2].adc_gain) + adc_config[2].adc_offset;


	// ---------------------
	// Convert ABC to DQ
	// ---------------------
	double Iabc1[3];
	Iabc1[0] = Iabc.a;
	Iabc1[1] = Iabc.b;
	Iabc1[2] = Iabc.c;
	double Ixyz[3]; // alpha beta gamma currents
	double Idq0[3]; // d q 0 currents
	transform_clarke(TRANS_DQZ_C_INVARIANT_AMPLITUDE, Iabc1, Ixyz);
	transform_park(theta_e, Ixyz, Idq0);


	// -----------------------------
	// Run through block diagram of CVCR to get Vdq*
	// -----------------------------
	vec_dq_t Vdq_star;

	vec_dq_t Idq;
	Idq.d = Idq0[0];
	Idq.q = Idq0[1];

	// d-axis
	Idq_err.d = cc_Id_star - Idq.d;
	Idq_err_acc.d += Idq_err.d;
	Vdq_star.d = (Kp.d * Idq_err.d) + (Ki.d * Ts * Idq_err_acc.d);

	// q-axis
	Idq_err.q = cc_Iq_star - Idq.q;
	Idq_err_acc.q += Idq_err.q;
	Vdq_star.q = (Kp.q * Idq_err.q) + (Ki.q * Ts * Idq_err_acc.q);


    // --------------------------------
    // Perform inverse DQ transform of Vdq_star
    // --------------------------------
    double Vabc_star1[3];
    double Vdq0[3];
    Vdq0[0] = Vdq_star.d;
    Vdq0[1] = Vdq_star.q;
    Vdq0[2] = 0.0;
    transform_dqz_inverse(TRANS_DQZ_C_INVARIANT_AMPLITUDE,
            theta_e, Vabc_star1, Vdq0);
    vec_abc_t Vabc_star;
    Vabc_star.a = Vabc_star1[0];
    Vabc_star.b = Vabc_star1[1];
    Vabc_star.c = Vabc_star1[2];


    // ------------------------------------
    // Saturate Vabc_star to inverter bus voltage
    // ------------------------------------
    inverter_saturate_to_Vdc(&Vabc_star.a);
    inverter_saturate_to_Vdc(&Vabc_star.b);
    inverter_saturate_to_Vdc(&Vabc_star.c);


    // --------------------------------------
    // Write voltages out to PWM hardware
    // --------------------------------------
    inverter_set_voltage(pwm_config[0].pwm_chnl, Vabc_star.a);
    inverter_set_voltage(pwm_config[1].pwm_chnl, Vabc_star.b);
    inverter_set_voltage(pwm_config[2].pwm_chnl, Vabc_star.c);
}

void task_cc_vdc_set(double vdc)
{
	cc_vdc = vdc;
}

void task_cc_pwm(uint8_t phase, uint8_t pwm_chnl)
{
	pwm_config[phase].pwm_chnl = pwm_chnl;
}

void task_cc_adc(uint8_t phase, uint8_t adc_chnl, double adc_gain, double adc_offset)
{
	adc_config[phase].adc_chnl = adc_chnl;
	adc_config[phase].adc_gain = adc_gain;
	adc_config[phase].adc_offset = adc_offset;
}

void task_cc_tune(double Rs, double Ld, double Lq, double bw)
{
	Kp.d = bw * Ld;
	Kp.q = bw * Lq;
	Ki.d = Kp.d * (Rs / Ld);
	Ki.q = Kp.q * (Rs / Lq);
}

void task_cc_set(double Id_star, double Iq_star, double omega_e)
{
	cc_Id_star = Id_star;
	cc_Iq_star = Iq_star;
	cc_omega_e = omega_e;

	if (cc_omega_e == 0.0) {
		theta_e = 0.0;
	}
}

#endif // APP_DEMO
