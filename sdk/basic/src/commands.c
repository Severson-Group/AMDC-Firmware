
#if 0
#include "xparameters.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "main.h"
#include "project_settings.h"
#include "log.h"
#include "prog_timer.h"
#endif

#include "project_include.h"

#include "../bsp/bsp.h"

/*****************************
 * MakeMostFullLFR()
 * Populate an LFR response from the most-full log
 *
 *
 * Response: LFR=sqn_num,index,flags,length,binary_data
 * **The response length will be different from the
 * **  command length if the end of the log is reached
 * **binary_data is byte-for-byte what is in the log.
 * **sqn_num is a sequence number that starts at 1 (reset when LFS is received)
 * 	and increments with each LFR
 * --> only 100us errors are but response is padded to 10 fields
 *szResponse: the response
 *uiMaxLength: the maximum length of the binary_data field (must be divisible by 4)
 */
int MakeMostFullLFR(char *szResponse, u32 uiMaxLength, u32 *SQnum)
{
	u32 uiLogIndex = 0, uiFlags = 0;
	int iStatus1 = ReadFromMostFullLog((unsigned char*) szResponse, &uiLogIndex, uiMaxLength);
	int iStatus2 = ReadLogFlags(&uiFlags, uiLogIndex);
	if ( (iStatus1 < 0) || (iStatus2 < 0))
		return -1;
	else if (iStatus1 == 0)
		return 0;
	else
	{
		char szTemp[64];
		if (uiFlags != 0)
			uiFlags = uiFlags; //breakpoint
		sprintf(szTemp, "LFR=%d,%d,%d,%d,", (int) *SQnum, (int) uiLogIndex, (int) uiFlags, (int) iStatus1);
		*SQnum = *SQnum + 1;
		memmove(&szResponse[strlen(szTemp)], szResponse, iStatus1);
		memcpy(szResponse, szTemp, strlen(szTemp));
		return strlen(szTemp) + iStatus1;
	}
}

//Streaming FIFO
u32 StreamFIFO_SqnNum = 0;
void *StreamFIFOComm = 0;
u32 StreamFIFO_MaxPacket = 0; //maximum LFR binary data field size in bytes
char bPrimeStreamFIFO = FALSE;	//set this to true if the streaming FIFO needs to be loaded from the main loop (instead of the callback)

int ReloadStreamFIFO(void * comm)
{
	int retVal = 0;
	int status;
	err_t err;
	if (comm > 0)
	{
		unsigned char ucSendAnother;
		char SendDataBuff[MAX_SEND_PACKET_SIZE];
		do
		{
			u32 tcp_space_avail = tcp_sndbuf((struct tcp_pcb *)comm);
			u32 uiMaxLen = ((u32)(tcp_space_avail/4) * 4)-64; //-64 is to account for the header
			status = 0;
			ucSendAnother = 0;
			if (uiMaxLen > StreamFIFO_MaxPacket)
				uiMaxLen = StreamFIFO_MaxPacket;
			if (uiMaxLen > 0)
				status = MakeMostFullLFR(&SendDataBuff[2], uiMaxLen, &StreamFIFO_SqnNum);

			if (status > 0)
			{
				u32 dummy;
				status += 4; //for \n\r
				SendDataBuff[0] = '\n'; SendDataBuff[1] = '\r';
				SendDataBuff[status-2] = '\n'; SendDataBuff[status-1] = '\r';
				if ( (tcp_space_avail - status >= MAX_SEND_PACKET_SIZE) && (DetermineMostFullLog(&dummy) >= MAX_SEND_PACKET_SIZE/4 )) 	//if we have at least a half packet that we can send
					ucSendAnother = 2;

				err = tcp_write(comm, SendDataBuff, status, 1+ ucSendAnother);
				if (err != ERR_OK)
					status = 0; //break point
				retVal += status;
			}

		} while(ucSendAnother > 0 );
	}
	return retVal;
}

//internal functions
static int CmdBMK(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdCFG(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdCNT(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdCST(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdCTT(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdHBA(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdHBV(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdLFH(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdLM(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdLFR(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdLFS(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdLS(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdPRE(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdReadADC(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdWD(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdTRE(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdTRT(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdTCE(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdVSL(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdVSI(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdFSW(const char * szCmd, char *szResponse, void *CommDevice);
static int CmdDEA(const char * szCmd, char *szResponse, void *CommDevice);

typedef struct command_table_entry {
	char *szCmd;
	char *szDescription;
	int (*cmd_function)(const char *, char *, void *);
} command_table_entry;

#define NUM_CMD 22
command_table_entry command_table[NUM_CMD] = {
		{"BMK", "Benchmark the PI controller logic", CmdBMK},
		{"CFG", "CFG=d: Write setting d to configuration register (d = 0 to 255)", CmdCFG},
		{"CNT", "CNT=n,cmd: Send the control application a message", CmdCNT},
		{"CST", "CST=n,cmd: Start control application n with startup command cmd", CmdCST},
		{"CTT", "CTT=n,cmd: Terminate control application n with command cmd", CmdCTT},
		{"HBA", "HBV=l1,l2,V,omega: Make an H-bridge from legs l1 and l2, output V (0-100), with omega (=int*128)", CmdHBA},
		{"HBV", "HBV=l1,l2,V: Make an H-bridge from legs l1 and l2 and output V (0-100)", CmdHBV},
		{"LFH", "Stop logging and hault sending unsolicited LFRs", CmdLFH},
		{"LFR", "Read a log output", CmdLFR},
		{"LFS", "Start logging and stream FIFO outputs (unsolicited LFRs sent until LFH recv'd)", CmdLFS},
		{"LM", "Log a memory address", CmdLM},
		{"LS", "Start logging", CmdLS},
		{"PRE", "PRE=toDev,fromDev: Read PECB data bus error count", CmdPRE},
		{"RADC", "Read ADC converter output", CmdReadADC},
		{"TCE", "Clear timing errors", CmdTCE},
		{"TRE", "Read timing errors", CmdTRE},
		{"TRT", "Read timer ticks", CmdTRT},
		{"WD", "WDn=d: Write d duty cycle register n (0 for off)", CmdWD},
		{"VSL", "VSL=l1,l2,l3: Set output legs for VSI", CmdVSL},
		{"VSI", "VSI=V,freq(,ramptime): Set V (percent output voltage, 0-100), with freq (Hz). Optional ramp time (ms)", CmdVSI},
		{"FSW", "FSW=frequency: Set PWM switching frequency (100Hz to 4MHz)", CmdFSW},
		{"DEA", "DEA=ns: Set PWM dead time in ns (25ns to ...)", CmdDEA}
	};





/*****************************
 * CmdLR()
 * Handle the LR command. Read portion of the log.
 * LR=index,start,length
 *
 * (must have already send LM commands to setup the log)
 *
 * Response: LR=index,start,length,binary_data
 * **The response length will be different from the
 * **  command length if the end of the log is reached
 * **binary_data is byte-for-byte what is in the log.
 * --> only 100us errors are but response is padded to 10 fields
 *szCmd: the command
 *szResponse: the response
 */
#if 0
static int CmdLR(const char * szCmd, char *szResponse, void *CommDevice)
{
	const char *szNum = &szCmd[3];
	char bError = 0;
	int retVal = 0;
	u32 uiLogIndex, uiStart, uiLength;

	//get the log index number
	uiLogIndex = atoi(szNum);

	if ( uiLogIndex >= MAX_LOG_VARS )
		bError = 1;
	else
	{
//get the number of bytes to read
		int i;
		for (i = 0; i < strlen(szCmd); i++)
			if (szCmd[i] == ',')
			{
				szNum = &szCmd[i+1];
				break;
			}
		if (i == strlen(szCmd))
			bError = 1;
		else
		{
			uiStart = atoi(szNum);

			for (i++; i < strlen(szCmd); i++)
				if (szCmd[i] == ',')
				{
					szNum = &szCmd[i+1];
					break;
				}
			if (i == strlen(szCmd))
				bError = 1;
			else
			{
				int iStatus;
				uiLength = atoi(szNum);
				iStatus = ReadFromLog((unsigned char*) szResponse, uiLogIndex, uiStart, uiLength);
				if (iStatus < 0)
					bError = 1;
				else
				{
					char szTemp[64];
					sprintf(szTemp, "LR=%d,%d,%d,", (int) uiLogIndex, (int) uiStart, (int) iStatus);
					memmove(&szResponse[strlen(szTemp)], szResponse, iStatus);
					memcpy(szResponse, szTemp, strlen(szTemp));
					retVal = strlen(szTemp) + iStatus;
				}
			}

		}
	}
	if (bError)
	{
		strcat(szResponse, "ERROR");
		retVal = strlen(szResponse);
	}

	return retVal;
}
#endif
/*****************************
 * CmdLLR()
 * Handle the LFR command. Read portion of the log.
 * LFR=index,length
 *
 * (must have already send LM commands to setup the log)
 *
 * Response: LFR=sqn_num, index,flags,length,binary_data
 * **The response length will be different from the
 * **  command length if the end of the log is reached
 * **binary_data is byte-for-byte what is in the log.
 * --> only 100us errors are but response is padded to 10 fields
 * **sqn_num is always 1
 *szCmd: the command
 *szResponse: the response
 */
static int CmdLFR(const char * szCmd, char *szResponse, void *CommDevice)
{
	const char *szNum = &szCmd[4];
	char bError = 0;
	int retVal = 0;
	int iStatus1, iStatus2;
	u32 uiLogIndex, uiLength, uiFlags;

	//get the log index number
	uiLogIndex = atoi(szNum);

	if ( uiLogIndex >= MAX_LOG_VARS )
		bError = 1;
	else
	{
//get the number of bytes to read
		int i;
		for (i = 0; i < strlen(szCmd); i++)
			if (szCmd[i] == ',')
			{
				szNum = &szCmd[i+1];
				break;
			}
		if (i == strlen(szCmd))
			bError = 1;
		else
		{
			uiLength = atoi(szNum);

			iStatus1 = ReadFromLog((unsigned char*) szResponse, uiLogIndex, uiLength);
			iStatus2 = ReadLogFlags(&uiFlags, uiLogIndex);
			if ( (iStatus1 < 0) || (iStatus2 < 0))
				bError = 1;
			else
			{
				char szTemp[64];
				sprintf(szTemp, "LFR=1,%d,%d,%d,", (int) uiLogIndex, (int) uiFlags, (int) iStatus1);
				memmove(&szResponse[strlen(szTemp)], szResponse, iStatus1);
				memcpy(szResponse, szTemp, strlen(szTemp));
				retVal = strlen(szTemp) + iStatus1;
			}
		}
	}
	if (bError)
	{
		strcat(szResponse, "ERROR");
		retVal = strlen(szResponse);
	}

	return retVal;
}


/*****************************
 * CmdBMK()
 * Benchmark a PI controller
 * BMK
 *
 * (must have already send LM commands to setup the log)
 *
 * Response: BMK=num
 * number of time the PI controller was called in 1 second.
 *szCmd: the command
 *szResponse: the response
 */
static int CmdBMK(const char * szCmd, char *szResponse, void *CommDevice)
{
	u32 started = MainTimerTimerTick;
	u32 count = 0;
	double input;
	double output;
	double abc[3], dq0[3];
	pi_controller cont;
	controller_context io;
	controller_holder ctxt;
	io.ClockTime = 10;
	io.NumClocks = 1;
	cont.kits = 10.315;
	cont.kp = 1.42;
	cont.plusSat = 5.12;
	cont.negSat = -100.91;
	cont.lastInt = 0;
	io.inputs = &input;
	io.outputs = &output;
	io.NumInputs = 1;
	io.NumOutputs = 1;
	io.Status = C_RUNNING;
	ctxt.control_io = &io;
	ctxt.controller_info = &cont;
	while (MainTimerTimerTick - started < 100000)
	{
		/*input += 0.1;
		if (input > cont.plusSat + 0.5)
			input = cont.negSat;
		RunPI_Controller(&ctxt);*/
		abc[0] = count/1000.0;
		abc[1] = (double) MainTimerTimerTick/100000.0;
		abc[2] = 3*abc[0];
		DQ0_Transform(((double) MainTimerTimerTick)/100000.0, abc, dq0);
		count++;
	}

	sprintf(szResponse, "BMK=%d", (int) count);
	return strlen(szResponse);
}


/*****************************
 * CmdLS()
 * Handle the LS command. Start logging.
 * LS
 *
 * (must have already send LM commands to setup the log)
 *
 * Response: OK
 * --> only 100us errors are but response is padded to 10 fields
 *szCmd: the command
 *szResponse: the response
 */
static int CmdLS(const char * szCmd, char *szResponse, void *CommDevice)
{
	if (StartLogging() == SUCCESS)
		strcat(szResponse, "OK");
	else
		strcat(szResponse, "ERROR");

	return strlen(szResponse);
}

/*****************************
 * CmdLFS()
 * Handle the LFS command. Start logging and start sending unsolicited
 * LFRs.
 * LFS=max_packet_size (in bytes)
 *
 * (must have already sent LM commands to setup the log)
 *
 * Response: OK
 *szCmd: the command
 *szResponse: the response
 */
static int CmdLFS(const char * szCmd, char *szResponse, void *CommDevice)
{
	const char *szNum = &szCmd[4];
	u32 tempLen = 0;

	//get the log index number
	tempLen = atoi(szNum);

	if ( (StartLogging() == SUCCESS) && (tempLen > 0) && (tempLen % 4 == 0))
	{
		strcat(szResponse, "OK");
		StreamFIFO_SqnNum = 1;
		StreamFIFO_MaxPacket = tempLen;
		StreamFIFOComm = CommDevice;
		bPrimeStreamFIFO = TRUE;
	}
	else
		strcat(szResponse, "ERROR");

	return strlen(szResponse);
}

/*****************************
 * CmdLFH()
 * Handle the LFH command. Hault sending unsolicited LFRs
 * LFH
 *
 *
 * Response: OK
 * --> only 100us errors are but response is padded to 10 fields
 *szCmd: the command
 *szResponse: the response
 */
static int CmdLFH(const char * szCmd, char *szResponse, void *CommDevice)
{
	StreamFIFOComm = 0;
	bPrimeStreamFIFO = FALSE;

	StopAllLogging();
	strcat(szResponse, "OK");

	return strlen(szResponse);
}




/*****************************
 * CmdLM()
 * Handle the LM command. Log a memory address
 * LM=index,address,settings
 * index: log file index (0 to MAX_LOG_VARS-1)
 * Response: OK
 * --> only 100us errors are but response is padded to 10 fields
 *szCmd: the command
 *szResponse: the response
 */
static int CmdLM(const char * szCmd, char *szResponse, void *CommDevice)
{
	const char *szNum = &szCmd[3];
	char bError = 0;
	u32 uiLogIndex, uiAddr, uiSettings;

	//get the log index number
	uiLogIndex = atoi(szNum);

	if ( uiLogIndex >= MAX_LOG_VARS )
		bError = 1;
	else
	{
//get the memory address
		int i;
		for (i = 0; i < strlen(szCmd); i++)
			if (szCmd[i] == ',')
			{
				szNum = &szCmd[i+1];
				break;
			}
		if (i == strlen(szCmd))
			bError = 1;
		else
		{
			uiAddr = atoi(szNum);
			for (i++; i < strlen(szCmd); i++)
				if (szCmd[i] == ',')
				{
					szNum = &szCmd[i+1];
					break;
				}
			if (i == strlen(szCmd))
				bError = 1;
			else
			{
				uiSettings = atoi(szNum);

				if (SetupLogVariable(uiLogIndex, (u32 *) uiAddr, uiSettings) != SUCCESS)
					bError = 1;
				else
					strcat(szResponse, "OK");
			}
		}
	}
	if (bError)
		strcat(szResponse, "ERROR");

	return strlen(szResponse);
}


unsigned char HBA_bEnabled = 0;
u32 HBA_uiLeg1, HBA_uiLeg2;
double HBA_dbVPercent, HBA_dbOmega, HBA_dbTheta;

/*****************************
 * CmdHBA()
 * Handle the 100uS tick for the ac HBridge function.
 */
void HBA_100usTick()
{
	//generate ac for Hbridge
	if (HBA_bEnabled){
		u8 duty1, duty2;
		double update_da = ((double) HBA_dbOmega/( (double) 10000));
		double dbVPercent;
		HBA_dbTheta += update_da;
		if (HBA_dbTheta > 6.283185307179586)
			HBA_dbTheta -= 6.283185307179586;

		dbVPercent = HBA_dbVPercent*cos(HBA_dbTheta);

		duty1 = (unsigned char) 127*(1 + dbVPercent);
		duty2 = (unsigned char) 127*(1 - dbVPercent);

		WriteDutyRatio(HBA_uiLeg1, duty1);
		WriteDutyRatio(HBA_uiLeg2, duty2);
	}
}


// Ramps between `current` and `setpoint` values assuming Fs = 10kHz
// Updates `current` as it ramps.
//
// `ramprate` is in (units of `current`) / sec
//     0 implies no ramp
//
//
// Returns delta in Ts for theta update
//
double omega_ramp_fcn(double *current, double *setpoint, double ramprate)
{
	double ret;

	if (ramprate != 0 && *current != *setpoint) {
		// Ramping
		double dir = (*setpoint > *current) ? 1 : -1;
		double del = ramprate / 10000.0;

		*current += (dir * del);
		ret = *current / 10000.0;

		// Check if done ramping
		if ((dir == 1)  && (*current > *setpoint)) *current = *setpoint;
		if ((dir == -1) && (*current < *setpoint)) *current = *setpoint;
	} else {
		// Not ramping
		ret = *setpoint / 10000.0;
	}

	return ret;
}

uint8_t VSI_enabled = 0;
uint8_t VSI_leg1;
uint8_t VSI_leg2;
uint8_t VSI_leg3;
double VSI_Vpercent = 0;
double VSI_omega = 0;
double theta = 0;

double VSI_omega_ramp = 0;
double VSI_old_Vpercent = 0;
double VSI_old_omega = 0;

double VSI_R;
double VSI_V0;

/*****************************
 * CmdVSI()
 * Handle the 100uS tick for the VSI function.
 */
void VSI_100usTick(void)
{
	if (VSI_enabled) {
//		uint8_t duty1, duty2, duty3;

		// Calculate `da`
		double update_da = omega_ramp_fcn(&VSI_old_omega, &VSI_omega, VSI_omega_ramp);

		// Calculate dv
		// ramp(&VSI_old_Vpercent, &VSI_Vpercent, 0.1);

		theta += update_da;
		if (theta > 6.283185307179586)
			theta -= 6.283185307179586;

		double v = (VSI_R * VSI_old_omega) + VSI_V0;

		double percent1 = v*cos(theta);
		double percent2 = v*cos(theta - PI23);
		double percent3 = v*cos(theta + PI23);

//		duty1 = (unsigned char) 127*(1 + percent1);
//		duty2 = (unsigned char) 127*(1 + percent2);
//		duty3 = (unsigned char) 127*(1 + percent3);

		pwm_set_duty(VSI_leg1 - 1, (1 + percent1) / 2.0);
		pwm_set_duty(VSI_leg2 - 1, (1 + percent2) / 2.0);
		pwm_set_duty(VSI_leg3 - 1, (1 + percent3) / 2.0);

//		WriteDutyRatio(VSI_leg1, duty1);
//		WriteDutyRatio(VSI_leg2, duty2);
//		WriteDutyRatio(VSI_leg3, duty3);
	} else {
		WriteDutyRatio(VSI_leg1, 0);
		WriteDutyRatio(VSI_leg2, 0);
		WriteDutyRatio(VSI_leg3, 0);
	}
}


/*****************************
 * CmdHBA()
 * Handle the HBA command. Make an HBridge from two legs and
 * output the specified voltage as a sine wave varying from 0
 * to the peak value specified
 * HBV=leg1,leg2,voltage,omega
 * leg1: number of leg 1 (1 to NUM_LEGS)
 * leg2: number of leg 2 (1 to NUM_LEGS)
 * voltage: percent of bus voltage (0 to 100)
 * omega: 0 to 2*pi*10000, specified as an integer which must
 * 			be divided by 128
 * Response: OK
 *szCmd: the command
 *szResponse: the response
 */
static int CmdHBA(const char * szCmd, char *szResponse, void *CommDevice)
{
	const char *szNum = &szCmd[4];
	char bError = 0;
	u32 uiLeg1, uiLeg2, uiVoltagePercent;

//get the leg 1 number
	uiLeg1 = atoi(szNum);

	if ( (uiLeg1 > NUM_LEGS) || (uiLeg1 == 0) )
		bError = 1;
	else
	{
//get the leg 2 number
		int i;
		for (i = 0; i < strlen(szCmd); i++)
			if (szCmd[i] == ',')
			{
				szNum = &szCmd[i+1];
				break;
			}
		if (i == strlen(szCmd))
			bError = 1;
		else
		{
			uiLeg2 = atoi(szNum);
			if ( (uiLeg2 > NUM_LEGS) || (uiLeg2 == 0) )
				bError = 1;
			else
			{
				for (i++; i < strlen(szCmd); i++)
					if (szCmd[i] == ',')
					{
						szNum = &szCmd[i+1];
						break;
					}
				if (i == strlen(szCmd))
					bError = 1;
				else
				{
					double dbVPercent;
					uiVoltagePercent = atoi(szNum);
					dbVPercent = ((double) uiVoltagePercent)/100;

					for (i++; i < strlen(szCmd); i++)
						if (szCmd[i] == ',')
						{
							szNum = &szCmd[i+1];
							break;
						}
					if (i == strlen(szCmd))
						bError = 1;
					else
					{
						HBA_dbOmega = ((double) atoi(szNum)) / ((double) 128);
						HBA_dbVPercent = dbVPercent;
						HBA_uiLeg1 = uiLeg1;
						HBA_uiLeg2 = uiLeg2;
						HBA_bEnabled = 1;
						strcat(szResponse, "OK");
					}
				}
			}
		}
	}
	if (bError)
		strcat(szResponse, "ERROR");

	return strlen(szResponse);
}

/*****************************
 * CmdHBV()
 * Handle the HBV command. Make an HBridge from two legs and
 * output the specified voltage
 * HBV=leg1,leg2,voltage
 * leg1: number of leg 1 (1 to NUM_LEGS)
 * leg2: number of leg 2 (1 to NUM_LEGS)
 * voltage: percent of bus voltage (0 to 100)
 * Response: OK
 *szCmd: the command
 *szResponse: the response
 */
static int CmdHBV(const char * szCmd, char *szResponse, void *CommDevice)
{
	const char *szNum = &szCmd[4];
	char bError = 0;
	u32 uiLeg1, uiLeg2, uiVoltagePercent;

//get the leg 1 number
	uiLeg1 = atoi(szNum);

	if ( (uiLeg1 > NUM_LEGS) || (uiLeg1 == 0) )
		bError = 1;
	else
	{
//get the leg 2 number
		int i;
		for (i = 0; i < strlen(szCmd); i++)
			if (szCmd[i] == ',')
			{
				szNum = &szCmd[i+1];
				break;
			}
		if (i == strlen(szCmd))
			bError = 1;
		else
		{
			uiLeg2 = atoi(szNum);
			if ( (uiLeg2 > NUM_LEGS) || (uiLeg2 == 0) )
				bError = 1;
			else
			{
				for (i++; i < strlen(szCmd); i++)
					if (szCmd[i] == ',')
					{
						szNum = &szCmd[i+1];
						break;
					}
				if (i == strlen(szCmd))
					bError = 1;
				else
				{
					double dbVPercent;
					u8 duty1, duty2;
					uiVoltagePercent = atoi(szNum);
					dbVPercent = ((double) uiVoltagePercent)/100;
					duty1 = (unsigned char) 127*(1 + dbVPercent);
					duty2 = (unsigned char) 127*(1 - dbVPercent);

					WriteDutyRatio(uiLeg1, duty1);
					WriteDutyRatio(uiLeg2, duty2);
					strcat(szResponse, "OK");
				}
			}
		}
	}
	if (bError)
		strcat(szResponse, "ERROR");

	return strlen(szResponse);
}


/*****************************
 * CmdReadADC()
 * Handle the RADC command
 * RADCn - n is adc number (1 - 12)
 * Response: RADCn=xx.xxxxV
 *szCmd: the command
 *szResponse: the response
 */
static int CmdReadADC(const char * szCmd, char *szResponse, void *CommDevice)
{
	const char *szADCnum = &szCmd[4];
	double dbADC = 0;
	u32 uiADCnum = atoi(szADCnum);
	if ( (uiADCnum > 0) && (uiADCnum < 13) )
	{
		dbADC = ReadADC(uiADCnum);
		sprintf(szResponse, "ADC%d=%2.4fV", (int)uiADCnum,dbADC); //reads +-10V, accurate to 0.0049V
	}
	else
		strcpy(szResponse, "ERROR");

	return strlen(szResponse);
}

/*****************************
 * CmdTCE()
 * Handle the TCE command. Clear timing errors.
 * RTE
 * Response: OK
 * --> only 100us errors are but response is padded to 10 fields
 *szCmd: the command
 *szResponse: the response
 */
static int CmdTCE(const char * szCmd, char *szResponse, void *CommDevice)
{
	Timing10usErrorCount = 0;
	Timing100usErrorCount = 0;
	ControlTimer0ErrorCount = 0;
	ControlTimer1ErrorCount = 0;
	strcpy(szResponse, "OK");

	return strlen(szResponse);
}


/*****************************
 * CmdTRT()
 * Handle the TRT command. Read timing ticks.
 * RTE
 * Response: TRT=main,control0,control1,,,,,,,
 * --> only first 3 are defined but response is padded to 10 fields
 *szCmd: the command
 *szResponse: the response
 */
static int CmdTRT(const char * szCmd, char *szResponse, void *CommDevice)
{
	/*u32 TimerCount1 = XTmrCtr_ReadReg(0x42800000,
			TIMER_CNTR_0, XTC_TCR_OFFSET);*/

	sprintf(szResponse, "TRT=%d,%d,%d,,,,,,,", (int) MainTimerTimerTick, (int) ControlTimer0_TickCount, (int) ControlTimer1_TickCount);

	return strlen(szResponse);
}

/*****************************
 * CmdPRE()
 * Handle the PRE command. Read PECB databus errors.
 * RTE
 * Response: PRE=ToPECB,FromPECB
 *szCmd: the command
 *szResponse: the response
 */
static int CmdPRE(const char * szCmd, char *szResponse, void *CommDevice)
{
	/*u32 TimerCount1 = XTmrCtr_ReadReg(0x42800000,
			TIMER_CNTR_0, XTC_TCR_OFFSET);*/
	u32 ec_to_dev, ec_from_dev;
	ec_to_dev = PECB[ER_CNT] & 0xFFFF;
	ec_from_dev = (PECB[ER_CNT] >> 16) & 0xFFFF;
	sprintf(szResponse, "TRE=%d,%d,%d,%d", (int) ec_to_dev, (int) ec_from_dev, (int) PECB[ER_CNT-1], (int) PECB[ER_CNT-2]);

	return strlen(szResponse);
}


/*****************************
 * CmdTRE()
 * Handle the TRE command. Read timing errors.
 * RTE
 * Response: TRE=10us,100us,,,,,,,,
 * --> only 10us, 100us errors are but response is padded to 10 fields
 *szCmd: the command
 *szResponse: the response
 */
static int CmdTRE(const char * szCmd, char *szResponse, void *CommDevice)
{
	/*u32 TimerCount1 = XTmrCtr_ReadReg(0x42800000,
			TIMER_CNTR_0, XTC_TCR_OFFSET);*/

	sprintf(szResponse, "TRE=%d,%d,%d,%d,,,,,,", (int) Timing10usErrorCount, (int) Timing100usErrorCount, (int) ControlTimer0ErrorCount, (int) ControlTimer1ErrorCount);

	return strlen(szResponse);
}


/*****************************
 * CmdCFG()
 * Handle the CFG command. Write setting to config register.
 * CFG=d
 * --> d: (0 to 255)
 *szCmd: the command
 *szResponse: the response
 */
static int CmdCFG(const char * szCmd, char *szResponse, void *CommDevice)
{
	const char *szNum = &szCmd[4];
	char bError = 0;
	u32 uiValue;
	if ( (szNum[0] < 0x30) || (szNum[0] > 0x39) )
		bError = 1;
	else
	{
//get the value
		uiValue = atoi(szNum);
		WriteConfigRegister((u8) uiValue);
		sprintf(szResponse, "OK");
	}
	if (bError)
		strcpy(szResponse, "ERROR");

	return strlen(szResponse);
}


/*****************************
 * CmdWD()
 * Handle the WD command. Write duty ratio.
 * WDn=d
 * --> n: duty ratio register to write to
 * --> d: (0 to 255)
 *szCmd: the command
 *szResponse: the response
 */
static int CmdWD(const char * szCmd, char *szResponse, void *CommDevice)
{
	const char *szNum = &szCmd[2];
	char bError = 0;
	u32 uiLeg, uiValue;
	if ( (szNum[0] < 0x31) )
		bError = 1;
	else
	{
//get the leg number
		uiLeg = atoi(szNum);

		if ( (uiLeg < 1) || (uiLeg > NUM_LEGS) )
			bError = 1;
		else
		{
//get new duty ratio
			int i;
			for (i = 0; i < strlen(szCmd); i++)
				if (szCmd[i] == '=')
				{
					szNum = &szCmd[i+1];
					break;
				}
			if (i == strlen(szCmd))
				bError = 1;
			else
			{
				uiValue = atoi(szNum);
				if (uiValue > 255)
					bError = 1;
				else
				{
					WriteDutyRatio(uiLeg, (unsigned char) uiValue);
					sprintf(szResponse, "OK");
				}
			}
		}
	}
	if (bError)
		strcpy(szResponse, "ERROR");

	return strlen(szResponse);
}


/*****************************
 * CmdCNT()
 * Handle the CNT command. Send a control application a command.
 * CNT=d,CMD
 * --> n: which control application to send the command to (number)
 * --> CMD: The command to send
 *szCmd: the command
 *szResponse: the response
 */
static int CmdCNT(const char * szCmd, char *szResponse, void *CommDevice)
{
	const char *szAppCMD = &szCmd[4];

	u32 uiApp;
	{
//get the control application to send to
		uiApp = atoi(szAppCMD);

		if ( uiApp >= 1) //|| (uiLeg > NUM_LEGS) )
		{
//get new duty ratio
			int i;
			for (i = 0; i < strlen(szCmd); i++)
				if (szCmd[i] == ',')
				{
					szAppCMD = &szCmd[i+1];
					break;
				}
			if (i != strlen(szCmd))
			{
				if (uiApp == 1)
					return ControlAppCommand(szAppCMD, szResponse, CommDevice);
				else if (uiApp == 2)
					return TorqueVSI_Command(szAppCMD, szResponse, CommDevice);
			}
		}
	}
	strcpy(szResponse, "ERROR");

	return strlen(szResponse);
}


/*****************************
 * CmdCTT()
 * Handle the CTT command. Start a control application with a command.
 * CTT=n,CMD
 * --> n: which control application to start (number)
 * --> CMD: The start command to send
 *szCmd: the command
 *szResponse: the response
 */
static int CmdCTT(const char * szCmd, char *szResponse, void *CommDevice)
{
	const char *szAppCMD = &szCmd[4];

	u32 uiApp;
	{
//get the control application to send to
		uiApp = atoi(szAppCMD);

		if ( uiApp >= 1) //|| (uiLeg > NUM_LEGS) )
		{
//get new duty ratio
			int i;
			for (i = 0; i < strlen(szCmd); i++)
				if (szCmd[i] == ',')
				{
					szAppCMD = &szCmd[i+1];
					break;
				}
			if (i != strlen(szCmd))
			{
				if (uiApp == 1)
					return ControlStopAppCommand(szAppCMD, szResponse, CommDevice);
				else if (uiApp == 2)
					return TorqueVSI_StopCommand(szAppCMD, szResponse, CommDevice);
			}
		}
	}
	strcpy(szResponse, "ERROR");

	return strlen(szResponse);
}

/*****************************
 * CmdCST()
 * Handle the CST command. Start a control application with a command.
 * CST=n,CMD
 * --> n: which control application to start (number)
 * --> CMD: The start command to send
 *szCmd: the command
 *szResponse: the response
 */
static int CmdCST(const char * szCmd, char *szResponse, void *CommDevice)
{
	const char *szAppCMD = &szCmd[4];

	u32 uiApp;
	{
//get the control application to send to
		uiApp = atoi(szAppCMD);

		if ( uiApp >= 1) //|| (uiLeg > NUM_LEGS) )
		{
//get new duty ratio
			int i;
			for (i = 0; i < strlen(szCmd); i++)
				if (szCmd[i] == ',')
				{
					szAppCMD = &szCmd[i+1];
					break;
				}
			if (i != strlen(szCmd))
			{
				if (uiApp == 1)
					return ControlStartAppCommand(szAppCMD, szResponse, CommDevice);
				else if (uiApp == 2)
					return TorqueVSI_StartCommand(szAppCMD, szResponse, CommDevice);
			}
		}
	}
	strcpy(szResponse, "ERROR");

	return strlen(szResponse);
}

/*****************************
 * CmdVSL()
 * Handle the VSL command.
 *
 * VSL=leg1,leg2,leg3
 *
 * leg1: number of leg 1 (1 to NUM_LEGS)
 * leg2: number of leg 2 (1 to NUM_LEGS)
 * leg3: number of leg 3 (1 to NUM_LEGS)
 *
 * Response: OK
 *
 *szCmd: the command
 *szResponse: the response
 */

static char bufferVSL[128];
static int CmdVSL(const char * szCmd, char *szResponse, void *CommDevice)
{
	char *p;
	int i = 0;

	// Create copy of cmd for parsing
	memset(bufferVSL, 0, 128);
	strcpy(bufferVSL, szCmd);

	// Parse out tokens
	int iLeg1 = 0;
	int iLeg2 = 0;
	int iLeg3 = 0;

	p = strtok(bufferVSL, "=,");
	while (p != NULL) {
		// Use current token...
		switch (i) {
		case 0:
			// Ignore 'VSL'
			break;
		case 1:
			iLeg1 = atoi(p);
			break;
		case 2:
			iLeg2 = atoi(p);
			break;
		case 3:
			iLeg3 = atoi(p);
			break;
		default:
			// This is an error!
			// Force error below
			iLeg1 = 0;
			iLeg2 = 1;
			iLeg3 = 1;
			break;
		}

		// Get next token
		p = strtok(NULL, "=,");
		i++;
	}

	// Check if all 0 => disable VSI
	if (iLeg1 == 0 && iLeg2 == 0 && iLeg3 == 0) {
		VSI_enabled = 0;
		strcat(szResponse, "OK");
		return strlen(szResponse);
	}

	// Check if errors in leg numbers
	int bError = 0;
	if (iLeg1 <= 0 || iLeg1 > NUM_LEGS) bError = 1;
	if (iLeg2 <= 0 || iLeg2 > NUM_LEGS) bError = 1;
	if (iLeg3 <= 0 || iLeg3 > NUM_LEGS) bError = 1;

	if (bError) {
		strcat(szResponse, "ERROR");
		return strlen(szResponse);
	}

	// Set legs for VSI
	VSI_leg1 = iLeg1;
	VSI_leg2 = iLeg2;
	VSI_leg3 = iLeg3;

	// Enable VSI
	VSI_enabled = 1;

	strcat(szResponse, "OK");
	return strlen(szResponse);
}


/*****************************
 * CmdVSI()
 * Handle the VSI command.
 *
 * VSI=voltage,freq(,ramp)
 *
 * voltage: peak voltage output percent of Vbus (0 to 100)
 * freq: frequency of output (Hz)
 * ramptime: (optional) duration of ramping (ms)
 *
 * Response: OK
 *
 *szCmd: the command
 *szResponse: the response
 */

static char bufferVSI[128];
static int CmdVSI(const char * szCmd, char *szResponse, void *CommDevice)
{
	char *p;
	int i = 0;

	// Create copy of cmd for parsing
	memset(bufferVSI, 0, 128);
	strcpy(bufferVSI, szCmd);

	// Parse out tokens
	int iVoltagePercent = 0;
	int iHz = 0;
	int iRamptime = 0;

	p = strtok(bufferVSI, "=,");
	while (p != NULL) {
		// Use current token...
		switch (i) {
		case 0:
			// Ignore 'VSI'
			break;
		case 1:
			iVoltagePercent = atoi(p);
			break;
		case 2:
			iHz = atoi(p);
			break;
		case 3:
			iRamptime = atoi(p);
			break;
		default:
			// This is an error!
			// Force error below
			iHz = 0;
			break;
		}

		// Get next token
		p = strtok(NULL, "=,");
		i++;
	}

	// Check for errors while parsing
//	if (iHz == 0 || iVoltagePercent == 0) {
//		strcat(szResponse, "ERROR");
//		return strlen(szResponse);
//	}

	// Convert voltage percent input to double percentage
	VSI_old_Vpercent = VSI_Vpercent;
	VSI_Vpercent = iVoltagePercent / 100.0;
	if (VSI_Vpercent > 100) VSI_Vpercent = 100.0;
	if (VSI_Vpercent < 0)   VSI_Vpercent = 0.0;


	// Convert freq input to rad/sec
	VSI_old_omega = VSI_omega;
	VSI_omega = 2 * PI * iHz;

	if (iRamptime > 0) {
		VSI_omega_ramp = abs(VSI_old_omega - VSI_omega) / (iRamptime / 1000.0); // (rad/sec) / sec
	} else {
		VSI_omega_ramp = 0;
	}


	if (VSI_omega != VSI_old_omega) {
		// There is delta freq requested
		VSI_R = (VSI_Vpercent - VSI_old_Vpercent) / (VSI_omega - VSI_old_omega);
		VSI_V0 = VSI_Vpercent - (VSI_R * VSI_omega);
	} else {
		// Instaneous voltage change
		VSI_R = 0;
		VSI_V0 = VSI_Vpercent;
	}

	strcat(szResponse, "OK");
	return strlen(szResponse);
}

static char bufferFSW[128];
static int CmdFSW(const char * szCmd, char *szResponse, void *CommDevice)
{
	char *p;
	int i = 0;

	// Create copy of cmd for parsing
	memset(bufferFSW, 0, 128);
	strcpy(bufferFSW, szCmd);

	// Parse out tokens
	int iHz = 0;

	p = strtok(bufferFSW, "=,");
	while (p != NULL) {
		// Use current token...
		switch (i) {
		case 0:
			// Ignore 'FSW'
			break;
		case 1:
			iHz = atoi(p);
			break;
		default:
			// This is an error!
			// Force error below
			iHz = 0;
			break;
		}

		// Get next token
		p = strtok(NULL, "=,");
		i++;
	}

	// Check for errors while parsing
	if (iHz < 100 || iHz > 4000000) {
		strcat(szResponse, "ERROR");
		return strlen(szResponse);
	}

	// do work
	pwm_set_switching_freq((double) iHz);

	strcat(szResponse, "OK");
	return strlen(szResponse);
}



static char bufferDEA[128];
static int CmdDEA(const char * szCmd, char *szResponse, void *CommDevice)
{
	char *p;
	int i = 0;

	// Create copy of cmd for parsing
	memset(bufferDEA, 0, 128);
	strcpy(bufferDEA, szCmd);

	// Parse out tokens
	int iNs = 0;

	p = strtok(bufferDEA, "=,");
	while (p != NULL) {
		// Use current token...
		switch (i) {
		case 0:
			// Ignore 'DEA'
			break;
		case 1:
			iNs = atoi(p);
			break;
		default:
			// This is an error!
			// Force error below
			iNs = 0;
			break;
		}

		// Get next token
		p = strtok(NULL, "=,");
		i++;
	}

	// Check for errors while parsing
	if (iNs < 25 || iNs > 1e9) {
		strcat(szResponse, "ERROR");
		return strlen(szResponse);
	}

	// do work
	pwm_set_deadtime_ns(iNs);

	strcat(szResponse, "OK");
	return strlen(szResponse);
}







//call this with the CR removed
//load the response into szResponse
int CommandParser(const char * szCmd, char *szResponse, void *CommDevice)
{
	char szTemp[256];
	int retVal = 0;
	//read ADC
	int i;
	char bFoundCmd = 0;
	szResponse[0] = 0; //put a leading null in to prevent any issues
	for (i = 0; i<NUM_CMD; i++)
		if (strstr(szCmd, command_table[i].szCmd) == szCmd)
		{
			bFoundCmd = 1;
			retVal = command_table[i].cmd_function(szCmd, szResponse, CommDevice);
			break;
		}
	if (!bFoundCmd)
	{
		if (strstr(szCmd, "?") == szCmd)
			for (i = 0; i < NUM_CMD; i++)
			{
				memset(szTemp, 0, sizeof(szTemp));
				sprintf(szTemp, "\r\n%s: %s", command_table[i].szCmd, command_table[i].szDescription);
				strcat(szResponse, szTemp);
			}
		else
			sprintf(szResponse, "Bad command: \"%s\"", szCmd);

		retVal = strlen(szResponse);
	}
	return retVal;
}



/*****************************
 * HandleCommands()
 * Call this function to handle incoming data
 *
 *
 *szRecvCmd: a pointer to memory that the function can use
 *state: the previously returned value (0 the first time the function is called)
 */
u32 HandleCommandsUart(char *szRcvCmd, u32 state)
{
	//static char szRcvCmd[4096];
	char szSendCmd[4096];
	//static char init = 0;
	char cRcvd[2];


	if (XUartPs_IsReceiveData(STDIN_BASEADDRESS))
	{
		cRcvd[0] = inbyte();
		cRcvd[1] = 0;
		print(cRcvd);

		if (cRcvd[0] == '\r')
		{
			int outLen = 0;
			memset (szSendCmd, 0, sizeof(szSendCmd));
			outLen = CommandParser(szRcvCmd, szSendCmd, 0);
			state = 0;

			//if sending string data
			/*if (outLen == strlen(szSendCmd))
				printf("\n%s\n", szSendCmd);
			//otherwise binary
			else
			{*/
				int i;
				print("\r\n");
				for (i = 0; i < outLen; i++)
					outbyte(szSendCmd[i]);
				print("\r\n");
			//}
		}
		else if (cRcvd[0] != '\n')	//ignore line feeds
		{
			szRcvCmd[state++] = cRcvd[0];
			szRcvCmd[state] = 0; //always null terminate

			//strcat(szRcvCmd, cRcvd);
		}
	}
	return state;
}

/*****************************
 * HandleCommandsGeneral()
 * Call this function to handle incoming data and get a response
 *
 *
 *szRecvCmd: a pointer to memory that the function can use
 *state: the previously returned value (0 the first time the function is called)
 */
u32 HandleCommandsGeneral(char *szOutData, char *szRcvCmd, char *szNewData, u32 NewDataLength, u32 *state, void *ComDevice)
{
	char *szSendCmd = szOutData;
	int i;
	char cRcvd[2];
	u32 outLen = 0;
	for (i = 0; i < NewDataLength; i++)
	{
		cRcvd[0] = *szNewData;
		cRcvd[1] = 0;
		szNewData++;

		*szSendCmd = cRcvd[0];
		outLen++;
		szSendCmd++;

		if (cRcvd[0] == '\r')
		{
			int newLen;
			//empty received data
			*state = 0;
			*szSendCmd = '\r';
			szSendCmd++;
			*szSendCmd = '\n';
			szSendCmd++;
			newLen = CommandParser(szRcvCmd, szSendCmd, ComDevice);
			if (newLen > 0)
			{
				szSendCmd[newLen++] = '\r';
				szSendCmd[newLen++] = '\n';
				outLen += newLen + 2;
			}
		}
		else if (cRcvd[0] != '\n')	//ignore line feeds
		{
			szRcvCmd[*state] = cRcvd[0];
			*state += 1;
			szRcvCmd[*state] = 0; //always null terminate

			//strcat(szRcvCmd, cRcvd);
		}
	}
	return outLen;
}
