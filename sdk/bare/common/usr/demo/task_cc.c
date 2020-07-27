#ifdef APP_DEMO

#include "usr/demo/task_cc.h"
#include "drv/analog.h"
#include "sys/scheduler.h"
#include "sys/transform.h"
#include "usr/demo/inverter.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct cc_context_t {
    // Scheduler TCB which holds task "context"
    task_control_block_t tcb;

    adc_config_t adc_config[3];
    pwm_config_t pwm_config[3];

    vec_dq_t Kp;
    vec_dq_t Ki;

    // Controller references
    double Id_star;
    double Iq_star;
    double omega_e;

    // Controller state
    vec_dq_t Idq_err;
    vec_dq_t Idq_err_acc;
    double theta_e;
    enum setmode { MODE_ANGLE, MODE_SPEED } mode;
} cc_context_t;

// Period between controller updates
const double Ts = 1.0 / TASK_CC_UPDATES_PER_SEC;

#define MAX_NUM_CC_CTXS (8)
static cc_context_t cc_ctxs[MAX_NUM_CC_CTXS] = { 0 };

static inline bool STREQ(char *in1, char *in2)
{
    return (strcmp(in1, in2) == 0) ? true : false;
}

void task_cc_init(int cc_idx)
{
    if (cc_idx < 0 || cc_idx >= MAX_NUM_CC_CTXS) {
        return;
    }

    cc_context_t *ctx = &cc_ctxs[cc_idx];

    if (!scheduler_tcb_is_registered(&ctx->tcb)) {
        scheduler_tcb_init(&ctx->tcb, task_cc_callback, ctx, "cc", TASK_CC_INTERVAL_USEC);
        scheduler_tcb_register(&ctx->tcb);
    }
    ctx->mode = MODE_SPEED;
}

void task_cc_deinit(int cc_idx)
{
    if (cc_idx < 0 || cc_idx >= MAX_NUM_CC_CTXS) {
        return;
    }

    cc_context_t *ctx = &cc_ctxs[cc_idx];

    if (scheduler_tcb_is_registered(&ctx->tcb)) {
        scheduler_tcb_unregister(&ctx->tcb);
    }

    inverter_set_voltage(cc_idx, ctx->pwm_config[0].pwm_chnl, 0.0);
    inverter_set_voltage(cc_idx, ctx->pwm_config[1].pwm_chnl, 0.0);
    inverter_set_voltage(cc_idx, ctx->pwm_config[2].pwm_chnl, 0.0);
}

void task_cc_setmode(int cc_idx, char *argMode)
{
    if (cc_idx < 0 || cc_idx >= MAX_NUM_CC_CTXS) {
        return;
    }

    cc_context_t *ctx = &cc_ctxs[cc_idx];

    if (STREQ("angle", argMode))
        ctx->mode = MODE_ANGLE;
    else if (STREQ("speed", argMode))
        ctx->mode = MODE_SPEED;
}
void task_cc_callback(void *arg)
{
    cc_context_t *ctx = (cc_context_t *) arg;
    int cc_idx = 0;
    while (&cc_ctxs[cc_idx] != ctx)
        cc_idx++;

    // ---------------------
    // Update position based on user specified speed
    // ---------------------
    if (ctx->mode == MODE_SPEED) {
        ctx->theta_e += ctx->omega_e * Ts;
    }
    if (ctx->theta_e > PI2)
        ctx->theta_e -= PI2;
    if (ctx->theta_e < -PI2)
        ctx->theta_e += PI2;

    // ---------------------
    // Read currents from ADC inputs
    // ---------------------

    // Read from ADCs
    float Iabc_f[3];
    for (uint8_t i = 0; i < 3; i++) {
        analog_getf(ctx->adc_config[i].adc_chnl, &Iabc_f[i]);
    }

    // Convert ADC values to raw currents
    vec_abc_t Iabc;
    Iabc.a = ((double) Iabc_f[0] * ctx->adc_config[0].adc_gain) + ctx->adc_config[0].adc_offset;
    Iabc.b = ((double) Iabc_f[1] * ctx->adc_config[1].adc_gain) + ctx->adc_config[1].adc_offset;
    Iabc.c = ((double) Iabc_f[2] * ctx->adc_config[2].adc_gain) + ctx->adc_config[2].adc_offset;

    // ---------------------
    // Convert ABC to DQ
    // ---------------------
    double Iabc1[3];
    Iabc1[0] = Iabc.a;
    Iabc1[1] = Iabc.b;
    Iabc1[2] = Iabc.c;
    double Ixyz[3]; // alpha beta gamma currents
    double Idq0[3]; // d q 0 currents
    transform_dqz(TRANS_DQZ_C_INVARIANT_AMPLITUDE, ctx->theta_e, Iabc1, Idq0);

    // -----------------------------
    // Run through block diagram of CVCR to get Vdq*
    // -----------------------------
    vec_dq_t Vdq_star;

    vec_dq_t Idq;
    Idq.d = Idq0[0];
    Idq.q = Idq0[1];

    // d-axis
    ctx->Idq_err.d = ctx->Id_star - Idq.d;
    ctx->Idq_err_acc.d += ctx->Idq_err.d;
    Vdq_star.d = (ctx->Kp.d * ctx->Idq_err.d) + (ctx->Ki.d * Ts * ctx->Idq_err_acc.d);

    // q-axis
    ctx->Idq_err.q = ctx->Iq_star - Idq.q;
    ctx->Idq_err_acc.q += ctx->Idq_err.q;
    Vdq_star.q = (ctx->Kp.q * ctx->Idq_err.q) + (ctx->Ki.q * Ts * ctx->Idq_err_acc.q);

    // --------------------------------
    // Perform inverse DQ transform of Vdq_star
    // --------------------------------
    double Vabc_star1[3];
    double Vdq0[3];
    Vdq0[0] = Vdq_star.d;
    Vdq0[1] = Vdq_star.q;
    Vdq0[2] = 0.0;
    transform_dqz_inverse(TRANS_DQZ_C_INVARIANT_AMPLITUDE, ctx->theta_e, Vabc_star1, Vdq0);
    vec_abc_t Vabc_star;
    Vabc_star.a = Vabc_star1[0];
    Vabc_star.b = Vabc_star1[1];
    Vabc_star.c = Vabc_star1[2];

    // ------------------------------------
    // Saturate Vabc_star to inverter bus voltage
    // ------------------------------------
    inverter_saturate_to_Vdc(cc_idx, &Vabc_star.a);
    inverter_saturate_to_Vdc(cc_idx, &Vabc_star.b);
    inverter_saturate_to_Vdc(cc_idx, &Vabc_star.c);

    // --------------------------------------
    // Write voltages out to PWM hardware
    // --------------------------------------
    inverter_set_voltage(cc_idx, ctx->pwm_config[0].pwm_chnl, Vabc_star.a);
    inverter_set_voltage(cc_idx, ctx->pwm_config[1].pwm_chnl, Vabc_star.b);
    inverter_set_voltage(cc_idx, ctx->pwm_config[2].pwm_chnl, Vabc_star.c);
}

void task_cc_vdc_set(int cc_idx, double vdc)
{
    inverter_set_Vdc(cc_idx, vdc);
}

void task_cc_pwm(int cc_idx, uint8_t phase, uint8_t pwm_chnl)
{
    if (cc_idx < 0 || cc_idx >= MAX_NUM_CC_CTXS) {
        return;
    }

    cc_context_t *ctx = &cc_ctxs[cc_idx];

    ctx->pwm_config[phase].pwm_chnl = pwm_chnl;
}

void task_cc_adc(int cc_idx, uint8_t phase, uint8_t adc_chnl, double adc_gain, double adc_offset)
{
    if (cc_idx < 0 || cc_idx >= MAX_NUM_CC_CTXS) {
        return;
    }

    cc_context_t *ctx = &cc_ctxs[cc_idx];

    ctx->adc_config[phase].adc_chnl = adc_chnl;
    ctx->adc_config[phase].adc_gain = adc_gain;
    ctx->adc_config[phase].adc_offset = adc_offset;
}

void task_cc_tune(int cc_idx, double Rs, double Ld, double Lq, double bw)
{
    if (cc_idx < 0 || cc_idx >= MAX_NUM_CC_CTXS) {
        return;
    }

    cc_context_t *ctx = &cc_ctxs[cc_idx];

    ctx->Kp.d = bw * Ld;
    ctx->Kp.q = bw * Lq;
    ctx->Ki.d = ctx->Kp.d * (Rs / Ld);
    ctx->Ki.q = ctx->Kp.q * (Rs / Lq);
}

void task_cc_set_currents(int cc_idx, double Id_star, double Iq_star)
{
    if (cc_idx < 0 || cc_idx >= MAX_NUM_CC_CTXS) {
        return;
    }

    cc_context_t *ctx = &cc_ctxs[cc_idx];

    ctx->Id_star = Id_star;
    ctx->Iq_star = Iq_star;
}

void task_cc_set_omega(int cc_idx, double omega_e)
{
    if (cc_idx < 0 || cc_idx >= MAX_NUM_CC_CTXS) {
        return;
    }

    cc_context_t *ctx = &cc_ctxs[cc_idx];

    if (ctx->mode == MODE_SPEED) {
        ctx->omega_e = omega_e;
        if (ctx->omega_e == 0.0) {
            ctx->theta_e = 0.0;
        }
    }
}

void task_cc_set_theta(int cc_idx, double theta_e)
{
    if (cc_idx < 0 || cc_idx >= MAX_NUM_CC_CTXS) {
        return;
    }

    cc_context_t *ctx = &cc_ctxs[cc_idx];

    if (ctx->mode == MODE_ANGLE) {
        ctx->theta_e = theta_e;
    }
}

#endif // APP_DEMO
