/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
/* other includes */
#include "sys/commands.h"
#include "ip/socket_manager.h"
// #include "drv/encoder.h"
#include "drv/uart.h"
#include "sys/cmd/cmd_help.h"
#include "sys/debug.h"
#include "sys/defines.h"
// #include "sys/log.h"
#include "sys/serial.h"
#include "sys/util.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define RECV_BUFFER_LENGTH (4 * 1024)

typedef enum cmd_parsing_state_e {
    BEGIN = 0,
    LOOKING_FOR_SPACE,
    LOOKING_FOR_CHAR,
} cmd_parsing_state_e;

#define CMD_MAX_ARGC       (16) // # of args accepted
#define CMD_MAX_ARG_LENGTH (32) // max chars of any arg
typedef struct pending_cmd_t {
    int argc;
    char *argv[CMD_MAX_ARGC];

    // Used by parser to keep track of how
    // long the argument is. If too long,
    // throws an error!
    int curr_arg_length;

    // Error flag set by parser task
    int err;

    // Set to 1 to indicate ready to execute,
    // 0 means not valid
    int ready;
} pending_cmd_t;

// Note, this must be >= 2.
//
// We always need a buffer to be filling while we
// execute the previous command
//
#define MAX_PENDING_CMDS (8)

typedef struct {
    cmd_parsing_state_e state;

    // Store parsed cmds
    pending_cmd_t pending_cmds[MAX_PENDING_CMDS];
    int pending_cmd_write_idx;
    int pending_cmd_read_idx;

    // Store rx chars
    char recv_buffer[RECV_BUFFER_LENGTH];
    int recv_buffer_idx;
} sm_parse_ascii_cmd_ctx_t;

static sm_parse_ascii_cmd_ctx_t ctx_uart;
static sm_parse_ascii_cmd_ctx_t ctx_eth;
static Socket_t rawSocket;

static int _command_handler(int argc, char **argv);

static void commands_uart(void *arg);
static void commands_eth(void *arg);
static void parse_commands(void *arg);

// Head of linked list of commands
static command_entry_t *cmds = NULL;

static TaskHandle_t tcb_uart;
static uint8_t uartTaskExists = 0; // extra data to ensure tasks don't get duplicated or double free'd
static TaskHandle_t tcb_eth;
static uint8_t ethTaskExists = 0; // extra data to ensure tasks don't get duplicated or double free'd

// The command response data should either go out UART or ETH,
// depending on the command source. Therefore, we'll define
// global general command response functions which users will
// use for their cmd response. This "commands" module will
// update these functions to target the correct stream.

typedef enum cmd_src_e {
    CMD_SRC_UART = 0,
    CMD_SRC_ETH,
} cmd_src_e;

static cmd_src_e current_cmd_source = CMD_SRC_UART;

void cmd_resp_write(char *msg, int len)
{
    if (current_cmd_source == CMD_SRC_UART) {
        serial_write(msg, len);
    } else {
    	if (len > 0) {
    		int socket_id = get_socket_id(rawSocket);
    		if (socket_id == -1) {
    			xil_printf("BAD\n");
    		} else {
    			socket_manager_set_time(socket_id, 5000);
    		}
    	}
        for (int i = 0; i < len; i++) {
        	FreeRTOS_send(rawSocket, &msg[i], 1, 0);
        }
    }
}

void cmd_resp_print(char *msg)
{
    if (current_cmd_source == CMD_SRC_UART) {
        debug_print(msg);
    } else {
        cmd_resp_write(msg, strlen(msg));
    }
}

#define PRINTF_BUFFER_LENGTH (1024)
static char buffer[PRINTF_BUFFER_LENGTH] = { 0 };
void cmd_resp_printf(const char *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    vsnprintf(buffer, PRINTF_BUFFER_LENGTH, format, vargs);
    cmd_resp_print(buffer);
    va_end(vargs);
}

void commands_init(void)
{
    printf("CMD:\tInitializing command tasks...\n");
    // Command parse & exec task (UART)
    if (uartTaskExists == 0) {
        xTaskCreate(commands_uart, (const char *) "command_uart", 1024,
				    NULL, tskIDLE_PRIORITY, &tcb_uart);
        uartTaskExists = 1;
	}

    // Command parse task (ETH)
    if (ethTaskExists == 0) {
        xTaskCreate(commands_eth, (const char *) "command_eth", 1024,
                    NULL, tskIDLE_PRIORITY, &tcb_eth);
        ethTaskExists = 1;
    }

    cmd_help_register();
}

// Set if we should echo back characters to sender:
//  - 0: when EXECUTING the pending command
//  - 1: when PROCESSING the incoming chars
//
// Both have pros and cons... For slow comms, we want
// to echo AS the chars come in since the user is likely
// using a UART terminal and wants to see a response to
// their inputs.
//
// For fast comms (i.e. Ethernet), we should only echo
// when the command is executed since the host could send
// a burst of multiple command strings in short time,
// before we have time to actually run the command.
//
// We will choose when PROCESSING the incoming chars and
// limit the command through-put from the host to:
// <=  1 command string per 100 usec time slice
// This will ensure we process the pending command before
// the next set of chars appears.
#define ECHO_BACK_WHEN_PROCESSING_INCOMING_CHARS (1)

static void _create_pending_cmds(sm_parse_ascii_cmd_ctx_t *ctx, char *buffer, int length)
{
    // Get current pending cmd slot
    pending_cmd_t *p = &ctx->pending_cmds[ctx->pending_cmd_write_idx];

    for (int i = 0; i < length; i++) {
        char c = buffer[i];

        if (ctx->state != BEGIN && (c == '\n' || c == '\r')) {
            // End of a command!

            // Set error flag if this arg was too long
            if (p->curr_arg_length > CMD_MAX_ARG_LENGTH) {
                p->err = CMD_INPUT_TOO_LONG;
            }

            // Put a NULL at the end of the last cmd arg
            // (replaces a \r or \n, so nbd
            buffer[i] = 0;

#if ECHO_BACK_WHEN_PROCESSING_INCOMING_CHARS == 1
            // Make console go to beginning of next line
            if (ctx == &ctx_uart) {
                debug_print("\r\n");
            } else {
            	FreeRTOS_send(rawSocket, "\r\n", 2, 0);
            }
#endif

            p->ready = 1;

            // Update current pending cmd slot
            if (++ctx->pending_cmd_write_idx >= MAX_PENDING_CMDS) {
                ctx->pending_cmd_write_idx = 0;
            }
            p = &ctx->pending_cmds[ctx->pending_cmd_write_idx];
            p->ready = 0;

            // Move on to next char, which starts
            // next command sequence
            ctx->state = BEGIN;
            continue;
        }

#if ECHO_BACK_WHEN_PROCESSING_INCOMING_CHARS == 1
        // Echo character back to host
        if (ctx == &ctx_uart) {
            serial_write(&c, 1);
        } else {
        	FreeRTOS_send(rawSocket, &c, 1, 0);
        }
#endif

        // Process incoming char `c`
        switch (ctx->state) {
        case BEGIN:
            if (!isspace(c)) {
                // Populate first argument
                p->argc = 1;
                p->argv[0] = &buffer[i];
                p->err = CMD_SUCCESS; // Assume the parsing will work!
                p->curr_arg_length = 1;
                ctx->state = LOOKING_FOR_SPACE;
            }
            break;

        case LOOKING_FOR_SPACE:
            if (c != ' ') {
                p->curr_arg_length++;
            }

            if (c == ' ') {
                // End of chars for the arg

                // Set error flag if this arg was too long
                if (p->curr_arg_length > CMD_MAX_ARG_LENGTH) {
                    p->err = CMD_INPUT_TOO_LONG;
                }

                // Put NULL at end of arg (replaces ' ')
                buffer[i] = 0;

                ctx->state = LOOKING_FOR_CHAR;
            }
            break;

        case LOOKING_FOR_CHAR:
            if (c != ' ') {
                p->argv[p->argc] = &buffer[i];
                p->argc++;

                // Check if argc too big!
                if (p->argc > CMD_MAX_ARGC) {
                    p->err = CMD_INPUT_TOO_LONG;

                    // Put argc back to zero...
                    // NOTE: this ensure no buffer overruns,
                    //       but, screws up previous args.
                    //       This is okay as we are going to
                    //       throw away this pending cmd!
                    p->argc = 0;
                }

                p->curr_arg_length = 1;
                ctx->state = LOOKING_FOR_SPACE;
            }
            break;

        default:
            // Impossible!
            HANG;
            break;
        }
    }
}

static void commands_uart(void *arg)
{
	sm_parse_ascii_cmd_ctx_t *ctx = &ctx_uart;
	for (;;) {
		vTaskDelay(COMMANDS_INTERVAL_TICKS);

		// Read in bounded chunk of new chars
		//
		// NOTE: careful not to try and read too much!
		//       would cause a buffer overrun!
		//
		int try_to_read = MIN(UART_RX_FIFO_LENGTH, RECV_BUFFER_LENGTH - ctx->recv_buffer_idx);
		int num_bytes = uart_recv(&ctx->recv_buffer[ctx->recv_buffer_idx], try_to_read);

		// Run state machine to create pending cmds to execute
		_create_pending_cmds(ctx, &ctx->recv_buffer[ctx->recv_buffer_idx], num_bytes);

		// Move along in recv buffer
		ctx->recv_buffer_idx += num_bytes;
		if (ctx->recv_buffer_idx >= RECV_BUFFER_LENGTH) {
			ctx->recv_buffer_idx = 0;
		}
		parse_commands(ctx);
	}
}

static void commands_eth(void *arg)
{
	sm_parse_ascii_cmd_ctx_t *ctx = &ctx_eth;

    for (;;) {
    	vTaskDelay(COMMANDS_INTERVAL_TICKS);
    	int try_to_read = MIN(UART_RX_FIFO_LENGTH, RECV_BUFFER_LENGTH - ctx->recv_buffer_idx);
		int num_bytes = socket_recv(&ctx->recv_buffer[ctx->recv_buffer_idx], try_to_read, &rawSocket);

//		if (num_bytes > 0) {
//			xil_printf("commands_eth received %d chars: %s\n", num_bytes, &ctx->recv_buffer[ctx->recv_buffer_idx]);
//		}

		 // Run state machine to create pending cmds to execute
		_create_pending_cmds(ctx, &ctx->recv_buffer[ctx->recv_buffer_idx], num_bytes);

		// Move along in recv buffer
		ctx->recv_buffer_idx += num_bytes;
		if (ctx->recv_buffer_idx >= RECV_BUFFER_LENGTH) {
			ctx->recv_buffer_idx = 0;
		}
		parse_commands(ctx);
    }
}

static void parse_commands(void *arg)
{
    sm_parse_ascii_cmd_ctx_t *ctx = (sm_parse_ascii_cmd_ctx_t *) arg;

    // Get current pending cmd slot
    pending_cmd_t *p = &ctx->pending_cmds[ctx->pending_cmd_read_idx];

    if (p->ready) {
        // Run me!

        // Change current cmd source
        if (ctx == &ctx_uart) {
            current_cmd_source = CMD_SRC_UART;
        } else if (ctx == &ctx_eth) {
            current_cmd_source = CMD_SRC_ETH;
        } else {
            // unreachable
        }

#if ECHO_BACK_WHEN_PROCESSING_INCOMING_CHARS == 0
        // Echo back command to sender
        for (int i = 0; i < p->argc; i++) {
            cmd_resp_printf("%s", p->argv[i]);

            if (i + 1 < p->argc) {
                cmd_resp_print(" ");
            }
        }
        cmd_resp_print("\r\n");
#endif

        // Don't run a cmd that has errors
        int err = p->err;
        if (err == CMD_SUCCESS) {
            err = _command_handler(p->argc, p->argv);
        }

        // Display command status to user
        switch (err) {
        case CMD_SUCCESS_QUIET:
            // Don't print anything
            break;

        case CMD_SUCCESS:
            cmd_resp_printf("SUCCESS\r\n\n");
            break;

        case CMD_FAILURE:
            cmd_resp_printf("FAILURE\r\n\n");
            break;

        case CMD_INVALID_ARGUMENTS:
            cmd_resp_printf("INVALID ARGUMENTS\r\n\n");
            break;

        case CMD_INPUT_TOO_LONG:
            cmd_resp_printf("INPUT TOO LONG\r\n\n");
            break;

        case CMD_UNKNOWN_CMD:
            cmd_resp_printf("UNKNOWN CMD\r\n\n");
            break;

        default:
            cmd_resp_printf("UNKNOWN ERROR\r\n\n");
            break;
        }

        p->ready = 0;

        // Update READ index
        if (++ctx->pending_cmd_read_idx >= MAX_PENDING_CMDS) {
            ctx->pending_cmd_read_idx = 0;
        }
    }
}

void commands_cmd_init(command_entry_t *cmd_entry,
                       const char *cmd,
                       const char *desc,
                       command_help_t *help,
                       int num_help_cmds,
                       int (*cmd_function)(int, char **))
{
    cmd_entry->cmd = cmd;
    cmd_entry->desc = desc;
    cmd_entry->help = help;
    cmd_entry->num_help_cmds = num_help_cmds;
    cmd_entry->cmd_function = cmd_function;
    cmd_entry->next = NULL;
}

void commands_cmd_register(command_entry_t *cmd_entry)
{
    // Base case: there are no tasks in linked list
    if (cmds == NULL) {
        cmds = cmd_entry;
        cmds->next = NULL;
        return;
    }

    // Find end of list
    command_entry_t *curr = cmds;
    while (curr->next != NULL)
        curr = curr->next;

    // Append new cmd to end of list
    curr->next = cmd_entry;
    cmd_entry->next = NULL;
}

void commands_start_msg(void)
{
    cmd_resp_printf("\r\n");
    commands_display_help();
}

// _command_handler
//
// Takes `argc` and `argv` and finds
// the command function to call.
//
int _command_handler(int argc, char **argv)
{
    command_entry_t *c = cmds;

    while (c != NULL) {
        if (strcmp(argv[0], c->cmd) == 0) {
            // Found command to run!
            return c->cmd_function(argc, argv);
        }

        c = c->next;
    }

    return CMD_UNKNOWN_CMD;
}

// *****************
//  State Machine
//   which outputs
// the help messages
// *****************

typedef enum sm_states_e { TITLE1 = 1, TITLE2, TITLE3, CMD_HEADER, SUB_CMD, REMOVE_TASK } sm_states_e;

typedef struct sm_ctx_t {
    sm_states_e state;
    command_entry_t *curr;
    int sub_cmd_idx;
    TaskHandle_t tcb;
} sm_ctx_t;

static sm_ctx_t ctxG;

#define SM_UPDATES_PER_SEC (10000)
#define SM_INTERVAL_TICKS  (pdMS_TO_TICKS(1000.0 / SM_UPDATES_PER_SEC))

void help_state_machine(void *arg)
{
    sm_ctx_t *ctx = &ctxG;
    for (;;) {
    	vTaskDelay(SM_INTERVAL_TICKS);
		switch (ctx->state) {
		case TITLE1:
			cmd_resp_printf("\r\n");

			ctx->curr = cmds;
			ctx->state = TITLE2;
			break;

		case TITLE2:
			cmd_resp_printf("Available commands:\r\n");
			ctx->state = TITLE3;
			break;

		case TITLE3:
			cmd_resp_printf("-------------------\r\n");
			ctx->state = CMD_HEADER;
			break;

		case CMD_HEADER:
			if (ctx->curr == NULL) {
				// DONE!
				cmd_resp_printf("\r\n");
				ctx->state = REMOVE_TASK;
			} else {
				cmd_resp_printf("%s -- %s\r\n", ctx->curr->cmd, ctx->curr->desc);
				ctx->sub_cmd_idx = 0;
				ctx->state = SUB_CMD;
			}
			break;

		case SUB_CMD:
			if (ctx->sub_cmd_idx >= ctx->curr->num_help_cmds) {
				ctx->curr = ctx->curr->next;
				ctx->state = CMD_HEADER;
			} else {
				command_help_t *h = &ctx->curr->help[ctx->sub_cmd_idx++];
				cmd_resp_printf("\t%s -- %s\r\n", h->subcmd, h->desc);
			}
			break;

		case REMOVE_TASK:
			cmd_resp_printf("SUCCESS\r\n\n");
			vTaskDelete(ctx->tcb);
			break;

		default:
			// Can't happen
			HANG;
			break;
		}
    }
}

void commands_display_help(void)
{
    // Initialize the state machine context
    ctxG.state = TITLE1;
    ctxG.curr = cmds;
    ctxG.sub_cmd_idx = 0;

    // Initialize the state machine callback tcb (with high priority)
    xTaskCreate(help_state_machine, (const char *) "command_help", configMINIMAL_STACK_SIZE,
    				NULL, configMAX_PRIORITIES - 1, &ctxG.tcb);
}
