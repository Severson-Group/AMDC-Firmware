#ifndef MACHINE_H
#define MACHINE_H

#include "../../sys/defines.h"


// TG motor from Beta
// ------------------
#define Ld_HAT			(11.5 / 1000000.0) // H
#define Lq_HAT			(20.5 / 1000000.0) // H
#define Rs_HAT			(236.0 / 1000.0) // Ohms
//#define Lambda_PM_HAT	(50) // ? TODO: what is this
#define Kt_HAT			(0.196)	// Nm / Arms
#define Km_HAT			(0.330)	// Nm / sqrt(W)
#define Ke_HAT			(0.160)	// Vpkll / (rads/s)
#define Jp				(3.97e-3) // kg-m^2

#define POLE_PAIRS		(16.0)

// NOTE: use this to artificially limit current
#define I_rated_rms			(2.0)	// Arms
//#define I_rated_rms			(24.2)	// Arms

#define I_rated_pk			(I_rated_rms * SQRT2)	// Apk
#define I_rated_dq			(I_rated_pk / SQRT23)	// A_dq


#endif // MACHINE_H
