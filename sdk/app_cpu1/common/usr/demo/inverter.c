#ifdef APP_DEMO

#include "usr/demo/inverter.h"
#include "drv/hardware_targets.h"
#include "drv/io.h"
#include "drv/pwm.h"
#include "usr/user_config.h"
#include <math.h>

typedef struct inverter_ctx_t {
    double Vdc;
} inverter_ctx_t;

#define MAX_NUM_INVERTER_CTXS (8)
static inverter_ctx_t inverter_ctxs[MAX_NUM_INVERTER_CTXS] = { 0 };

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

void inverter_saturate_to_Vdc(int inv_idx, double *phase_voltage)
{
    if (inv_idx < 0 || inv_idx >= MAX_NUM_INVERTER_CTXS) {
        return;
    }

    inverter_ctx_t *ctx = &inverter_ctxs[inv_idx];

    // Enforce voltage is positive so we can divide by it... User should have set this up (i.e. > 1V)
    if (ctx->Vdc < 1.0) {
        ctx->Vdc = 1.0;
    }

    saturate(-ctx->Vdc / 2.0, ctx->Vdc / 2.0, phase_voltage);
}

void inverter_set_voltage(int inv_idx, uint8_t pwm_idx, double phase_voltage)
{
    if (inv_idx < 0 || inv_idx >= MAX_NUM_INVERTER_CTXS) {
        return;
    }

    inverter_ctx_t *ctx = &inverter_ctxs[inv_idx];

    // Enforce voltage is positive so we can divide by it... User should have set this up (i.e. > 1V)
    if (ctx->Vdc < 1.0) {
        ctx->Vdc = 1.0;
    }

    double duty = 0.5 + (phase_voltage / ctx->Vdc);
    pwm_set_duty(pwm_idx, duty);
}

void inverter_set_Vdc(int inv_idx, double Vdc)
{
    if (inv_idx < 0 || inv_idx >= MAX_NUM_INVERTER_CTXS) {
        return;
    }

    inverter_ctx_t *ctx = &inverter_ctxs[inv_idx];

    ctx->Vdc = Vdc;
}

double inverter_get_Vdc(int inv_idx)
{
    if (inv_idx < 0 || inv_idx >= MAX_NUM_INVERTER_CTXS) {
        return -1.0;
    }

    inverter_ctx_t *ctx = &inverter_ctxs[inv_idx];

    return ctx->Vdc;
}

#endif // APP_DEMO
