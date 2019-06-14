#ifndef TRANSFORM_H
#define TRANSFORM_H

void transform_dqz(double theta, double *abc, double *dqz);
void transform_dqz_inverse(double theta, double *abc, double *dqz);

void transform_clarke(double *abc, double *xyz);
void transform_park(double theta, double *xyz, double *dqz);

#endif // TRANSFORM_H
