#ifndef MCFF_H
#define MCFF_H

void mcff_update(double omega_m_star, double omega_dot_m_star);

double mcff_get_Te_cff_total(void);
double mcff_get_Te_cff_bp(void);
double mcff_get_Te_cff_jp(void);

#endif // MCFF_H
