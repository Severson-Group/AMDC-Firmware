#ifndef DEFINES_H
#define DEFINES_H

#include <stdio.h>

#define UNUSED(x) (void) (x)

#define HANG                                                                                                           \
    printf("HANG!!!\n");                                                                                               \
    while (1)

#define SUCCESS (0)
#define FAILURE (1)

#define PI   (3.141592653589793238463) // pi
#define PI23 (2.094395102393195492308) // 2*pi/3
#define PI2  (6.283185307179586476925) // 2*pi

#define SQRT23 (0.816496580927726032732) // sqrt(2/3)
#define SQRT2  (1.414213562373095048802) // sqrt(2)
#define SQRT3  (1.732050807568877293528) // sqrt(3)

#define DEG_TO_RAD(deg)         (deg * PI / 180.0)
#define RAD_TO_DEG(rad)         (rad * 180.0 / PI)
#define RPM_TO_RAD_PER_SEC(rpm) (rpm * PI2 / 60.0)
#define RAD_PER_SEC_TO_RPM(rps) (rps * 60.0 / PI2)

#endif // DEFINES_H
