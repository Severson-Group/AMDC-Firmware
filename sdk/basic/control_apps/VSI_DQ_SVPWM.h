#ifndef __VSI_DQ_SVPWM_H
#define __VSI_DQ_SVPWM_H


#define PI 3.14159265
#define PI2 6.283185307179586
#define PI23 2.094395102393195
#define DQK 0.816496580927726

#define VSI_DQ_SVPWM_NUM_VSI 5

typedef struct pi_house
{
	pi_controller pi;
	controller_context io;
	controller_holder ctxt;
	double input;
	double output;
	double reference;
	u8 updated;	//set to TRUE everytime this controller output is updated, cleared everytime the output is used
	//1 to 1 double adc_scale;		//3 amps per volt, wound around twice gives me a factor of 3/2
} pi_house;

typedef u8 (*VSI_DQ_SVPWM_input)(void *arg, void *VSIcontext);
typedef void (*VSI_DQ_SVPWM_output)(void *arg, void *VSIcontext);

typedef struct VSI_context
{
	pi_house d;
	pi_house q;
	pi_house zero;
	u32 InputAtTickCount;		//the tick count of the last time the inputs were updated (to avoid unnecessary dq transformations)
	double Iabc[3];
	double Idq0[3];
	double dabc[3];
	double mabc[3];				//the 3 output modulation indices
	double dqComp[2];			//compensation d and q terms. These are added to the controller outputs before saturating the result.
	double theta_da;
	u8 bUseZeroSequence;		//set to 1 to use a zero sequence controller, otherwise 0
	VSI_DQ_SVPWM_input get_input;
	void * input_arg;
	VSI_DQ_SVPWM_output write_output;
	void * output_arg;
	u8 bSineTriangle;			//0 for CSVPWM, 1 for SineTriangle
} VSI_context;




//move these to a library:
int ChangeDQ_ReferenceFrame(double *new_dq0, double new_theta_da, double *old_dq0, double old_theta_da);
int SaturateDQVoltages(double *inputDQ, double *outputDQ, u8 bSineTriangle);
extern int DQ0_Transform(double theta_da, double *abc, double *dq0);
extern int InverseDQ0_Transform(double theta_da, double *abc, double *dq0);

extern int Start_VSI_DQ_SVPWM_Controller(u32 vsi);
extern int Stop_VSI_DQ_SVPWM_Controller(u32 vsi);
extern int Init_VSI_DQ_VSPWM(u8 StartAllControllers);
extern int VSI_DQ_VSPW_StopCommand(const char *szCmd, char *szResponse, void *CommDevice);
extern int VSI_DQ_VSPW_StartCommand(const char *szCmd, char *szResponse, void *CommDevice);
extern int VSI_DQ_VSPW_Command(const char * szCmd, char *szResponse, void *CommDevice);
extern int VSI_DQ_VSPW_SetCurrentRef(u32 vsi, u8 bDaxis, double reference);

extern VSI_context VSI_DQ_Cur_Controllers[VSI_DQ_SVPWM_NUM_VSI];
#endif
