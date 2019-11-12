#ifndef CMD_BLINK_H
#define CMD_BLINK_H

// Called in app init function to register command with system
void cmd_blink_register(void);

// Function called when user types "blink" command into command prompt
// i.e., this is the blink command handler function
int cmd_blink(int argc, char **argv);

#endif // CMD_BLINK_H
