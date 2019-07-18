# Blink Application

This is a minimal application which shows how to use both commands and tasks.

## `app_blink.c`

- Every application needs a top-level app file.
- This is called for app initialization during start-up.
- Users define what this app init function does.

## `task_blink.c`

- This task turns on and off an LED at 1Hz.

## `cmd/cmd_blink.c`

- This command echos the user-supplied name back to the terminal.
- Be careful, not all names are welcome!