#include "can.h"
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

static u32 TxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
static u32 RxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];

// Two CAN peripherals
static XCanPs CanPs0;
static XCanPs CanPs1;

// Representing device we are currently on
static XCanPs *CanPs;

// Set the mode of the CAN device
int can_setmode(can_mode_t mode)
{
    XCanPs *CanInstPtr = CanPs;
    uint32_t currMode = XCanPs_GetMode(CanInstPtr);
    if (currMode == XCANPS_MODE_LOOPBACK && mode != CAN_CONFIG) {
#ifdef CAN_DEBUG
        print("\nCAN peripheral currently in loopback mode. Can only enter config mode from here.");
#endif
        return FAILURE;
    } else if (currMode == XCANPS_MODE_NORMAL && mode != CAN_SLEEP && mode != CAN_CONFIG) {
#ifdef CAN_DEBUG
        print("\nCAN peripheral currently in normal mode. Can only enter config or sleep mode from here.");
#endif
        return FAILURE;
    }

    XCanPs_EnterMode(CanInstPtr, mode);

    // Wait to reach specified mode, should happen instantaneously
    while (XCanPs_GetMode(CanInstPtr) != mode)
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
#ifdef CAN_DEBUG
        print("\nMust be in config mode to set baud rate prescalar register");
#endif
        return FAILURE;
    }

    Status = XCanPs_SetBaudRatePrescaler(CanInstPtr, rate);

    if (Status != XST_SUCCESS)
        return FAILURE;
    return SUCCESS;
}

// Set Bit Timing Register (BTR)
int can_set_btr(uint8_t sjw, uint8_t ts2, uint8_t ts1)
{
    int Status;
    XCanPs *CanInstPtr = CanPs;

    // Ensure CAN peripheral in config mode
    if (XCanPs_GetMode(CanInstPtr) != XCANPS_MODE_CONFIG) {
#ifdef CAN_DEBUG
        print("\nMust be in config mode to set bit timing register");
#endif
        return FAILURE;
    }

    Status = XCanPs_SetBitTiming(CanInstPtr, sjw, ts2, ts1);

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
#ifdef CAN_DEBUG
        printf("device_id can only be 0 or 1\n");
#endif
        return FAILURE;
    } else if (!device_id) {
        DeviceId = CAN0_DEVICE_ID;
        CanInstPtr = &CanPs0;
        CanPs = &CanPs0;
        printf("CAN0:\tInitializing...\n");
    } else {
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
    Status = can_set_btr(DEFAULT_BTR_SYNCJUMPWIDTH, DEFAULT_BTR_SECOND_TIMESEGMENT, DEFAULT_BTR_FIRST_TIMESEGMENT);

    if (Status != SUCCESS) {
        return FAILURE;
    }

    // Enter Normal Mode to use CAN peripheral
    return can_setmode(XCANPS_MODE_NORMAL);
}

// Send a CAN packet
int can_send(can_packet_t *packet)
{
    u8 *FramePtr;
    int i;
    int Status;
    XCanPs *CanInstPtr = CanPs;

    // Check that pointer isn't NULL
    if (packet == NULL) {
#ifdef CAN_DEBUG
        printf("Packet of data is null, please initialize it!\n");
#endif
        return FAILURE;
    }

    // Check number of bytes user is sending at once is between 1 to 8
    if (num_bytes <= 0 || num_bytes > 8) {
#ifdef CAN_DEBUG
        printf("Can only send 8 bytes at once!\n");
#endif
        return FAILURE;
    }

    // Populate correct values for Identifier - check Zync 700 Reference Manual for meaning of this info
    TxFrame[0] = (u32) XCanPs_CreateIdValue((u32) packet->message_id, 0, 0, 0, 0);

    // Specify number of bytes of data sending
    TxFrame[1] = (u32) XCanPs_CreateDlcValue((u32) packet->num_bytes);

    // Populate the TX FIFO with CAN packet to send
    FramePtr = (u8 *) (&TxFrame[2]);
    for (i = 0; i < packet->num_bytes; i++) {
        *FramePtr++ = (u8) packet->buffer[i];
    }

    // Check if TxFIFO is full
    if (XCanPs_IsTxFifoFull(CanInstPtr) == TRUE) {
#ifdef CAN_DEBUG
        printf("CAN TxFIFO is full, try sending your data again in a bit!\n");
#endif
        return FAILURE;
    }

    // Send the frame
    Status = XCanPs_Send(CanInstPtr, TxFrame);

    if (Status != XST_SUCCESS)
        return FAILURE;
    return SUCCESS;
}

// Print latest can packet received
int can_rcv(can_packet_t *packet)
{
    u8 *FramePtr;
    int Status;
    int i;

    XCanPs *CanInstPtr = CanPs;

    // Check if a frame is empty
    if (XCanPs_IsRxEmpty(CanInstPtr) == TRUE) {
#ifdef CAN_DEBUG
        printf("Currently there isn't a packet in the RxFIFO, try again in a bit!\n");
#endif
        return FAILURE;
    }

    // Receive a frame and verify its contents
    Status = XCanPs_Recv(CanInstPtr, RxFrame);

    if (Status == XST_SUCCESS) {
        packet->message_id = (int) (RxFrame[0] >> 21);
        packet->num_bytes = (int) (RxFrame[1] >> 28);
        FramePtr = (u8 *) (&RxFrame[2]);
        for (i = 0; i < packet->num_bytes; i++) {
            packet->buffer[i] = *FramePtr;
            FramePtr++;
        }
        return SUCCESS;
    }
    return FAILURE;
}

// Print mode the CAN peripheral is in, useful for debugging purposes
void can_print_mode()
{
#ifdef CAN_DEBUG
    XCanPs *CanInstPtr = CanPs;

    uint32_t mode;
    mode = XCanPs_GetMode(CanInstPtr);
    if (mode == XCANPS_MODE_NORMAL)
        print("\nXCANPS_MODE_NORMAL");
    else if (mode == XCANPS_MODE_LOOPBACK)
        print("\nXCANPS_MODE_LOOPBACK");
    else if (mode == XCANPS_MODE_CONFIG)
        print("\nXCANPS_MODE_CONFIG");
    else if (mode == XCANPS_MODE_SLEEP)
        print("\nXCANPS_MODE_SLEEP");
    else
        print("\nOther mode");
#endif
}

// Print the CAN peripheral the user has set, useful for debugging purposes
void can_print_peripheral()
{
#ifdef CAN_DEBUG
    if (CanPs == &CanPs0)
        print("\nCAN 0");
    else if (CanPs == &CanPs1)
        print("\nCAN 1");
    else
        print("\nCAN peripheral not set properly");
#endif
}

// Check packet received in loopback mode is correct
static int can_checkpacket()
{
    u8 *FramePtr;
    int Status;
    int Index;

    XCanPs *CanInstPtr = CanPs;

    // Wait until a frame is received
    while (XCanPs_IsRxEmpty(CanInstPtr) == TRUE)
        ;

    // Receive a frame and verify its contents
    Status = XCanPs_Recv(CanInstPtr, RxFrame);
    if (Status == XST_SUCCESS) {
        // Verify Identifier and Data Length Code
        if (RxFrame[0] != (u32) XCanPs_CreateIdValue((u32) MESSAGE_ID, 0, 0, 0, 0))
            return XST_LOOPBACK_ERROR;

        if ((RxFrame[1] & ~XCANPS_DLCR_TIMESTAMP_MASK) != TxFrame[1])
            return XST_LOOPBACK_ERROR;

        // Verify Data field contents
        FramePtr = (u8 *) (&RxFrame[2]);
        for (Index = 0; Index < FRAME_DATA_LENGTH; Index++) {
            if (*FramePtr++ != (u8) Index) {
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
#ifdef CAN_DEBUG
        print("\nNot in loopback mode, can't run self test");
#endif
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
