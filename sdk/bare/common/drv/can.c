#include "drv/uart.h"
#include "gpio_mux.h"
#include "sys/defines.h"
#include "xcanps.h"
#include "xparameters.h"
#include <stdio.h>

#define CAN0_DEVICE_ID XPAR_XCANPS_0_DEVICE_ID
#define CAN1_DEVICE_ID XPAR_XCANPS_1_DEVICE_ID

// Maximum CAN frame length in words.
#define XCANPS_MAX_FRAME_SIZE_IN_WORDS (XCANPS_MAX_FRAME_SIZE / sizeof(u32))

// Default number of bytes of data sending
#define FRAME_DATA_LENGTH 8

// Message Id Constant
#define MESSAGE_ID 2000

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
#define DEFAULT_BTR_SYNCJUMPWIDTH 	   3
#define DEFAULT_BTR_SECOND_TIMESEGMENT 2
#define DEFAULT_BTR_FIRST_TIMESEGMENT  15

/*
 * The Baud rate Prescalar value in the Baud Rate Prescaler Register (BRPR)
 * needs to be set based on the input clock  frequency to the CAN core and
 * the desired CAN baud rate.
 * This value is for a 40 Kbps baudrate assuming the CAN input clock frequency
 * is 24 MHz.
 */
#define DEFAULT_BAUD_PRESCALAR 29

static u32 TxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
static u32 RxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];

// Two CAN peripherals
static XCanPs CanPs0;
static XCanPs CanPs1;

// Representing device we are currently on
static XCanPs *CanPs;

// Set the mode of the CAN device
int can_setmode(uint32_t mode)
{
	XCanPs *CanInstPtr = CanPs;
	uint32_t currMode = XCanPs_GetMode(CanInstPtr);
	if (currMode == XCANPS_MODE_LOOPBACK && mode != XCANPS_MODE_CONFIG) {
		print("\nCAN peripheral currently in loopback mode. Can only enter config mode from here.");
		return FAILURE;
	} else if (currMode == XCANPS_MODE_NORMAL && mode != XCANPS_MODE_SLEEP && mode != XCANPS_MODE_CONFIG){
		print("\nCAN peripheral currently in normal mode. Can only enter config or sleep mode from here.");
		return FAILURE;
	}

	XCanPs_EnterMode(CanInstPtr, mode);

	// Wait to reach specified mode, should happen instantaneously
	while(XCanPs_GetMode(CanInstPtr) != mode)
		;
	return SUCCESS;
}

// Set Baud Rate Prescalar Register (BRPR)
int can_setbaud(int rate)
{
	int Status;
	XCanPs *CanInstPtr = CanPs;

	// Ensure CAN peripheral in config mode
	if (XCanPs_GetMode(CanInstPtr) != XCANPS_MODE_CONFIG) {
		print("\nMust be in config mode to set baud rate prescalar register");
		return FAILURE;
	}

	// Initialize to default baud rate
	if (!rate)
		rate = DEFAULT_BAUD_PRESCALAR;
	Status = XCanPs_SetBaudRatePrescaler(CanInstPtr, rate);

	if (Status != XST_SUCCESS)
		return FAILURE;
	return SUCCESS;
}

// Set Bit Timing Register (BTR)
int can_set_btr(int jump_width, int first_time, int second_time)
{
	int Status;
	XCanPs *CanInstPtr = CanPs;

	// Ensure CAN peripheral in config mode
	if (XCanPs_GetMode(CanInstPtr) != XCANPS_MODE_CONFIG) {
		print("\nMust be in config mode to set bit timing register");
		return FAILURE;
	}


	// Initialize to defaults provided by Xilinx
	if (!jump_width && !first_time && !second_time)
		Status = XCanPs_SetBitTiming(CanInstPtr,
				   	   	   	   	   	 DEFAULT_BTR_SYNCJUMPWIDTH,
									 DEFAULT_BTR_SECOND_TIMESEGMENT,
									 DEFAULT_BTR_FIRST_TIMESEGMENT
									);
	else
		Status = XCanPs_SetBitTiming(CanInstPtr,
									 jump_width,
									 first_time,
									 second_time
									);

	if (Status != XST_SUCCESS)
		return FAILURE;
	return SUCCESS;
}

// Set CAN peripheral that we are using currently
int can_set_peripheral(int device_id)
{
	if (device_id != 1 && device_id != 0)
		return FAILURE;
	else if (device_id)
		CanPs = &CanPs1;
	else
		CanPs = &CanPs0;
	return SUCCESS;
}

// Initialize the CAN device, default settings
int can_init(int device_id)
{

	// Set GPIO Device and Port
	gpio_mux_set_device(GPIO_PORT1, GPIO_MUX_DEVICE1);

	XCanPs *CanInstPtr;
	u16 DeviceId;

	// Initialize the user specified CAN peripheral
	if (device_id != 1 && device_id != 0) {
		printf("device_id can only be 0 or 1\n");
		return FAILURE;
	}
	else if(!device_id) {
		DeviceId = CAN0_DEVICE_ID;
		CanInstPtr = &CanPs0;
		CanPs = &CanPs0;
		printf("CAN0:\tInitializing...\n");
	}
	else {
		DeviceId = CAN1_DEVICE_ID;
		CanInstPtr = &CanPs1;
		CanPs = &CanPs1;
		printf("CAN1:\tInitializing...\n");
	}

	int Status;
	XCanPs_Config *Config;

	// Initialize the CAN driver so that it's ready to use
	// Look up the configuration in the config table, then initialize it
	Config = XCanPs_LookupConfig(DeviceId);
	if (Config == NULL || CanInstPtr == NULL) {
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
	Status = can_setmode(XCANPS_MODE_CONFIG);
	if (Status != SUCCESS)
		return FAILURE;

	// Set Baud Rate Prescalar Register (BRPR) and
	// Bit Timing Register (BTR)
	Status = can_setbaud(DEFAULT_BAUD_PRESCALAR);
	if (Status != SUCCESS) {
		return FAILURE;
	}
	Status = can_set_btr(DEFAULT_BTR_SYNCJUMPWIDTH,
						 DEFAULT_BTR_SECOND_TIMESEGMENT,
						 DEFAULT_BTR_FIRST_TIMESEGMENT
						);

	if (Status != SUCCESS) {
		return FAILURE;
	}

	// Enter Normal Mode to use CAN peripheral
	return can_setmode(XCANPS_MODE_NORMAL);
}

// Send a CAN packet
int can_send(uint8_t* packet, int num_bytes)
{
	u8 *FramePtr;
	int Index;
	int Status;
	XCanPs *CanInstPtr = CanPs;

	// Check that pointer isn't NULL
	if (packet == NULL) {
		printf("Packet of data is null, please initialize it!\n");
		return FAILURE;
	}

	// Check number of bytes user is sending at once is between 1 to 8
	if (num_bytes <= 0 || num_bytes > 8) {
		printf("Can only send 8 bytes at once!\n");
		return FAILURE;
	}

	// Populate correct values for Identifier - check Zync 700 Reference Manual for meaning of this info
	TxFrame[0] = (u32)XCanPs_CreateIdValue((u32)MESSAGE_ID, 0, 0, 0, 0);

	// Specify number of bytes of data sending
	TxFrame[1] = (u32)XCanPs_CreateDlcValue((u32)num_bytes);

	// Populate the TX FIFO with CAN packet to send
	FramePtr = (u8 *)(&TxFrame[2]);
	for (Index = 0; Index < num_bytes; Index++) {
		*FramePtr++ = (u8)*packet;
		packet++;
	}

	// Check if TxFIFO is full
	if (XCanPs_IsTxFifoFull(CanInstPtr) == TRUE) {
		printf("CAN TxFIFO is full, try sending your data again in a bit!\n");
		return FAILURE;
	}

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

	XCanPs *CanInstPtr = CanPs;

	// Wait until a frame is received
	if (XCanPs_IsRxEmpty(CanInstPtr) == TRUE) {
		printf("Currently there isn't a packet in the RxFIFO, try again in a bit!\n");
		return FAILURE;
	}

	// Receive a frame and verify its contents
	Status = XCanPs_Recv(CanInstPtr, RxFrame);

	if (Status == XST_SUCCESS) {
		print("\nLatest CAN packet is: ");
		FramePtr = (u8 *)(&RxFrame[2]);
		for (Index = 0; Index < FRAME_DATA_LENGTH; Index++) {
			print(*FramePtr);
			FramePtr++;
		}
		return SUCCESS;
	}
	return FAILURE;
}

// Print mode the CAN peripheral is in, useful for debugging purposes
void can_print_mode()
{
	XCanPs *CanInstPtr = CanPs;

	uint32_t mode;
	mode = XCanPs_GetMode(CanInstPtr);
	if(mode == XCANPS_MODE_NORMAL)
		print("\nXCANPS_MODE_NORMAL");
	else if(mode == XCANPS_MODE_LOOPBACK)
		print("\nXCANPS_MODE_LOOPBACK");
	else if(mode == XCANPS_MODE_CONFIG)
			print("\nXCANPS_MODE_CONFIG");
	else if(mode == XCANPS_MODE_SLEEP)
			print("\nXCANPS_MODE_SLEEP");
	else
		print("\nOther mode");
}

// Print the CAN peripheral the user has set, useful for debugging purposes
void can_print_peripheral()
{
	if (CanPs == &CanPs0)
		print("\nCAN 0");
	else if (CanPs == &CanPs1)
		print("\nCAN 1");
	else
		print("\nCAN peripheral not set properly");
}

// Check packet received in loopback mode is correct
static int can_checkpacket()
{
	u8 *FramePtr;
	int Status;
	int Index;

	XCanPs *CanInstPtr = CanPs;

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

	// Check we are in loopback mode
	uint32_t mode;
	XCanPs *CanInstPtr = CanPs;
	mode = XCanPs_GetMode(CanInstPtr);
	if (mode != XCANPS_MODE_LOOPBACK) {
		print("\nNot in loopback mode, can't run self test");
		return FAILURE;
	}

	// Populate CAN packet
	int i;
	for (i = 0; i < FRAME_DATA_LENGTH; i++) {
		packet[i] = i;
	}

	// Send fake packet of data
	Status = can_send(packet, FRAME_DATA_LENGTH);
	if (Status != XST_SUCCESS) {
		return FAILURE;
	}

	// Check fake packet of data is received correctly
	Status = can_checkpacket();
	if (Status != XST_SUCCESS)
		return FAILURE;
	return SUCCESS;
}
