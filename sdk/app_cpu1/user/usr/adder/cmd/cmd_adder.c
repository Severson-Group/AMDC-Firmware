#ifdef APP_ADDER

#include "usr/adder/cmd/cmd_adder.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "sys/util.h"
// #include "drv/pwm.h"
#include <stdlib.h>
#include <string.h>
#include "drv/cpu_timer.h"
#include "xil_io.h"

// Stores command entry for command system module
static command_entry_t cmd_entry;

// Defines help content displayed for this command
// when user types "help" at command prompt
static command_help_t cmd_help[] = {
	{ "test", "return the average execution time" },
};

void cmd_adder_register(void)
{
    commands_cmd_init(&cmd_entry, "adder", "Controller commands",
                        cmd_help, ARRAY_SIZE(cmd_help), cmd_adder);
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
            	asm volatile("MRC p15, 0, %0, c9, c13, 0\t\n" : "=r"(now_start));

                // Compute result using CPU
                out = 8*in1 + in2/4 - 10203;

                asm volatile("MRC p15, 0, %0, c9, c13, 0\t\n" : "=r"(now_stop));
                total_time += (now_stop - now_start);
            }
        } else if (STREQ("fpga", argv[2])) {
            // Pointer to our custom IP core
            volatile uint32_t *base_addr = (volatile uint32_t *) 0x43DB0000;

            for (int i = 0; i < N; i++) {
            	asm volatile("MRC p15, 0, %0, c9, c13, 0\t\n" : "=r"(now_start));

                // Compute result using FPGA
                base_addr[0] = in1;
                dmb();
                base_addr[1] = in2;
                dmb();
                out = base_addr[2];
                dmb();

                asm volatile("MRC p15, 0, %0, c9, c13, 0\t\n" : "=r"(now_stop));
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
