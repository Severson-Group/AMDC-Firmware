#ifdef APP_BETA_LABS

#include "usr/beta_labs/co.h"
#include "usr/beta_labs/machine.h"

// Current observer gains assuming Ts = 5e-05
// Tuned to 10 Hz
#define Kpo_d   (0.0008127575979211963)
#define Kio_dTs (0.0008155323839119321)
#define Kpo_q   (0.0008706077043813599)
#define Kio_qTs (0.0008155323839119323)
#define A_d     (0.4991479447697602)
#define A_q     (0.5163317748186554)
#define B_d     (1.9263540585778451)
#define B_q     (1.8602624045436327)

typedef struct complex_t {
    double d;
    double q;
} complex_t;

// Forward declarations
static void _update_from_err(complex_t Idq_err, complex_t Idq, complex_t Vdq, double omega_e);

// States
static complex_t Idq_hat;
static complex_t Idq_err_acc_last;
static complex_t Idq_hat_last;
static complex_t Idq_err;
static complex_t Idq_err_acc;
static double Vq_sfb;
static double Vq_tot;
static double Iq_1;
static double Iq_hat;
static double Vd_sfb;
static double Vd_tot;
static double Id_1;
static double Id_hat;

void co_update(double *Idq0, double *Vdq0, double omega_e)
{
    complex_t Idq;
    complex_t Vdq;

    Idq.d = Idq0[0]; // direct
    Idq.q = Idq0[1]; // quadrature

    Vdq.d = Vdq0[0]; // direct
    Vdq.q = Vdq0[1]; // quadrature

    // Find current error
    Idq_err.d = Idq.d - Idq_hat.d;
    Idq_err.q = Idq.q - Idq_hat.q;

    _update_from_err(Idq_err, Idq, Vdq, omega_e);
}

static void _update_from_err(complex_t Idq_err, complex_t Idq, complex_t Vdq, double omega_e)
{
    // Saving Last States
    Idq_err_acc_last.d = Idq_err_acc.d;
    Idq_err_acc_last.q = Idq_err_acc.q;
    Idq_hat_last.d = Idq_hat.d;
    Idq_hat_last.q = Idq_hat.q;

    // Error and Accumulators
    Idq_err_acc.d = Idq_err_acc.d + Idq_err.d;
    Idq_err_acc.q = Idq_err_acc.q + Idq_err.q;

    // Q-axis
    Vq_sfb = Kpo_q * Idq_err.q + Kio_qTs * Idq_err_acc.q;
    Vq_tot = Vq_sfb - (Ld_HAT * Idq.d) * omega_e + Vdq.q;
    Iq_1 = B_q * Vq_tot;
    Iq_hat = Iq_1 + Idq_hat_last.q * A_q;

    // D-axis
    Vd_sfb = Kpo_d * Idq_err.d + Kio_dTs * Idq_err_acc.d;
    Vd_tot = Vd_sfb + Lq_HAT * Idq.q * omega_e + Vdq.d;
    Id_1 = B_d * Vd_tot;
    Id_hat = Id_1 + Idq_hat_last.d * A_d;

    Idq_hat.d = Id_hat;
    Idq_hat.q = Iq_hat;
}

void co_get_Idq_hat(double *Id_hat, double *Iq_hat)
{
    *Id_hat = Idq_hat.d;
    *Iq_hat = Idq_hat.q;
}

void co_get_Esal_hat(double *Esal_d_hat, double *Esal_q_hat)
{
    *Esal_d_hat = -Vd_sfb;
    *Esal_q_hat = -Vq_sfb;
}

#endif // APP_BETA_LABS
