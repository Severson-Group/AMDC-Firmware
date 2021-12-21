#ifndef BEMFO_H
#define BEMFO_H

void bemfo_init(void);
void bemfo_update(double Esal_alpha, double Esal_beta, double Te_hat);

void bemfo_start(void);

double bemfo_get_omega_m_hat(void);
double bemfo_get_omega_e_hat(void);
double bemfo_get_theta_e_hat(void);

#endif // BEMFO_H
