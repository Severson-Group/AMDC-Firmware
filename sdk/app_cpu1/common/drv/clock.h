#include "xparameters.h"

// CPU 0 and 1 clock frequency: 666.666 MHz
#define CLOCK_CPU0_CLK_FREQ_HZ XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ
#define CLOCK_CPU1_CLK_FREQ_HZ XPAR_PS7_CORTEXA9_1_CPU_CLK_FREQ_HZ

#define CLOCK_CPU0_CLK_FREQ_MHZ (CLOCK_CPU0_CLK_FREQ_HZ / (double) 1e6)
#define CLOCK_CPU1_CLK_FREQ_MHZ (CLOCK_CPU1_CLK_FREQ_HZ / (double) 1e6)

// FPGA clock frequency: 200 MHz
#define CLOCK_FPGA_CLK_FREQ_HZ  (200000000)
#define CLOCK_FPGA_CLK_FREQ_MHZ (200)
