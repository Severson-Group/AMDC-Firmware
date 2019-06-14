#include "transform.h"
#include "defines.h"
#include <math.h>

void transform_clarke(double *abc, double *xyz)
{
	xyz[0] = SQRT23 * (1		*abc[0]			-0.5	*abc[1]			-0.5	*abc[2]);
	xyz[1] = SQRT23 * (0		*abc[0]		+SQRT3/2	*abc[1]		-SQRT3/2	*abc[2]);
	xyz[2] = SQRT23 * (1/SQRT2	*abc[0]		+1/SQRT2	*abc[1]		+1/SQRT2	*abc[2]);
}

void transform_park(double theta, double *xyz, double *dqz)
{
	dqz[0] = +cos(theta)	*xyz[0]		+sin(theta)	*xyz[1]		+0	*xyz[2];
	dqz[1] = -sin(theta)	*xyz[0]		+cos(theta)	*xyz[1]		+0	*xyz[2];
	dqz[2] = +0				*xyz[0]		+0			*xyz[1]		+1	*xyz[2];
}

void transform_dqz(double theta, double *abc, double *dqz)
{
	double xyz[3];
	transform_clarke(abc, xyz);
	transform_park(theta, xyz, dqz);
}

void transform_dqz_inverse(double theta, double *abc, double *dqz)
{
	abc[0] = SQRT23 * (cos(theta)			*dqz[0] 	-sin(theta)			*dqz[1] 	+(SQRT2/2)	*dqz[2]);
	abc[1] = SQRT23 * (cos(theta - PI23)	*dqz[0] 	-sin(theta - PI23)	*dqz[1] 	+(SQRT2/2)	*dqz[2]);
	abc[2] = SQRT23 * (cos(theta + PI23)	*dqz[0] 	-sin(theta + PI23)	*dqz[1] 	+(SQRT2/2)	*dqz[2]);
}
