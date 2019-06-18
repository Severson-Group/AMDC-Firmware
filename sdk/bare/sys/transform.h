#ifndef TRANSFORM_H
#define TRANSFORM_H

#define TRANS_DQZ_C_INVARIANT_POWER			(0.816496580927726032732) // sqrt(2/3)
#define TRANS_DQZ_C_INVARIANT_AMPLITUDE		(0.666666666666666666667) // 2/3

void transform_dqz(double C, double theta, double *abc, double *dqz);
void transform_dqz_inverse(double C, double theta, double *abc, double *dqz);

void transform_clarke(double C, double *abc, double *xyz);
void transform_park(double theta, double *xyz, double *dqz);

#endif // TRANSFORM_H
