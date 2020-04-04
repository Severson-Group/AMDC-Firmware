#include "transform.h"
#include "defines.h"
#include <math.h>

void transform_clarke(double C, double *abc, double *xyz)
{
    xyz[0] = C * (1		*abc[0]			-0.5	*abc[1]			-0.5	*abc[2]);
    xyz[1] = C * (0		*abc[0]		+SQRT3/2	*abc[1]		-SQRT3/2	*abc[2]);
    xyz[2] =     (1/3	*abc[0]		+1/3	    *abc[1]		+1/3	    *abc[2]);
}

void transform_park(double theta, double *xyz, double *dqz)
{
    dqz[0] = +cos(theta)	*xyz[0]		+sin(theta)	*xyz[1]		+0	*xyz[2];
    dqz[1] = -sin(theta)	*xyz[0]		+cos(theta)	*xyz[1]		+0	*xyz[2];
    dqz[2] = +0				*xyz[0]		+0			*xyz[1]		+1	*xyz[2];
}

void transform_dqz(double C, double theta, double *abc, double *dqz)
{
    double xyz[3];
    transform_clarke(C, abc, xyz);
    transform_park(theta, xyz, dqz);
}

void transform_dqz_inverse(double C, double theta, double *abc, double *dqz)
{
    abc[0] = C * (cos(theta)		*dqz[0] 	-sin(theta)			*dqz[1]) 	+	dqz[2]);
    abc[1] = C * (cos(theta - PI23)	*dqz[0] 	-sin(theta - PI23)	*dqz[1]) 	+	dqz[2]);
    abc[2] = C * (cos(theta + PI23)	*dqz[0] 	-sin(theta + PI23)	*dqz[1]) 	+	dqz[2]);
}
