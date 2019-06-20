#ifndef MACHINE_H
#define MACHINE_H

#include "../../sys/defines.h"

// Parameters for TG motor from Beta
//
// Started as datasheet values, but since
// have been updated to more accurate estimates

#define Ld_HAT			(18.709  / 1000000.0)	// H
#define Lq_HAT			(19.667  / 1000000.0)	// H
#define Rs_HAT			(260.0   / 1000.0)		// Ohms
#define Lambda_pm_HAT	(7407.0  / 1000000.0)	// Wb

#define Kt_HAT			(0.196)				// Nm / Arms
#define Km_HAT			(0.330)				// Nm / sqrt(W)
#define Ke_HAT			(0.160)				// Vpkll / (rads/s)
#define Jp				(3.97e-3)			// kg-m^2

#define POLE_PAIRS		(16.0)

// NOTE: use this to artificially limit current
#define I_rated_rms			(2.0)	// Arms
//#define I_rated_rms			(24.2)	// Arms

#define I_rated_pk			(I_rated_rms * SQRT2)	// Apk
#define I_rated_dq			(I_rated_pk / SQRT23)	// A_dq


#endif // MACHINE_H
