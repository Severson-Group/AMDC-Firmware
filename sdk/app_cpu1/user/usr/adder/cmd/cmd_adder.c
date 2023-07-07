#ifdef APP_ADDER

#include "usr/adder/cmd/cmd_adder.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "sys/util.h"
#include <stdlib.h>
#include <string.h>
#include "xil_io.h"
#include "drv/cpu_timer.h"


// Stores command entry for command system module
static command_entry_t cmd_entry;

// Defines help content displayed for this command
// when user types "help" at command prompt
static command_help_t cmd_help[] = {
    { "test [cpu | fpga] <N> <in1> <in2>", "test adder"},

};

void cmd_adder_register(void)
{
	// Populate the command entry block
	    //
	    // Here is where you define the base command string: "ctrl"
	    // and what function is called to handle command
    commands_cmd_init(&cmd_entry, "adder", "Adder commands",
                        cmd_help, ARRAY_SIZE(cmd_help), cmd_adder);
    // Register the command with the system
    commands_cmd_register(&cmd_entry);
}


int cmd_adder(int argc, char **argv) {
    if (argc == 6 && STREQ("test", argv[1])) {
        int N = atoi(argv[3]);
        if (N < 1) {
            return CMD_INVALID_ARGUMENTS;
        }

        volatile uint32_t in1 = (uint32_t) atoi(argv[4]);
        volatile uint32_t in2 = (uint32_t) atoi(argv[5]);
        volatile uint32_t out;

        uint32_t now_start, now_stop;
        uint32_t total_time = 0; // in units of CPU cycles
        if (STREQ("cpu", argv[2])) {
            for (int i = 0; i < N; i++) {
                CPU_TIMER_NOW(now_start);

                // Compute result using CPU
                out = 8*in1 + in2/4 - 10203;

                CPU_TIMER_NOW(now_stop);
                total_time += (now_stop - now_start);
            }
        } else if (STREQ("fpga", argv[2])) {
            // Pointer to our custom IP core
            volatile uint32_t *base_addr = (volatile uint32_t *) 0x43DB0000;

            for (int i = 0; i < N; i++) {
            	CPU_TIMER_NOW(now_start);

                // Compute result using FPGA
                base_addr[0] = in1;
                base_addr[1] = in2;
                out = base_addr[2];


                CPU_TIMER_NOW(now_stop);
                total_time += (now_stop - now_start);
            }
        } else {
            return CMD_INVALID_ARGUMENTS;
        }

        double total_time_usec = cpu_timer_ticks_to_usec(total_time);
        cmd_resp_printf("out: %d\r\n", out);
        cmd_resp_printf("average time [us] per operation: %f\r\n",
                            total_time_usec / ((double) N));

        return CMD_SUCCESS;
    }

    return CMD_INVALID_ARGUMENTS;
}

#endif // APP_ADDER
