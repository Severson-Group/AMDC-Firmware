#ifndef CO_SYNC_H
#define CO_SYNC_H

void co_sync_update(
        double Id, double Iq,
        double Vd_star, double Vq_star,
        double omega_e);

void co_sync_get_Idq_hat(double *Id_hat, double *Iq_hat);
void co_sync_get_Esal_hat(double *Esal_d_hat, double *Esal_q_hat);

#endif // CO_SYNC_H
