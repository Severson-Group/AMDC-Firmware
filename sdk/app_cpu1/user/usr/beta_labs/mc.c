#ifdef APP_BETA_LABS

#include "usr/beta_labs/mc.h"

// Tuning for motion controller with Ts = 0.00025
// Bandwidth is tuned to 5 Hz
#define Ba   (0.18121428301337322)
#define Ksa  (0.4668134916780646)
#define Kisa (0.10834187149852403)

// Static variables for controller
static double delta_theta_m_error_acc;
static double delta_theta_m_error_acc_acc;

void mc_init(void)
{
    delta_theta_m_error_acc = 0.0;
    delta_theta_m_error_acc_acc = 0.0;
}

double mc_update(double delta_theta_m_star, double delta_theta_m, double Ts)
{
    // Run through motion controller block diagram
    double delta_theta_m_error = delta_theta_m_star - delta_theta_m;
    delta_theta_m_error_acc += delta_theta_m_error;
    delta_theta_m_error_acc_acc += delta_theta_m_error_acc;

    // Calculate commanded torque
    double term1 = (Ba / Ts * delta_theta_m_error);
    double term2 = (Ksa * delta_theta_m_error_acc);
    double term3 = (Kisa * Ts * delta_theta_m_error_acc_acc);
    double Tem_SFB_star = term1 + term2 + term3;

    return Tem_SFB_star;
}

#endif // APP_BETA_LABS
