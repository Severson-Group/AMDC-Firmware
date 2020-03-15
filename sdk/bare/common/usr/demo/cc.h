#ifndef CC_H
#define CC_H

#include "usr/custom_types.h"

typedef struct cc_ctx_t {
    // Controller references
    vec_dq_t Idq_star;

    // Private controller tuning
    vec_dq_t __Kp;
    vec_dq_t __Ki;

    // Private controller state
    double __Ts;
    vec_dq_t __Vdq_star;
    vec_dq_t __Idq_err;
    vec_dq_t __Idq_err_acc;
} cc_ctx_t;

// Initialize current control context.
// Tune PI controller gains using estimated machine parameters.
void cc_init(cc_ctx_t *ctx, double Ts, double Rs_hat, double Ld_hat, double Lq_hat, double bandwidth_hz);

// Update current control context (perform PI control loop).
// NOTE: Application must call this function every Ts seconds
vec_dq_t cc_update(cc_ctx_t *ctx, vec_dq_t Idq, double omega_e);

#endif // CC_H
