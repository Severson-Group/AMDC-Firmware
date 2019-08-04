#include "transform.h"
#include "defines.h"
#include <math.h>

// -------------------------
// Clarke transform (3ph to 2ph)
// -------------------------

void transform_clarke(double C, double *in, double *out)
{
    out[0] = C * (1        *in[0]  -0.5      *in[1]  -0.5      *in[2]);
    out[1] = C * (0        *in[0]  +SQRT3/2  *in[1]  -SQRT3/2  *in[2]);
    out[2] = C * (1/SQRT2  *in[0]  +1/SQRT2  *in[1]  +1/SQRT2  *in[2]);
}

void transform_clarke_inverse(double C, double *in, double *out)
{

}

// -------------------------
// Park transform (rotation)
// -------------------------

void transform_park(double theta, double *in, double *out)
{
    out[0] = +cos(theta)  *in[0]  +sin(theta)  *in[1]  +0  *in[2];
    out[1] = -sin(theta)  *in[0]  +cos(theta)  *in[1]  +0  *in[2];
    out[2] = +0           *in[0]  +0           *in[1]  +1  *in[2];
}

void transform_park_inverse(double theta, double *in, double *out)
{
    // Rotate backwards
    transform_park(-theta, in, out);
}

// ------------------
// Full DQZ transform
// ------------------

void transform_dqz(double C, double theta, double *abc_in, double *dqz_out)
{
    double xyz[3];
    transform_clarke(C, abc_in, xyz);
    transform_park(theta, xyz, dqz_out);
}

void transform_dqz_inverse(double C, double theta, double *dqz_in, double *abc_out)
{
    abc_out[0] = C * (cos(theta)        *dqz_in[0] -sin(theta)        *dqz_in[1] +(SQRT2/2) *dqz_in[2]);
    abc_out[1] = C * (cos(theta - PI23) *dqz_in[0] -sin(theta - PI23) *dqz_in[1] +(SQRT2/2) *dqz_in[2]);
    abc_out[2] = C * (cos(theta + PI23) *dqz_in[0] -sin(theta + PI23) *dqz_in[1] +(SQRT2/2) *dqz_in[2]);
}
