#include "sys/serial.h"
#include "drv/uart.h"
#include "sys/scheduler.h"
#include "sys/util.h"
#include <string.h>

#define OUTPUT_BUFFER_LENGTH (32 * 1024)

static char output_buffer[OUTPUT_BUFFER_LENGTH] = { 0 };
static int output_idx = 0;

// Used to mark position in `output_buffer` of char which needs to output
//
// -1 means not currently outputting anything
// >= 0 means trying to print at that idx
static int print_idx = -1;

// Amount of chars we need to print at the moment
static int print_amount = 0;

static task_control_block_t tcb;

void serial_init(void)
{
    printf("DB:\tInitializing serial task...\n");
    scheduler_tcb_init(&tcb, serial_callback, NULL, "serial", SERIAL_INTERVAL_USEC);
    scheduler_tcb_register(&tcb);
}

void serial_callback(void *arg)
{
    if (print_amount > 0) {
        // Have work to do!

        // Determine amount of chars to print before buffer wrap
        int try_to_send = MIN(print_amount, OUTPUT_BUFFER_LENGTH - print_idx);

        // Try to send that amount of chars to the UART
        int bytes_sent = uart_send(&output_buffer[print_idx], try_to_send);
        print_amount -= bytes_sent;

        // UART TX FIFO might have been full, so didn't print all we tried
        print_idx += bytes_sent;
        if (print_idx >= OUTPUT_BUFFER_LENGTH) {
            print_idx = 0;
        }

        // Check if done outputting data
        if (print_amount == 0) {
            print_idx = -1;
        }
    }
}

static void _append_to_output_buffer(char c)
{
    output_buffer[output_idx] = c;

    output_idx++;
    if (output_idx >= OUTPUT_BUFFER_LENGTH) {
        output_idx = 0;
    }
}

void serial_write(char *msg, int len)
{
    // Mark index to start outputting at in the callback
    if (print_idx == -1) {
        print_idx = output_idx;
    }

    print_amount += len;

    // Copy contents into circular output buffer
    for (int i = 0; i < len; i++) {
        _append_to_output_buffer(msg[i]);
    }
}
