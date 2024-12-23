# Blink Application

This is a minimal application which shows how to use both commands and tasks.

## Usage

Follow the guide to load this application onto AMDC hardware. Connect to the UART serial port via your PC (115200 baud).

Once started, the RGB LED should begin cycling colors. The start-up messages should appear on the serial terminal. The list of available commands will appear. Try issuing the `blink hello <name>` command.

View the source code for the following files to learn how the application is implemented.

## Files

The application directory structure contains the minimal set of files to have a functioning app with a single command and task. All application files live in the application directory (`usr/blink/`).

```
blink/
|-- cmd/
|   |-- cmd_blink.c
|   |-- cmd_blink.h
|-- app_blink.c
|-- app_blink.h
|-- task_blink.c
|-- task_blink.h
```

### [`app_blink.c`](app_blink.c)

- Every application needs a top-level app file.
- This is called for app initialization during start-up.
- Users define what this app init function does.

### [`task_blink.c`](task_blink.c)

- This task cycles through the red, green, and blue LED colors at 1Hz.

### [`cmd/cmd_blink.c`](cmd/cmd_blink.c)

- This command echos the user-supplied name back to the terminal.
- Be careful, not all names are welcome!
