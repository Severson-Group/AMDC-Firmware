#ifdef APP_DEMO

#include "usr/demo/task_control.h"
#include "drv/analog.h"
#include "drv/pwm.h"
#include "sys/scheduler.h"
#include "sys/transform.h"
#include "usr/demo/cc.h"
#include "usr/vector_types.h"
//#include "drv/led.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct pwm_config_t {
    uint8_t pwm_chnl;
} pwm_config_t;

typedef struct adc_config_t {
    uint8_t adc_chnl;
    double adc_gain;
    double adc_offset;
} adc_config_t;

typedef struct inverter_ctx_t {
    double Vdc;
} inverter_ctx_t;

typedef struct control_context_t {
    // Scheduler TCB which holds task "context"
    task_control_block_t tcb;

    adc_config_t adc_config[3];
    pwm_config_t pwm_config[3];

    inverter_ctx_t inverter_ctx;

    cc_ctx_t cc_ctx;

    double omega_e_star;
    double __theta_e;
} control_context_t;

// Period between controller updates
const double Ts = 1.0 / TASK_CONTROL_UPDATES_PER_SEC;

#define MAX_NUM_CTRL_CTXS (8)
static control_context_t ctrl_ctxs[MAX_NUM_CTRL_CTXS] = { 0 };

static vec_dq_t my_transform_dq(vec_abc_t Fabc, double theta);
static vec_abc_t my_transform_dq_inverse(vec_dq_t Fdq, double theta);
inline static int saturate(double min, double max, double *value);

void task_control_callback(void *arg)
{
    // Cast scheduler-supplied "arg" into the controller context
    // NOTE: We can do this because of how we inited our callback,
    //       see task_control_init() below
    control_context_t *ctx = (control_context_t *) arg;

    // Update position based on user specified speed command
    ctx->__theta_e += ctx->omega_e_star * Ts;
    while (ctx->__theta_e > PI2)
        ctx->__theta_e -= PI2;
    while (ctx->__theta_e < -PI2)
        ctx->__theta_e += PI2;

    // Read ADCs
    vec_abc_t Iabc;
    for (size_t i = 0; i < 3; i++) {
        float I;
        analog_getf(ctx->adc_config[i].adc_chnl, &I);
        Iabc.elems[i] = ((double) I * ctx->adc_config[i].adc_gain) + ctx->adc_config[i].adc_offset;
    }

    // Convert ABC to DQ
    vec_dq_t Idq = my_transform_dq(Iabc, ctx->__theta_e);

    // Use CC module to get Vdq*
    vec_dq_t Vdq_star = cc_update(&ctx->cc_ctx, Idq, 0.0);

    // Perform inverse DQ transform of Vdq*
    vec_abc_t Vabc_star = my_transform_dq_inverse(Vdq_star, ctx->__theta_e);

    // Saturate Vabc* to 1/2 inverter bus voltage
    if (ctx->inverter_ctx.Vdc < 1.0)
        ctx->inverter_ctx.Vdc = 1.0;
    double Vdc = ctx->inverter_ctx.Vdc;
    for (size_t i = 0; i < 3; i++) {
        if (saturate(-Vdc / 2.0, Vdc / 2.0, &Vabc_star.elems[i]) != 0) {
            // Saturated!
            // led_set_color_bytes(0, 10, 0, 0);
        } else {
            // led_set_color_bytes(0, 0, 10, 0);
        }
    }

    // Write voltages out as PWM using sine-triangle modulation
    for (size_t i = 0; i < 3; i++) {
        double duty = 0.5 + (Vabc_star.elems[i] / Vdc);
        pwm_set_duty(ctx->pwm_config[i].pwm_chnl, duty);
    }
}

void task_control_init(int ctrl_idx)
{
    if (ctrl_idx < 0 || ctrl_idx >= MAX_NUM_CTRL_CTXS)
        return;
    control_context_t *ctx = &ctrl_ctxs[ctrl_idx];

    if (!scheduler_tcb_is_registered(&ctx->tcb)) {
        scheduler_tcb_init(&ctx->tcb, task_control_callback, ctx, "ctrl", TASK_CONTROL_INTERVAL_USEC);
        scheduler_tcb_register(&ctx->tcb);
    }
}

void task_control_deinit(int ctrl_idx)
{
    if (ctrl_idx < 0 || ctrl_idx >= MAX_NUM_CTRL_CTXS)
        return;
    control_context_t *ctx = &ctrl_ctxs[ctrl_idx];

    if (scheduler_tcb_is_registered(&ctx->tcb)) {
        scheduler_tcb_unregister(&ctx->tcb);
    }

    // Set all half-bridges to 0.5 duty ratio
    for (size_t i = 0; i < 3; i++) {
        pwm_set_duty(ctx->pwm_config[i].pwm_chnl, 0.5);
    }
}

void task_control_vdc_set(int ctrl_idx, double vdc)
{
    if (ctrl_idx < 0 || ctrl_idx >= MAX_NUM_CTRL_CTXS)
        return;
    control_context_t *ctx = &ctrl_ctxs[ctrl_idx];

    ctx->inverter_ctx.Vdc = vdc;
}

void task_control_pwm(int ctrl_idx, uint8_t phase, uint8_t pwm_chnl)
{
    if (ctrl_idx < 0 || ctrl_idx >= MAX_NUM_CTRL_CTXS)
        return;
    control_context_t *ctx = &ctrl_ctxs[ctrl_idx];

    ctx->pwm_config[phase].pwm_chnl = pwm_chnl;
}

void task_control_adc(int ctrl_idx, uint8_t phase, uint8_t adc_chnl, double adc_gain, double adc_offset)
{
    if (ctrl_idx < 0 || ctrl_idx >= MAX_NUM_CTRL_CTXS)
        return;
    control_context_t *ctx = &ctrl_ctxs[ctrl_idx];

    ctx->adc_config[phase].adc_chnl = adc_chnl;
    ctx->adc_config[phase].adc_gain = adc_gain;
    ctx->adc_config[phase].adc_offset = adc_offset;
}

void task_control_tune(int ctrl_idx, double Rs_hat, double Ld_hat, double Lq_hat, double bandwidth_hz)
{
    if (ctrl_idx < 0 || ctrl_idx >= MAX_NUM_CTRL_CTXS)
        return;
    control_context_t *ctx = &ctrl_ctxs[ctrl_idx];

    cc_init(&ctx->cc_ctx, Ts, Rs_hat, Ld_hat, Lq_hat, bandwidth_hz);
}

void task_control_set(int ctrl_idx, double Id_star, double Iq_star, double omega_e)
{
    if (ctrl_idx < 0 || ctrl_idx >= MAX_NUM_CTRL_CTXS)
        return;
    control_context_t *ctx = &ctrl_ctxs[ctrl_idx];

    ctx->cc_ctx.Idq_star.d = Id_star;
    ctx->cc_ctx.Idq_star.q = Iq_star;

    ctx->omega_e_star = omega_e;

    if (ctx->omega_e_star == 0.0) {
        ctx->__theta_e = 0.0;
    }
}

static vec_dq_t my_transform_dq(vec_abc_t Fabc, double theta)
{
    double abc[3];
    abc[0] = Fabc.a;
    abc[1] = Fabc.b;
    abc[2] = Fabc.c;
    double dq0[3];

    transform_dqz(TRANS_DQZ_C_INVARIANT_AMPLITUDE, theta, abc, dq0);

    vec_dq_t ret;
    ret.d = dq0[0];
    ret.q = dq0[1];
    return ret;
}

static vec_abc_t my_transform_dq_inverse(vec_dq_t Fdq, double theta)
{
    double abc[3];
    double dq0[3];
    dq0[0] = Fdq.d;
    dq0[1] = Fdq.q;
    dq0[2] = 0.0;

    transform_dqz_inverse(TRANS_DQZ_C_INVARIANT_AMPLITUDE, theta, abc, dq0);

    vec_abc_t ret;
    ret.a = abc[0];
    ret.b = abc[1];
    ret.c = abc[2];
    return ret;
}

inline static int saturate(double min, double max, double *value)
{
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

#endif // APP_DEMO
