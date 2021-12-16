#ifndef TRANSFORM_H
#define TRANSFORM_H

#define C_INVARIANT_POWER     (0.816496580927726032732) // sqrt(2/3)
#define C_INVARIANT_AMPLITUDE (0.666666666666666666667) // 2/3

typedef enum { TRANS_DQZ_C_INVARIANT_AMPLITUDE = 0, TRANS_DQZ_C_INVARIANT_POWER } transform_dqz_type_e;

void transform_dqz(transform_dqz_type_e C, double theta, double *abc, double *dqz);
void transform_dqz_inverse(transform_dqz_type_e C, double theta, double *abc, double *dqz);

void transform_clarke(transform_dqz_type_e C, double *abc, double *xyz);
void transform_park(double theta, double *xyz, double *dqz);

void transform_clarke_inverse(transform_dqz_type_e C, double *abc, double *xyz);
void transform_park_inverse(double theta, double *xyz, double *dqz);

#endif // TRANSFORM_H
