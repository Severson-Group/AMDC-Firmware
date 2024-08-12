/* 8/7/24: Created by Ryan Srichai
 *
 * Based off of https://github.com/FreeRTOS/FreeRTOS/blob/main/FreeRTOS-Plus/Demo/FreeRTOS_Plus_TCP_Echo_Posix/SimpleTCPEchoServer.c
 *
 *
 */

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include "socket_manager.h"
#include "ringbuf.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

/* The maximum time to wait for a closing socket to close. */
#define tcpechoSHUTDOWN_DELAY    (pdMS_TO_TICKS(5000))

/* If ipconfigUSE_TCP_WIN is 1 then the Tx sockets will use a buffer size set by
 * ipconfigTCP_TX_BUFFER_LENGTH, and the Tx window size will be
 * configECHO_SERVER_TX_WINDOW_SIZE times the buffer size.  Note
 * ipconfigTCP_TX_BUFFER_LENGTH is set in FreeRTOSIPConfig.h as it is a standard TCP/IP
 * stack constant, whereas configECHO_SERVER_TX_WINDOW_SIZE is set in
 * FreeRTOSConfig.h as it is a demo application constant. */
#ifndef configECHO_SERVER_TX_WINDOW_SIZE
	#define configECHO_SERVER_TX_WINDOW_SIZE    2
#endif

/* If ipconfigUSE_TCP_WIN is 1 then the Rx sockets will use a buffer size set by
 * ipconfigTCP_RX_BUFFER_LENGTH, and the Rx window size will be
 * configECHO_SERVER_RX_WINDOW_SIZE times the buffer size.  Note
 * ipconfigTCP_RX_BUFFER_LENGTH is set in FreeRTOSIPConfig.h as it is a standard TCP/IP
 * stack constant, whereas configECHO_SERVER_RX_WINDOW_SIZE is set in
 * FreeRTOSConfig.h as it is a demo application constant. */
#ifndef configECHO_SERVER_RX_WINDOW_SIZE
	#define configECHO_SERVER_RX_WINDOW_SIZE    2
#endif

/*-----------------------------------------------------------*/

/*
 * Uses FreeRTOS+TCP to listen for incoming echo connections, creating a task
 * to handle each connection.
 */
    static void prvConnectionListeningTask(void *pvParameters);

/*
 * Created by the connection listening task to handle a single connection.
 */
    static void prvServerConnectionInstance(void *pvParameters);

/*-----------------------------------------------------------*/

/* Stores the stack size passed into vStartSimpleTCPServerTasks() so it can be
 * reused when the server listening task creates tasks to handle connections. */
    static uint16_t usUsedStackSize = 0;

/* Create task stack and buffers for use in the Listening and Server connection tasks */
    static StaticTask_t listenerTaskBuffer;
    static StackType_t listenerTaskStack[configMINIMAL_STACK_SIZE];

    static StaticTask_t echoServerTaskBuffer;
    static StackType_t echoServerTaskStack[configMINIMAL_STACK_SIZE];

/*-----------------------------------------------------------*/

/* needed for FreeRTOS-Plus-TCP */

static UBaseType_t ulNextRand;

UBaseType_t uxRand(void) {
	const uint32_t ulMultiplier = 0x015a4e35UL, ulIncrement = 1UL;
	/* Utility function to generate a pseudo random number. */
	ulNextRand = (ulMultiplier * ulNextRand) + ulIncrement;
	return((int) (ulNextRand) & 0x7fffUL );
}

BaseType_t xApplicationGetRandomNumber(uint32_t *pulNumber) {
	*pulNumber = (uint32_t) uxRand();
	return pdTRUE;
}

uint32_t ulApplicationGetNextSequenceNumber(uint32_t ulSourceAddress, uint16_t usSourcePort, uint32_t ulDestinationAddress, uint16_t usDestinationPort) {
	return ( uint32_t ) uxRand();
}

void vApplicationIPNetworkEventHook(eIPCallbackEvent_t eNetworkEvent) {
	static BaseType_t xTasksAlreadyCreated = pdFALSE;
	/* If the network has just come up...*/
	if ((eNetworkEvent == eNetworkUp) && (xTasksAlreadyCreated == pdFALSE)) {
		/* Do nothing. Just a stub. */
		xTasksAlreadyCreated = pdTRUE;
	}
}

/* Default MAC address configuration.  The demo creates a virtual network
 * connection that uses this MAC address by accessing the raw Ethernet data
 * to and from a real network connection on the host PC.  See the
 * configNETWORK_INTERFACE_TO_USE definition for information on how to configure
 * the real network connection to use. */
const uint8_t ucMACAddress[6] =
{
	configMAC_ADDR0,
	configMAC_ADDR1,
	configMAC_ADDR2,
	configMAC_ADDR3,
	configMAC_ADDR4,
	configMAC_ADDR5
};

/* The default IP and MAC address used by the code. It is used as a place holder. */
static const uint8_t ucIPAddress[4] = {
	configIP_ADDR0,
	configIP_ADDR1,
	configIP_ADDR2,
	configIP_ADDR3
};
static const uint8_t ucNetMask[4] = {
	configNET_MASK0,
	configNET_MASK1,
	configNET_MASK2,
	configNET_MASK3
};
static const uint8_t ucGatewayAddress[4] = {
	configGATEWAY_ADDR0,
	configGATEWAY_ADDR1,
	configGATEWAY_ADDR2,
	configGATEWAY_ADDR3
};
static const uint8_t ucDNSServerAddress[4] = {
	configDNS_SERVER_ADDR0,
	configDNS_SERVER_ADDR1,
	configDNS_SERVER_ADDR2,
	configDNS_SERVER_ADDR3
};

void start_tcp(uint16_t usStackSize, UBaseType_t uxPriority) {
	socket_manager_init();

	xil_printf("Ethernet Init with address: %d.%d.%d.%d\n", ucIPAddress[0], ucIPAddress[1], ucIPAddress[2], ucIPAddress[3]);
	FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);

	/* Create the TCP echo server. */
	xTaskCreateStatic(prvConnectionListeningTask, "ServerListener", configMINIMAL_STACK_SIZE, NULL, uxPriority + 1, listenerTaskStack, &listenerTaskBuffer);

	/* Remember the requested stack size so it can be re-used by the server
	 * listening task when it creates tasks to handle connections. */
	usUsedStackSize = usStackSize;
}
/*-----------------------------------------------------------*/

    static void prvConnectionListeningTask(void * pvParameters)
    {
        struct freertos_sockaddr xClient, xBindAddress;
        Socket_t xListeningSocket, xConnectedSocket;
        socklen_t xSize = sizeof(xClient);
        static const TickType_t xReceiveTimeOut = portMAX_DELAY;
        const BaseType_t xBacklog = 20;

        #if (ipconfigUSE_TCP_WIN == 1)
            WinProperties_t xWinProps;

            /* Fill in the buffer and window sizes that will be used by the socket. */
            xWinProps.lTxBufSize = ipconfigTCP_TX_BUFFER_LENGTH;
            xWinProps.lTxWinSize = configECHO_SERVER_TX_WINDOW_SIZE;
            xWinProps.lRxBufSize = ipconfigTCP_RX_BUFFER_LENGTH;
            xWinProps.lRxWinSize = configECHO_SERVER_RX_WINDOW_SIZE;
        #endif /* ipconfigUSE_TCP_WIN */

        /* Just to prevent compiler warnings. */
        (void) pvParameters;

        /* Attempt to open the socket. */
        xListeningSocket = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP);
        configASSERT(xListeningSocket != FREERTOS_INVALID_SOCKET);

        /* Set a time out so accept() will just wait for a connection. */
        FreeRTOS_setsockopt(xListeningSocket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof(xReceiveTimeOut));

        /* Set the window and buffer sizes. */
        #if (ipconfigUSE_TCP_WIN == 1)
            FreeRTOS_setsockopt(xListeningSocket, 0, FREERTOS_SO_WIN_PROPERTIES, (void *) &xWinProps, sizeof(xWinProps));
        #endif /* ipconfigUSE_TCP_WIN */

        /* Bind the socket to the port that the client task will send to, then
         * listen for incoming connections. */
        xBindAddress.sin_port = configIP_PORT;
        xBindAddress.sin_port = FreeRTOS_htons(xBindAddress.sin_port);
        xBindAddress.sin_family = FREERTOS_AF_INET;
        FreeRTOS_bind( xListeningSocket, &xBindAddress, sizeof(xBindAddress));
        FreeRTOS_listen(xListeningSocket, xBacklog);

        for(;;) {
            /* Wait for a client to connect. */
            xConnectedSocket = FreeRTOS_accept(xListeningSocket, &xClient, &xSize);
            configASSERT(xConnectedSocket != FREERTOS_INVALID_SOCKET);

            /* Spawn a task to handle the connection. */
            xTaskCreateStatic(prvServerConnectionInstance, "ip", configMINIMAL_STACK_SIZE, (void *) xConnectedSocket, tskIDLE_PRIORITY, echoServerTaskStack, &echoServerTaskBuffer);
        }
    }
/*-----------------------------------------------------------*/

    static void prvServerConnectionInstance(void * pvParameters)
    {
        int32_t recvBytes;
        Socket_t xConnectedSocket;
        static const TickType_t xReceiveTimeOut = pdMS_TO_TICKS(5000);
        static const TickType_t xSendTimeOut = pdMS_TO_TICKS(5000);
        TickType_t xTimeOnShutdown;
        uint8_t *rxBuffer;

        xConnectedSocket = (Socket_t) pvParameters;

        /* add to socket manager */
        socket_manager_put(xConnectedSocket);

        /* Create receive buffer */
        rxBuffer = (uint8_t *) pvPortMalloc(ipconfigTCP_MSS);

        if (rxBuffer != NULL) {
            FreeRTOS_setsockopt(xConnectedSocket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof(xReceiveTimeOut));
            FreeRTOS_setsockopt(xConnectedSocket, 0, FREERTOS_SO_SNDTIMEO, &xSendTimeOut, sizeof(xReceiveTimeOut));
            for (;;) {
            	// Do not read the packet if we are not in ESTABLISHED state
				// Also, abort connection if socket has never been registered
				if (!socket_manager_is_registered(xConnectedSocket)) {
					socket_manager_remove(xConnectedSocket);
					break; // abort socket
				}
				memset(rxBuffer, 0x00, ipconfigTCP_MSS);
				recvBytes = FreeRTOS_recv(xConnectedSocket, rxBuffer, ipconfigTCP_MSS, 0);
				if (recvBytes <= 0) {
					break;
				}
				socket_manager_rx_data(xConnectedSocket, rxBuffer, recvBytes);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10.0)); // keep socket open for 10 ms to allow response (without going out of scope)

        socket_manager_remove(xConnectedSocket);

        /* Initiate a shutdown in case it has not already been initiated. */
        FreeRTOS_shutdown(xConnectedSocket, FREERTOS_SHUT_RDWR);

        /* Wait for the shutdown to take effect, indicated by FreeRTOS_recv() returning an error. */
        xTimeOnShutdown = xTaskGetTickCount();

        do {
            if (FreeRTOS_recv(xConnectedSocket, rxBuffer, ipconfigTCP_MSS, 0) < 0) {
                break;
            }
        } while ((xTaskGetTickCount() - xTimeOnShutdown) < tcpechoSHUTDOWN_DELAY);

        /* Finished with the socket, buffer, the task. */
        vPortFree(rxBuffer);
        FreeRTOS_closesocket(xConnectedSocket);

        vTaskDelete(NULL);
    }

