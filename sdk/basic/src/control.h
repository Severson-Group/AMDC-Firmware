/*
 * control.h
 *
 *  Created on: Jun 24, 2014
 *      Author: sever212
 */

#ifndef CONTROL_H_
#define CONTROL_H_

typedef enum {C_IDLE, C_READY, C_RUNNING} controller_status;
typedef enum {C_PI, C_PID} controller_type;

typedef struct controller_context{
	double *inputs;	//reference - measured value of quantity being controlled
	double *outputs;	//output of the pi controller
	u32 NumInputs;	//number of inputs
	u32	NumOutputs;	//number of outputs
	controller_status Status;	//the status of the controller
	u32 NumClocks;	//run this controller every NumClocks count (1 for every time)
	u32 ClockTime;	//duration (in microseconds) of a clock edge
	u32 clockCount;	//current clock count
	controller_type Type;		//the type of controller (PI, PID)
} controller_context;

typedef void (*get_controller_inputs)(void *arg, struct controller_context *cnt);
typedef void (*take_controller_output)(void *arg, struct controller_context *cnt);

typedef struct pid_controller{
	double kp;		//proportional gain
	double kits;	//integral gain * sample time
	double kdn;		//derivative gain for current error: derivative difference EQ: D = kdn*E[n] + kdn1*E[n-1] + kdm1*D[n-1]
	double kdn1;	//derivative gain for previous error
	double kdm1;	//derivative gain for previous derivative
	double lastInt;	//value of last integral
	double lastErr;	//value of the last error
	double lastD;	//value of the last derivative output
	double plusSat;	//positive integral saturation limit
	double negSat;	//negative integral saturation limit
} pid_controller;

typedef struct pi_controller{
	double kp;		//proportional gain
	double kits;	//integral gain * sample time
	double lastInt;	//value of last integral
	double plusSat;	//positive integral saturation limit
	double negSat;	//negative integral saturation limit
} pi_controller;

typedef struct controller_holder{
	void *controller_info;					//specific to the type of controller
	struct controller_context *control_io;			//the io of the controller (general to all controllers)
	get_controller_inputs input_callback;	//function to call to get measured and reference values
	take_controller_output output_callback;	//function to call to take the controller output
	void *arg_in;	//argument to pass input_callback when called
	void *arg_out;	//argument to pass output_callback when called
} controller_holder;

typedef struct controller_linked_list_item
{
	struct controller_holder *context;
	struct controller_linked_list_item *next;
	struct controller_linked_list_item *prev;
	u8 bRegistered;		//set to true when added to the list, false when removed
} controller_linked_list_item;

extern volatile u32 ControlTimer0ErrorCount;
extern volatile u32 ControlTimer1ErrorCount;

extern int InitControllers(XScuGic* IntcInstancePtr);
//extern int InitControlTimers(XScuGic* IntcInstancePtr);
extern void RunPI_Controller (controller_holder* ctxt);
extern void RunPID_Controller (controller_holder* ctxt);
extern int UnregisterController(int contNum, struct controller_linked_list_item *cnt);
extern int RegisterController(int contNum, struct controller_linked_list_item *cnt);

#endif /* CONTROL_H_ */
