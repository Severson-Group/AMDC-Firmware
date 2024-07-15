/*
 * ControlApp.h
 *
 *  Created on: Jun 24, 2014
 *      Author: sever212
 */

#ifndef CONTROLAPP_H_
#define CONTROLAPP_H_

#define NUM_PI_CONTROLLERS 12

extern int ControlStartAppCommand(const char *szCmd, char *szResponse, void *CommDevice);
extern int ControlAppCommand(const char * szCmd, char *szResponse, void *CommDevice);
extern int InitControlApp();

#endif /* CONTROLAPP_H_ */
