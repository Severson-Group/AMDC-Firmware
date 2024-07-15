/*
 * io_pecb.c
 *
 *  Created on: Jun 25, 2014
 *      Author: sever212
 */

#include "project_include.h"

volatile u32 *PECB;
volatile u32 *LEDs;

/*****************************
 * ReadADC()
 * Call this function to read in the ADC voltage
 *
 * reads +-10V, accurate to 0.0049V
 *uiADCnum: the ADC to use (between 1 and 12)
 */
double ReadADC(u32 uiADCnum)
{
	float v;
	analog_getf(uiADCnum, &v);
	return v;

//	signed int iADC = 0;
//	double dbADCval = 0, dbTemp;
//
//	if ( (uiADCnum > 0) && (uiADCnum < 13) )
//		iADC = PECB[uiADCnum-1];
//	dbTemp = iADC;
//	dbADCval = (dbTemp / 0x7FF) * 10;
//	return dbADCval;
}

/*void WriteDutyFraction(u32 Leg, float frac)
{
	unsigned char value = (unsigned char) (frac*256);
	PECB[Leg-1] = value;
}*/

/*****************************
 * WriteDutyRatio()
 * Call this function to write a duty ratio out to
 * a single leg. The value should be between 0 and 255
 * as where 255 is always on and 0 is always off
 *
 * Leg: 1 - ??
 * value: fractional duty ratio between 0 and 255
 *
 */
void WriteDutyRatio(u32 Leg, unsigned char value)
{
	pwm_set_duty_raw(Leg-1, value);

//	PECB[Leg-1] = value;

}

/*****************************
 * WriteConfigRegister()
 * Call this function to write a setting to the PECB config
 * register.
 *
 * value: value between 0 and 255
 *
 */
void WriteConfigRegister(u8 value)
{
	PECB[NUM_LEGS] = value;
}

/*****************************
 * InitPECB_IO()
 * Call this function to start setting up the
 * PECB IO. After approximately 0.5 seconds have
 * passed, call FinishInitPECB_IO() to finish
 * the initialization
 *
 */
int InitPECB_IO()
{
	u32 i;
	LEDs = (u32 *) 0x41200000;
	PECB = (u32 *) 0x43c00000;

	//turn all of the PECB legs on.
	//this is to get the leg's dc/dc converters up and running properly
	//in accordance with the email from sever212@umn.edu on Dec. 10, 2014
	//will turn all the legs off later, in FinishInitPECB_IO()
	for (i = 0; i < NUM_LEGS; i++)
		WriteDutyRatio(i+1, 255);

	return XST_SUCCESS;
}

/*****************************
 * FinishInitPECB_IO()
 * Call this function approximately 0.5 seconds
 * after calling InitPECB_IO() to finish the
 * initialization.
 *
 */
int FinishInitPECB_IO()
{
	u32 i;
	//turn all of the legs off
	for (i = 0; i < NUM_LEGS; i++)
			WriteDutyRatio(i+1, 0);
}

void WriteToZB_LEDs(u32 LED_value)
{
	*LEDs = LED_value;
}
