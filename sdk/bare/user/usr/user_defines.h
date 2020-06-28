#ifndef USER_CONFIG_H
#define USER_CONFIG_H

// This file is used to override system defines.

// Specify hardware revision (i.e. REV C, REV D, etc)
// Alphabet to number: A = 1, B = 2, C = 3, etc
#define USER_CONFIG_HARDWARE_TARGET (4)

// Override the default scheduler elementary
// frequency by defining SYS_TICK_FREQ here.
// System uses 10kHz by default
//#define SYS_TICK_FREQ (20000) // Hz

// Enforce time quantum limits
// set to 1 for enabled, 0 for disabled
#define USER_CONFIG_ENABLE_TIME_QUANTUM_CHECKING (1)

// Enable the watchdog timer
// set to 1 for enabled, 0 for disabled
#define USER_CONFIG_ENABLE_WATCHDOG (0)

// Enable logging functionality
// set to 1 for enabled, 0 for disabled
#define USER_CONFIG_ENABLE_LOGGING (1)

// Enable injection functionality
// set to 1 for enabled, 0 for disabled
#define USER_CONFIG_ENABLE_INJECTION (0)

#endif // USER_CONFIG_H
