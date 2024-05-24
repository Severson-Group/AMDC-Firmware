#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#include "drv/hardware_targets.h"

// This file is used to override system defines and conditionally enable
// various system-level firmware features.

// Specify hardware revision (i.e. REV E, REV F, etc)
#define USER_CONFIG_HARDWARE_TARGET (AMDC_REV_F)

// Override the default scheduler elementary frequency by defining SYS_TICK_FREQ here.
// System uses 10kHz by default
//#define SYS_TICK_FREQ (20000) // Hz

// Specify the source of the scheduler ISR
// Mode 0: legacy mode - scheduler is triggered based on the PWM carrier events and ratio
//         of carrier frequency to desired control frequency
// Mode 1: new mode - scheduler is triggered when all the enabled sensors are done
//         acquiring their data
#define USER_CONFIG_ISR_SOURCE (0)

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

// Override default number of available logging variables
// when defined, this takes precedence over system default of 32
//#define USER_CONFIG_LOGGING_MAX_NUM_VARIABLES (150)

// Override default logging sample depth per variable
// when defined, this takes precedence over system default of 100k
//#define USER_CONFIG_LOGGING_SAMPLE_DEPTH_PER_VARIABLE (50000)

// Enable injection functionality
// set to 1 for enabled, 0 for disabled
#define USER_CONFIG_ENABLE_INJECTION (0)

// Enable support for AMDS (Advanced Motor Drive Sensing) platform
// set to 1 for enabled, 0 for disabled
#define USER_CONFIG_ENABLE_AMDS_SUPPORT (0)

#endif // USER_CONFIG_H
