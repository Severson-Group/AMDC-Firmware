#ifndef COMMANDS_H
#define COMMANDS_H

#include "sys/scheduler.h"

#define COMMANDS_UPDATES_PER_SEC (10000)
#define COMMANDS_INTERVAL_USEC   (USEC_IN_SEC / COMMANDS_UPDATES_PER_SEC)

// Supported command handler return codes
#define CMD_SUCCESS           (0)
#define CMD_FAILURE           (1)
#define CMD_SUCCESS_QUIET     (2)
#define CMD_INVALID_ARGUMENTS (3)
#define CMD_INPUT_TOO_LONG    (4)
#define CMD_UNKNOWN_CMD       (5)

// Forward declarations
typedef struct command_entry_t command_entry_t;
typedef struct command_help_t command_help_t;

typedef struct command_entry_t {
    const char *cmd;
    const char *desc;
    command_help_t *help;
    int num_help_cmds;
    int (*cmd_function)(int, char **);

    // Pointer to next cmd; set this to NULL in user code.
    // When cmd is registered, this will form a linked list.
    command_entry_t *next;
} command_entry_t;

typedef struct command_help_t {
    const char *subcmd;
    const char *desc;
} command_help_t;

void commands_init(void);
void commands_callback_parse(void *arg);
void commands_callback_exec(void *arg);

void commands_cmd_init(command_entry_t *cmd_entry,
                       const char *cmd,
                       const char *desc,
                       command_help_t *help,
                       int num_help_cmds,
                       int (*cmd_function)(int, char **));
void commands_cmd_register(command_entry_t *cmd_entry);

void commands_start_msg(void);
void commands_display_help(void);

#endif // COMMANDS_H
