#include "socket_manager.h"
#include "xstatus.h"
#include <stdint.h>

// Communications
#define MAX_BASE_SOCKETS     (5)
#define MAX_SEND_PACKET_SIZE (2*8192)

typedef enum socket_type_e {
	MY_SOCKET_TYPE_
} my_socket_type_e;

typedef struct my_lwip_socket {
	void *socket;
	char szRcvCmdEth[4096];
	uint32_t CmdHndlStateEth;
} my_lwip_socket_t;

my_lwip_socket_t EthComm[MAX_BASE_SOCKETS];

// These protection functions come from Eric's "basic" app where the ScuTimer
// was configured to call ISR at 100 kHz. This was driving the whole
// application.
//
// In this code, we are running Ethernet on a different core, so there
// is no high priority timer doing anything... Therefore, we can just
// do nothing when we want to enter/leave protected code...
//
//char TimerDisabled = TRUE;
int EnterProtection() {
//	if (!TimerDisabled)
//	{
//		XScuGic_Disable(&IntcInstance, TIMER_IRPT_INTR);
//		TimerDisabled = TRUE;
//		return TRUE;
//	}
//	else
//		return FALSE;
	return FALSE;
}

int LeaveProtection(int State) {
//	if (State)
//	{
//		XScuGic_Enable(&IntcInstance, TIMER_IRPT_INTR);
//		TimerDisabled = FALSE;
//	}
//	return State;
	return FALSE;
}


// socket_manager_init()
//
// Set up the communication data structures.
// Call on power up.
//
void socket_manager_init(void)
{
	int i;
	for (i = 0; i < MAX_BASE_SOCKETS; i++)
	{
		EthComm[i].socket = 0;
		EthComm[i].CmdHndlStateEth = 0;
	}
}

// socket_manager_get()
//
// Get a pointer to the receive data and the receive state for
// this socket.
//
// socket: unique pointer to the socket (doesn't need to point
// 	to the socket, just needs to be unique to the socket)
// data: returned pointer to the data
// state: returned pointer to the state
//
// Returns a status code: NOT_FOUND or SUCCESS
//
int socket_manager_get(void *socket, char **data, uint32_t **state)
{
	int retVal = XST_DEVICE_NOT_FOUND;
	int i;
	int protect = EnterProtection();
	for (i = 0; i < MAX_BASE_SOCKETS; i++)
	{
		if (EthComm[i].socket == socket)
		{
			*data = EthComm[i].szRcvCmdEth;
			*state = &EthComm[i].CmdHndlStateEth;
			retVal = XST_SUCCESS;
			break;
		}
	}
	LeaveProtection(protect);
	return retVal;
}

// socket_manager_put()
//
// Call this upon the connection of a new socket to setup a data
// structure for it.
//
// socket: unique pointer to the socket (doesn't need to point
// 	to the socket, just needs to be unique to the socket)
//
// Returns a status code: OVERFLOW (if no space) or SUCCESS
//
int socket_manager_put(void *socket)
{
	int retVal = XST_FAILURE; // indicate overflow
	int i;
	int protect = EnterProtection();
	for (i = 0; i < MAX_BASE_SOCKETS; i++)
	{
		if ((EthComm[i].socket == 0) || (EthComm[i].socket == socket))
		{
			EthComm[i].socket = socket;
			EthComm[i].CmdHndlStateEth = 0;
			retVal = XST_SUCCESS;
			break;
		}
	}
	LeaveProtection(protect);
	return retVal;
}

// socket_manager_remove()
//
// Call this upon the disconnection of a socket to free up
// the memory used for its data structure
//
// socket: unique pointer to the socket (doesn't need to point
// 	to the socket, just needs to be unique to the socket)
//
// Returns a status code: NOT_FOUND or SUCCESS
//
int socket_manager_remove(void *socket)
{
	int retVal = XST_DEVICE_NOT_FOUND;
	int i;
	int protect = EnterProtection();
	for (i = 0; i < MAX_BASE_SOCKETS; i++)
	{
		if (EthComm[i].socket == socket)
		{
			EthComm[i].socket = 0;
			EthComm[i].CmdHndlStateEth = 0;
			retVal = XST_SUCCESS;
			break;
		}
	}
	LeaveProtection(protect);
	return retVal;
}
