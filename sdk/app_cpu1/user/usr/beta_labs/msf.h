#ifndef MSF_H
#define MSF_H

void msf_init(void);

double msf_update(double omega_m_star);

double msf_get_omega_m(void);
double msf_get_omega_m_dot(void);

#endif // MSF_H
