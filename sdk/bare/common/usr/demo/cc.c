#ifdef APP_DEMO

#include "cc.h"
#include <math.h>

void cc_init(cc_ctx_t *ctx, double Ts, double Rs_hat, double Ld_hat, double Lq_hat, double bandwidth_hz)
{
    ctx->__Ts = Ts;

    // Calculate tuning
    double wb = 2.0 * M_PI * bandwidth_hz;
    ctx->__Kp.d = wb * Ld_hat;
    ctx->__Kp.q = wb * Lq_hat;
    ctx->__Ki.d = ctx->__Kp.d * (Rs_hat / Ld_hat);
    ctx->__Ki.q = ctx->__Kp.q * (Rs_hat / Lq_hat);

    // Zero controller references
    ctx->Idq_star.d = 0.0;
    ctx->Idq_star.q = 0.0;

    // Zero controller states
    ctx->__Vdq_star.d = 0.0;
    ctx->__Vdq_star.q = 0.0;
    ctx->__Idq_err.d = 0.0;
    ctx->__Idq_err.q = 0.0;
    ctx->__Idq_err_acc.d = 0.0;
    ctx->__Idq_err_acc.q = 0.0;
}

vec_dq_t cc_update(cc_ctx_t *ctx, vec_dq_t Idq, double omega_e)
{
    // d-axis
    ctx->__Idq_err.d = ctx->Idq_star.d - Idq.d;
    ctx->__Idq_err_acc.d += ctx->__Idq_err.d;
    ctx->__Vdq_star.d = (ctx->__Kp.d * ctx->__Idq_err.d) + (ctx->__Ki.d * ctx->__Ts * ctx->__Idq_err_acc.d);

    // q-axis
    ctx->__Idq_err.q = ctx->Idq_star.q - Idq.q;
    ctx->__Idq_err_acc.q += ctx->__Idq_err.q;
    ctx->__Vdq_star.q = (ctx->__Kp.q * ctx->__Idq_err.q) + (ctx->__Ki.q * ctx->__Ts * ctx->__Idq_err_acc.q);

    return ctx->__Vdq_star;
}


#endif // APP_DEMO
