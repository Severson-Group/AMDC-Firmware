#ifndef TRANSFORM_H
#define TRANSFORM_H

#define TRANS_DQZ_C_INVARIANT_POWER      (0.816496580927726032732) // sqrt(2/3)
#define TRANS_DQZ_C_INVARIANT_AMPLITUDE  (0.666666666666666666667) // 2/3

// Clarke transform (3ph to 2ph)
void transform_clarke(double C, double *in, double *out);
void transform_clarke_inverse(double C, double *in, double *out);

// Park transform (rotation)
void transform_park(double theta, double *in, double *out);
void transform_park_inverse(double theta, double *in, double *out);

// Full DQZ transform
void transform_dqz(double C, double theta, double *abc_in, double *dqz_out);
void transform_dqz_inverse(double C, double theta, double *dqz_in, double *abc_out);

#endif // TRANSFORM_H
