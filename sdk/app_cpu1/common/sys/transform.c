#include "transform.h"
#include "defines.h"
#include <math.h>

void transform_clarke(transform_dqz_type_e C, double *abc, double *xyz)
{
    if (C == TRANS_DQZ_C_INVARIANT_AMPLITUDE) {
        xyz[0] = C_INVARIANT_AMPLITUDE * (1.0 * abc[0] - 0.5 * abc[1] - 0.5 * abc[2]);
        xyz[1] = C_INVARIANT_AMPLITUDE * (0.0 * abc[0] + SQRT3 / 2.0 * abc[1] - SQRT3 / 2.0 * abc[2]);
        xyz[2] = (1.0 / 3.0 * abc[0] + 1.0 / 3.0 * abc[1] + 1.0 / 3.0 * abc[2]);
    }
    if (C == TRANS_DQZ_C_INVARIANT_POWER) {
        xyz[0] = C_INVARIANT_POWER * (1.0 * abc[0] - 0.5 * abc[1] - 0.5 * abc[2]);
        xyz[1] = C_INVARIANT_POWER * (0.0 * abc[0] + SQRT3 / 2.0 * abc[1] - SQRT3 / 2.0 * abc[2]);
        xyz[2] = (1.0 / SQRT3 * abc[0] + 1.0 / SQRT3 * abc[1] + 1.0 / SQRT3 * abc[2]);
    }
}

void transform_park(double theta, double *xyz, double *dqz)
{
    dqz[0] = +cos(theta) * xyz[0] + sin(theta) * xyz[1] + 0.0 * xyz[2];
    dqz[1] = -sin(theta) * xyz[0] + cos(theta) * xyz[1] + 0.0 * xyz[2];
    dqz[2] = 0.0 * xyz[0] + 0.0 * xyz[1] + 1.0 * xyz[2];
}

void transform_dqz(transform_dqz_type_e C, double theta, double *abc, double *dqz)
{
    double xyz[3];
    transform_clarke(C, abc, xyz);
    transform_park(theta, xyz, dqz);
}

void transform_clarke_inverse(transform_dqz_type_e C, double *abc, double *xyz)
{
    if (C == TRANS_DQZ_C_INVARIANT_AMPLITUDE) {
        abc[0] = 1.0 * xyz[0] + 0.0 * xyz[1] + 1.0 * xyz[2];
        abc[1] = -1.0 / 2.0 * xyz[0] + SQRT3 / 2.0 * xyz[1] + 1.0 * xyz[2];
        abc[2] = -1.0 / 2.0 * xyz[0] - SQRT3 / 2.0 * xyz[1] + 1.0 * xyz[2];
    }
    if (C == TRANS_DQZ_C_INVARIANT_POWER) {
        abc[0] = C_INVARIANT_POWER * (1.0 * xyz[0] + 0.0 * xyz[1] + 1.0 / SQRT2 * xyz[2]);
        abc[1] = C_INVARIANT_POWER * (-1.0 / 2.0 * xyz[0] + SQRT3 / 2.0 * xyz[1] + 1.0 / SQRT2 * xyz[2]);
        abc[2] = C_INVARIANT_POWER * (-1.0 / 2.0 * xyz[0] - SQRT3 / 2.0 * xyz[1] + 1.0 / SQRT2 * xyz[2]);
    }
}

void transform_park_inverse(double theta, double *xyz, double *dqz)
{
    // Rotate backwards
    transform_park(-theta, dqz, xyz);
}

void transform_dqz_inverse(transform_dqz_type_e C, double theta, double *abc, double *dqz)
{
    double xyz[3];
    transform_park_inverse(theta, xyz, dqz);
    transform_clarke_inverse(C, abc, xyz);
}
