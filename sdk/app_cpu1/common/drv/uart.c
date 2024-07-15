#include "drv/uart.h"
#include "sys/defines.h"
#include "xparameters.h"
#include "xuartps.h"
#include <stdio.h>

#define UART_DEVICE_ID XPAR_XUARTPS_0_DEVICE_ID

#define TEST_BUFFER_SIZE (26)

// Instance of the UART Device
XUartPs UartPs;

// The following buffers are used in this example to send
// and receive data with the UART.
static u8 SendBuffer[TEST_BUFFER_SIZE]; // Buffer for Transmitting Data
static u8 RecvBuffer[TEST_BUFFER_SIZE]; // Buffer for Receiving Data

// The following counters are used to determine when the entire
// buffer has been sent and received.
volatile int TotalReceivedCount;
volatile int TotalSentCount;
int TotalErrorCount;

int uart_init(void)
{
    XUartPs *UartInstPtr = &UartPs;
    u16 DeviceId = UART_DEVICE_ID;

    u32 LoopCount = 0;
    unsigned int SentCount;
    unsigned int ReceivedCount;

    int Status;
    XUartPs_Config *Config;
    int Index;

    /*
     * Initialize the UART driver so that it's ready to use
     * Look up the configuration in the config table, then initialize it.
     */
    Config = XUartPs_LookupConfig(DeviceId);
    if (NULL == Config) {
        return FAILURE;
    }

    Status = XUartPs_CfgInitialize(UartInstPtr, Config, Config->BaseAddress);
    if (Status != XST_SUCCESS) {
        return FAILURE;
    }

    /* Check hardware build */
    Status = XUartPs_SelfTest(UartInstPtr);
    if (Status != XST_SUCCESS) {
        return FAILURE;
    }

    /* Use local loopback mode. */
    XUartPs_SetOperMode(UartInstPtr, XUARTPS_OPER_MODE_LOCAL_LOOP);

    /*
     * Initialize the send buffer bytes with a pattern and zero out
     * the receive buffer.
     */
    for (Index = 0; Index < TEST_BUFFER_SIZE; Index++) {
        SendBuffer[Index] = (Index % 26) + 'A';
        RecvBuffer[Index] = 0;
    }

    /* Block sending the buffer. */
    SentCount = XUartPs_Send(UartInstPtr, SendBuffer, TEST_BUFFER_SIZE);
    if (SentCount != TEST_BUFFER_SIZE) {
        return FAILURE;
    }

    /*
     * Wait while the UART is sending the data so that we are guaranteed
     * to get the data the 1st time we call receive, otherwise this function
     * may enter receive before the data has arrived
     */
    while (XUartPs_IsSending(UartInstPtr)) {
        LoopCount++;
    }

    /* Block receiving the buffer. */
    ReceivedCount = 0;
    while (ReceivedCount < TEST_BUFFER_SIZE) {
        ReceivedCount += XUartPs_Recv(UartInstPtr, &RecvBuffer[ReceivedCount], (TEST_BUFFER_SIZE - ReceivedCount));
    }

    /*
     * Check the receive buffer against the send buffer and verify the
     * data was correctly received
     */
    for (Index = 0; Index < TEST_BUFFER_SIZE; Index++) {
        if (SendBuffer[Index] != RecvBuffer[Index]) {
            return FAILURE;
        }
    }

    /* Restore to normal mode. */
    XUartPs_SetOperMode(UartInstPtr, XUARTPS_OPER_MODE_NORMAL);

    return SUCCESS;
}

int uart_send(char *msg, int len)
{
    return XUartPs_Send(&UartPs, (uint8_t *) msg, len);
}

int uart_recv(char *msg, int len)
{
    return XUartPs_Recv(&UartPs, (uint8_t *) msg, len);
}
