#ifndef USER_DEFINES_H
#define USER_DEFINES_H

// This file is used to override system defines.

// Specify hardware revision (i.e. REV C, REV D, etc)
// Alphabet to number: A = 1, B = 2, C = 3, etc
#define HARDWARE_REVISION (3)

// Override the default scheduler elementary
// frequency by defining SYS_TICK_FREQ here:
//#define SYS_TICK_FREQ (20000) // Hz

// Enable the watchdog timer
#define ENABLE_WATCHDOG (0)

// Enable logging functionality
#define ENABLE_LOGGING (1)

// Enable injection functionality
#define ENABLE_INJECTION (0)

#endif // USER_DEFINES_H
