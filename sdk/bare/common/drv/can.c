#include "drv/uart.h"
#include "sys/defines.h"
#include "xparameters.h"
#include "xcanps.h"
#include <stdio.h>

#define CAN0_DEVICE_ID XPAR_XCANPS_0_DEVICE_ID
#define CAN1_DEVICE_ID XPAR_XCANPS_1_DEVICE_ID

/*
 * Maximum CAN frame length in words.
 */
#define XCANPS_MAX_FRAME_SIZE_IN_WORDS (XCANPS_MAX_FRAME_SIZE / sizeof(u32))

#define FRAME_DATA_LENGTH 		8  /* Frame Data field length */

/*
 * Message Id Constant.
 */
#define MESSAGE_ID			2000

/*
 * The Baud Rate Prescaler Register (BRPR) and Bit Timing Register (BTR)
 * are setup such that CAN baud rate equals 40Kbps, assuming that the
 * the CAN clock is 24MHz. The user needs to modify these values based on
 * the desired baud rate and the CAN clock frequency. For more information
 * see the CAN 2.0A, CAN 2.0B, ISO 11898-1 specifications.
 */

/*
 * Timing parameters to be set in the Bit Timing Register (BTR).
 * These values are for a 40 Kbps baudrate assuming the CAN input clock
 frequency
 * is 24 MHz.
 */
#define TEST_BTR_SYNCJUMPWIDTH		3
#define TEST_BTR_SECOND_TIMESEGMENT	2
#define TEST_BTR_FIRST_TIMESEGMENT	15

/*
 * The Baud rate Prescalar value in the Baud Rate Prescaler Register (BRPR)
 * needs to be set based on the input clock  frequency to the CAN core and
 * the desired CAN baud rate.
 * This value is for a 40 Kbps baudrate assuming the CAN input clock frequency
 * is 24 MHz.
 */
#define TEST_BRPR_BAUD_PRESCALAR	29

static u32 TxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
static u32 RxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];

// Instance of the CAN Device
XCanPs CanPs;

// Set the mode of the CAN device
int can_setmode(uint32_t mode) {

	XCanPs *CanInstPtr = &CanPs;
	XCanPs_EnterMode(CanInstPtr, mode);
	while(XCanPs_GetMode(CanInstPtr) != mode);
	return SUCCESS;
}

// Set Baud Rate Prescalar Register (BRPR)
int can_setbaud(int rate) {
	int Status;
	XCanPs *CanInstPtr = &CanPs;
	// Initialize to default baud rate
	if (!rate)
		rate = TEST_BRPR_BAUD_PRESCALAR;
	Status = XCanPs_SetBaudRatePrescaler(CanInstPtr, rate);
	if (Status != XST_SUCCESS)
		return FAILURE;
	return SUCCESS;
}

// Set Bit Timing Register (BTR)
int can_set_btr(int jump_width, int first_time, int second_time) {
	int Status;
	XCanPs *CanInstPtr = &CanPs;
	// Initialize to defaults provided by Xilinx
	if (!jump_width && !first_time && !second_time)
		Status = XCanPs_SetBitTiming(CanInstPtr, TEST_BTR_SYNCJUMPWIDTH,
						TEST_BTR_SECOND_TIMESEGMENT,
						TEST_BTR_FIRST_TIMESEGMENT);
	else
		Status = XCanPs_SetBitTiming(CanInstPtr, jump_width,
						first_time,
						second_time);
	if (Status != XST_SUCCESS)
		return FAILURE;
	return SUCCESS;
}

// Initialize the CAN device, default settings
int can_init(int device_id)
{
	XCanPs *CanInstPtr = &CanPs;
	u16 DeviceId;
	if (device_id != 1 || device_id != 0)
		return FAILURE;
	else if(!device_id)
		DeviceId = CAN0_DEVICE_ID;
	else
		DeviceId = CAN0_DEVICE_ID;

	int Status;
	XCanPs_Config *Config;

	// Initialize the CAN driver so that it's ready to use
	// Look up the configuration in the config table, then initialize it
	Config = XCanPs_LookupConfig(DeviceId);
	if (NULL == Config) {
		return FAILURE;
	}

	Status = XCanPs_CfgInitialize(CanInstPtr, Config, Config->BaseAddr);
	if (Status != XST_SUCCESS) {
		return FAILURE;
	}

	// Run self-test on the device, which verifies basic sanity of the
	// device and the driver
	Status = XCanPs_SelfTest(CanInstPtr);
	if (Status != XST_SUCCESS) {
		return FAILURE;
	}

	// Enter Configuration Mode so that we can setup Baud Rate Precalar
	// Register (BRPR) and Bit Timing Register (BTR).
	can_setmode(XCANPS_MODE_CONFIG);

	// Set Baud Rate Prescalar Register (BRPR) and
	// Bit Timing Register (BTR)
	Status = can_setbaud(TEST_BRPR_BAUD_PRESCALAR);
	if (Status != XST_SUCCESS) {
		return FAILURE;
	}
	Status = can_set_btr(TEST_BTR_SYNCJUMPWIDTH, TEST_BTR_SECOND_TIMESEGMENT, TEST_BTR_FIRST_TIMESEGMENT);
	if (Status != XST_SUCCESS) {
		return FAILURE;
	}

	// Enter Normal Mode to use CAN peripheral via API calls
	can_setmode(XCANPS_MODE_NORMAL);

	return SUCCESS;
}

// Send a CAN packet
int can_send(uint8_t* packet, int num_bytes)
{
	u8 *FramePtr;
	int Index;
	int Status;
	XCanPs *CanInstPtr = &CanPs;

	// Create correct values for Identifier and Data Length Code Register
	TxFrame[0] = (u32)XCanPs_CreateIdValue((u32)MESSAGE_ID, 0, 0, 0, 0);
	TxFrame[1] = (u32)XCanPs_CreateDlcValue((u32)FRAME_DATA_LENGTH);

	// Populate the TX FIFO with CAN packet to send
	FramePtr = (u8 *)(&TxFrame[2]);
	for (Index = 0; Index < num_bytes; Index++) {
		*FramePtr++ = (u8)*packet;
		packet++;
	}


	// Wait until TX FIFO has room
	while (XCanPs_IsTxFifoFull(CanInstPtr) == TRUE);

	// Send the frame
	Status = XCanPs_Send(CanInstPtr, TxFrame);
	if (Status != XST_SUCCESS)
		return FAILURE;

	return SUCCESS;
}

// Print latest can packet received
int can_print()
{
	u8 *FramePtr;
	int Status;
	int Index;

	XCanPs *CanInstPtr = &CanPs;

	// Wait until a frame is received
	while (XCanPs_IsRxEmpty(CanInstPtr) == TRUE);

	// Receive a frame and verify its contents
	Status = XCanPs_Recv(CanInstPtr, RxFrame);

	if (Status == XST_SUCCESS) {
		print("Latest CAN packet is: ");
		FramePtr = (u8 *)(&RxFrame[2]);
		for (Index = 0; Index < FRAME_DATA_LENGTH; Index++) {
			print(*FramePtr);
			FramePtr++;
		}
		return SUCCESS;
	}
	return FAILURE;
}

// Check packet received in loopback mode is correct
static int can_checkpacket()
{
	u8 *FramePtr;
	int Status;
	int Index;

	XCanPs *CanInstPtr = &CanPs;

	// Wait until a frame is received
	while (XCanPs_IsRxEmpty(CanInstPtr) == TRUE);

	// Receive a frame and verify its contents
	Status = XCanPs_Recv(CanInstPtr, RxFrame);
	if (Status == XST_SUCCESS) {
		// Verify Identifier and Data Length Code
		if (RxFrame[0] !=
			(u32)XCanPs_CreateIdValue((u32)MESSAGE_ID, 0, 0, 0, 0))
			return XST_LOOPBACK_ERROR;

		if ((RxFrame[1] & ~XCANPS_DLCR_TIMESTAMP_MASK) != TxFrame[1])
			return XST_LOOPBACK_ERROR;

		// Verify Data field contents
		FramePtr = (u8 *)(&RxFrame[2]);
		for (Index = 0; Index < FRAME_DATA_LENGTH; Index++) {
			if (*FramePtr++ != (u8)Index) {
				return XST_LOOPBACK_ERROR;
			}
		}
	}

	return Status;
}

// Run a sanity check loopback test
int can_loopback_test()
{

	int Status;
	uint8_t packet[FRAME_DATA_LENGTH];

	// CAN Packet Populated
	int i;
	for (i = 0; i < FRAME_DATA_LENGTH; i++) {
		packet[i] = i;
	}

	Status = can_send(packet, FRAME_DATA_LENGTH);
	if (Status != XST_SUCCESS) {
		return FAILURE;
	}

	Status = can_checkpacket();
	if (Status != XST_SUCCESS)
		return FAILURE;
	return SUCCESS;

}
