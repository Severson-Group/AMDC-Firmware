#ifndef CMD_CTRL_H
#define CMD_CTRL_H

// Called in app init function to register command with system
void cmd_ctrl_register(void);

// Function called when user types "ctrl" command into command prompt
// i.e., this is the ctrl command handler function
int cmd_ctrl(int argc, char **argv);

#endif // CMD_CTRL_H
