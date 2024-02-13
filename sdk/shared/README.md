# Shared Code Folder

This folder (`AMDC-Firmware/sdk/shared`) contains source code shared by both CPU applications in the v2, FreeRTOS-based firmware.

Shared code includes system-level source code for inter-core communication, interrupts, etc, as well as the [FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel) source code, which is available in `AMDC-Firmware/sdk/FreeRTOS-Kernel` as a [Git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

### Differentiating code between CPUs

If you need to include or run slightly different code on CPU 0 and CPU 1 in a shared source file, it can be done using the `#if` directive like so:

```c
#include xparameters.h          // Must be included to gain access to the XPAR_CPU_ID definition
#include foo.h
#include bar.h

// Code to be included on both CPUs

#if XPAR_CPU_ID == 0

// Code for CPU 0 only

#elif XPAR_CPU_ID == 1

// Code for CPU 1 only

#endif

// More code to be included on both CPUs

```
