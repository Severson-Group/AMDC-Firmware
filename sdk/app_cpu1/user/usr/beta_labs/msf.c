#ifdef APP_BETA_LABS

#include "usr/beta_labs/msf.h"

// Motion state filter gains with Ts = 0.00025
// Bandwidth tuned to 200.0 Hz
#define Ts     (0.00025)
#define Ksf2Ts (1.226383390687582)
#define Ksf1   (841.3256566809094)

// Output States
static double msf_omega_m_dot;
static double msf_omega_m;
static double msf_omega_m_ave;
static double msf_delta_theta;

// Internal States
static double msf_omega_m_last;
static double msf_omega_m_dot_next;
static double msf_omega_m_next;
static double msf_omega_err;
static double msf_omega_dot_err;
static double msf_delta_omega;

double msf_get_omega_m(void)
{
    return msf_omega_m;
}

double msf_get_omega_m_dot(void)
{
    return msf_omega_m_dot;
}

void msf_init(void)
{
    msf_omega_m = 0.0;
    msf_omega_m_dot_next = 0.0;
    msf_omega_m_next = 0.0;
}

double msf_update(double omega_m_star)
{
    msf_omega_m_last = msf_omega_m;
    msf_omega_m_dot = msf_omega_m_dot_next;
    msf_omega_m = msf_omega_m_next;

    msf_omega_err = (omega_m_star - msf_omega_m);
    msf_omega_dot_err = Ksf1 * msf_omega_err - msf_omega_m_dot;
    msf_omega_m_dot_next = msf_omega_m_dot + msf_omega_dot_err * Ksf2Ts;
    msf_delta_omega = Ts * msf_omega_m_dot;
    msf_omega_m_next = msf_delta_omega + msf_omega_m;
    msf_omega_m_ave = 0.5 * (msf_omega_m + msf_omega_m_last);
    msf_delta_theta = msf_omega_m_ave * Ts;

    return msf_delta_theta;
}

#endif // APP_BETA_LABS
