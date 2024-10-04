#ifndef CMD_VSI_H
#define CMD_VSI_H

// Called in app init function to register command with system
void cmd_vsiApp_register(void);

// Function called when user types "vsi" command into command prompt
// i.e., this is the vsi command handler function
int cmd_vsiApp(int argc, char **argv);

#endif // CMD_VSI_H
