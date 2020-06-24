#ifndef USER_DEFINES_H
#define USER_DEFINES_H

//
// This file is used to override system defines.
//

// Specify hardware revision (i.e. REV C, REV D, etc)
// Alphabet to number: A = 1, B = 2, C = 3, etc
#define HARDWARE_REVISION (3)

// Override the default scheduler elementary
// frequency by defining SYS_TICK_FREQ here:
//#define SYS_TICK_FREQ (20000) // Hz

// Enable the watchdog timer
// by defining ENABLE_WATCHDOG here:
//#define ENABLE_WATCHDOG

// Disable logging functionality
// by defining DISABLE_LOGGING here:
#define DISABLE_LOGGING

// Disable injection functionality
// by defining DISABLE_INJECTION here:
#define DISABLE_INJECTION

#endif // USER_DEFINES_H
