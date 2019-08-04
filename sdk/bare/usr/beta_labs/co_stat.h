#ifndef CO_STAT_H
#define CO_STAT_H

void co_stat_reset(void);

void co_stat_update(
        double Ialpha, double Ibeta,
        double Valpha_star, double Vbeta_star,
        double omega_e);

void co_stat_get_Esal_hat(double *Esal_alpha_hat, double *Esal_beta_hat);

#endif // CO_STAT_H
