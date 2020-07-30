#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#include "drv/hardware_targets.h"

// This file is used to override system defines.

// Specify hardware revision (i.e. REV C, REV D, etc)
#define USER_CONFIG_HARDWARE_TARGET AMDC_REV_C

// Override the default scheduler elementary
// frequency by defining SYS_TICK_FREQ here.
// System uses 10kHz by default
//#define SYS_TICK_FREQ (20000) // Hz

// Enforce time quantum limits
// set to 1 for enabled, 0 for disabled
#define USER_CONFIG_ENABLE_TIME_QUANTUM_CHECKING (1)

// Enable task statistic collection by default
// NOTE: The user can still go and enable the stats themselves if this is set to 0!
// set to 1 for enabled, 0 for disabled
#define USER_CONFIG_ENABLE_TASK_STATISTICS_BY_DEFAULT (0)

// Enable the watchdog timer
// set to 1 for enabled, 0 for disabled
#define USER_CONFIG_ENABLE_WATCHDOG (0)

// Enable logging functionality
// set to 1 for enabled, 0 for disabled
#define USER_CONFIG_ENABLE_LOGGING (0)

// Enable injection functionality
// set to 1 for enabled, 0 for disabled
#define USER_CONFIG_ENABLE_INJECTION (0)

// Enable SensorCard platform motherboard support
// set to 1 for enabled, 0 for disabled
#define USER_CONFIG_ENABLE_MOTHERBOARD_SUPPORT (0)

#endif // USER_CONFIG_H
