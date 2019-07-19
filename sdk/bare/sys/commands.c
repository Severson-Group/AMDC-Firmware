#include "commands.h"
#include "debug.h"
#include "defines.h"
#include "log.h"
#include "scheduler.h"
#include "serial.h"
#include "cmd/cmd_help.h"
#include "../drv/encoder.h"
#include "../drv/uart.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define RECV_BUFFER_LENGTH (4 * 1024)
static char recv_buffer[RECV_BUFFER_LENGTH] = {0};
static int recv_buffer_idx = 0;

#define CMD_MAX_ARGC        (16) // # of args accepted
#define CMD_MAX_ARG_LENGTH  (16) // max chars of any arg
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
static pending_cmd_t pending_cmds[MAX_PENDING_CMDS] = {0};
static int pending_cmd_write_idx = 0;
static int pending_cmd_read_idx = 0;

static int _command_handler(int argc, char **argv);

// Head of linked list of commands
command_entry_t *cmds = NULL;

static task_control_block_t tcb_parse;
static task_control_block_t tcb_exec;

void commands_init(void)
{
    printf("CMD:\tInitializing command tasks...\n");

    // Command parse task
    scheduler_tcb_init(&tcb_parse, commands_callback_parse, NULL, "command_parse", COMMANDS_INTERVAL_USEC);
    scheduler_tcb_register(&tcb_parse);

    // Command exec task
    scheduler_tcb_init(&tcb_exec, commands_callback_exec, NULL, "command_exec", COMMANDS_INTERVAL_USEC);
    scheduler_tcb_register(&tcb_exec);

    cmd_help_register();
}


typedef enum state_e {
    BEGIN = 1,
    LOOKING_FOR_SPACE,
    LOOKING_FOR_CHAR
} state_e;

state_e state = BEGIN;

void _create_pending_cmds(char *buffer, int length)
{
    // Get current pending cmd slot
    pending_cmd_t *p = &pending_cmds[pending_cmd_write_idx];

    for (int i = 0; i < length; i++) {
        char c = buffer[i];

        if (state != BEGIN && (c == '\n' || c == '\r')) {
            // End of a command!

            // Set error flag if this arg was too long
            if (p->curr_arg_length > CMD_MAX_ARG_LENGTH) {
                p->err = INPUT_TOO_LONG;
            }

            // Put a NULL at the end of the last cmd arg
            // (replaces a \r or \n, so nbd
            buffer[i] = 0;

            // Make console go to beginning of next line
            debug_printf("\r\n");

            p->ready = 1;

            // Update current pending cmd slot
            if (++pending_cmd_write_idx >= MAX_PENDING_CMDS) pending_cmd_write_idx = 0;
            p = &pending_cmds[pending_cmd_write_idx];
            p->ready = 0;

            // Move on to next char, which starts
            // next command sequence
            state = BEGIN;
            continue;
        }

        // Echo character back to terminal
        serial_write(&c, 1);

        // Process incoming char `c`
        switch (state) {
        case BEGIN:
            if (!isspace(c)) {
                // Populate first argument
                p->argc = 1;
                p->argv[0] = &buffer[i];
                p->err = SUCCESS; // Assume the parsing will work!
                p->curr_arg_length = 1;
                state = LOOKING_FOR_SPACE;
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
                    p->err = INPUT_TOO_LONG;
                }

                // Put NULL at end of arg (replaces ' ')
                buffer[i] = 0;

                state = LOOKING_FOR_CHAR;
            }
            break;

        case LOOKING_FOR_CHAR:
            if (c != ' ') {
                p->argv[p->argc] = &buffer[i];
                p->argc++;

                // Check if argc too big!
                if (p->argc > CMD_MAX_ARGC) {
                    p->err = INPUT_TOO_LONG;

                    // Put argc back to zero...
                    // NOTE: this ensure no buffer overruns,
                    //       but, screws up previous args.
                    //       This is okay as we are going to
                    //       throw away this pending cmd!
                    p->argc = 0;
                }

                p->curr_arg_length = 1;
                state = LOOKING_FOR_SPACE;
            }
            break;

        default:
            // Impossible!
            HANG;
            break;
        }
    }
}

void commands_callback_parse(void *arg)
{
    // Read in bounded chunk of new chars
    //
    // NOTE: careful not to try and read too much!
    //       would cause a buffer overrun!
    //
    int try_to_read = MIN(UART_RX_FIFO_LENGTH, RECV_BUFFER_LENGTH - recv_buffer_idx);
    int num_bytes = uart_recv(&recv_buffer[recv_buffer_idx], try_to_read);

    // Run state machine to create pending cmds to execute
    _create_pending_cmds(&recv_buffer[recv_buffer_idx], num_bytes);

    // Move along in recv buffer
    recv_buffer_idx += num_bytes;
    if (recv_buffer_idx >= RECV_BUFFER_LENGTH) {
        recv_buffer_idx = 0;
    }
}


void commands_callback_exec(void *arg)
{
    int err;

    // Get current pending cmd slot
    pending_cmd_t *p = &pending_cmds[pending_cmd_read_idx];

    if (p->ready) {
        // Run me!

        // Don't run a cmd that has errors
        err = p->err;
        if (err == SUCCESS) {
            err = _command_handler(p->argc, p->argv);
        }

        // Display command status to user
        switch (err) {
        case SUCCESS_QUIET:
            // Don't print anything
            break;

        case SUCCESS:
            debug_printf("SUCCESS\r\n\n");
            break;

        case FAILURE:
            debug_printf("FAILURE\r\n\n");
            break;

        case INVALID_ARGUMENTS:
            debug_printf("INVALID ARGUMENTS\r\n\n");
            break;

        case INPUT_TOO_LONG:
            debug_printf("INPUT TOO LONG\r\n\n");
            break;

        case UNKNOWN_CMD:
            debug_printf("UNKNOWN CMD\r\n\n");
            break;

        default:
            debug_printf("UNKNOWN ERROR\r\n\n");
            break;
        }

        p->ready = 0;

        // Update READ index
        if (++pending_cmd_read_idx >= MAX_PENDING_CMDS) {
            pending_cmd_read_idx = 0;
        }
    }
}

void commands_cmd_init(command_entry_t *cmd_entry,
        const char *cmd, const char *desc,
        command_help_t *help, int num_help_cmds,
        int (*cmd_function)(int, char**)
)
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
    while (curr->next != NULL) curr = curr->next;

    // Append new cmd to end of list
    curr->next = cmd_entry;
    cmd_entry->next = NULL;
}

void commands_start_msg(void)
{
    debug_printf("\r\n");
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

    return UNKNOWN_CMD;
}

// ****************
// State Machine
// which outputs
// the help messages
// ****************

typedef enum sm_states_e {
    TITLE1 = 1,
    TITLE2,
    TITLE3,
    CMD_HEADER,
    SUB_CMD,
    REMOVE_TASK
} sm_states_e;

typedef struct sm_ctx_t {
    sm_states_e state;
    command_entry_t *curr;
    int sub_cmd_idx;
    task_control_block_t tcb;
} sm_ctx_t;

#define SM_UPDATES_PER_SEC      (10000)
#define SM_INTERVAL_USEC        (USEC_IN_SEC / SM_UPDATES_PER_SEC)

void help_state_machine_callback(void *arg)
{
    sm_ctx_t *ctx = (sm_ctx_t *) arg;

    switch (ctx->state) {
    case TITLE1:
        debug_printf("\r\n");

        ctx->curr = cmds;
        ctx->state = TITLE2;
        break;

    case TITLE2:
        debug_printf("Available commands:\r\n");
        ctx->state = TITLE3;
        break;

    case TITLE3:
        debug_printf("-------------------\r\n");
        ctx->state = CMD_HEADER;
        break;

    case CMD_HEADER:
        if (ctx->curr == NULL) {
            // DONE!
            debug_printf("\r\n");
            ctx->state = REMOVE_TASK;
        } else {
            debug_printf("%s -- %s\r\n", ctx->curr->cmd, ctx->curr->desc);
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
            debug_printf("\t%s -- %s\r\n", h->subcmd, h->desc);
        }
        break;

    case REMOVE_TASK:
        scheduler_tcb_unregister(&ctx->tcb);
        break;

    default:
        // Can't happen
        HANG;
        break;
    }
}

static sm_ctx_t ctx;

void commands_display_help(void)
{
    // Initialize the state machine context
    ctx.state = TITLE1;
    ctx.curr = cmds;
    ctx.sub_cmd_idx = 0;

    // Initialize the state machine callback tcb
    scheduler_tcb_init(&ctx.tcb, help_state_machine_callback, &ctx, "help_sm", SM_INTERVAL_USEC);
    scheduler_tcb_register(&ctx.tcb);
}
