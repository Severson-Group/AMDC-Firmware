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

#endif // DEFINES_H
