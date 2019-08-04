
#ifdef APP_BETA_LABS

#include "co_stat.h"
#include "machine.h"

// Current observer gains assuming Ts = 1 / 20000
// Tuned to 400 Hz
#define Kpo_d   (0.030598576913049357)
#define Kio_dTs (0.030703041642474154)
#define Kpo_q   (0.03277650909907494)
#define Kio_qTs (0.030703041642474154)
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
static complex_t Idq_hat = {0.0, 0.0};
static complex_t Idq_err_acc_last = {0.0, 0.0};
static complex_t Idq_hat_last = {0.0, 0.0};
static complex_t Idq_err = {0.0, 0.0};
static complex_t Idq_err_acc = {0.0, 0.0};
static double Vq_sfb = 0.0;
static double Vq_tot = 0.0;
static double Iq_1 = 0.0;
static double Iq_hat = 0.0;
static double Vd_sfb = 0.0;
static double Vd_tot = 0.0;
static double Id_1 = 0.0;
static double Id_hat = 0.0;

void co_stat_reset(void)
{
    Idq_hat.d = 0.0;
    Idq_hat.q = 0.0;
    Idq_err_acc_last.d = 0.0;
    Idq_err_acc_last.q = 0.0;
    Idq_hat_last.d = 0.0;
    Idq_hat_last.q = 0.0;
    Idq_err.d = 0.0;
    Idq_err.q = 0.0;
    Idq_err_acc.d = 0.0;
    Idq_err_acc.q = 0.0;
    Vq_sfb = 0.0;
    Vq_tot = 0.0;
    Iq_1 = 0.0;
    Iq_hat = 0.0;
    Vd_sfb = 0.0;
    Vd_tot = 0.0;
    Id_1 = 0.0;
    Id_hat = 0.0;
}

void co_stat_update(
        double Ialpha, double Ibeta,
        double Valpha_star, double Vbeta_star,
        double omega_e)
{
    complex_t Idq;
    complex_t Vdq;

    Idq.d = Ialpha; // direct
    Idq.q = Ibeta; // quadrature

    Vdq.d = Valpha_star; // direct
    Vdq.q = Vbeta_star; // quadrature

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
    Vq_tot = Vq_sfb + Vdq.q;
    Iq_1 = B_q *  Vq_tot;
    Iq_hat = Iq_1 + Idq_hat_last.q * A_q;

    // D-axis
    Vd_sfb = Kpo_d * Idq_err.d + Kio_dTs * Idq_err_acc.d;
    Vd_tot = Vd_sfb + Vdq.d;
    Id_1 = B_d *  Vd_tot;
    Id_hat = Id_1 + Idq_hat_last.d * A_d;

    Idq_hat.d = Id_hat;
    Idq_hat.q = Iq_hat;
}

void co_stat_get_Esal_hat(double *Esal_alpha_hat, double *Esal_beta_hat)
{
    *Esal_alpha_hat = -Vd_sfb;
    *Esal_beta_hat  = -Vq_sfb;
}


#endif // APP_BETA_LABS
