#ifndef CO_H
#define CO_H

void co_update(double *Idq0, double *Vdq0, double omega_e);

void co_get_Idq_hat(double *Id_hat, double *Iq_hat);
void co_get_Esal_hat(double *Esal_d_hat, double *Esal_q_hat);

#endif // CO_H
