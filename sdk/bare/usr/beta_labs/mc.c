#ifdef APP_BETA_LABS

#include "mc.h"

// Tuning for motion controller with Ts = 0.00025
// Bandwidth is tuned to 20 Hz
#define Ba	(0.7171488891319134)
#define Ksa	(7.388907366318899)
#define Kisa	(6.860110384268592)

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
