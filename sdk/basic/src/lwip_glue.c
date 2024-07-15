/*
 * lwip_glue.c
 *
 *  Created on: Jun 13, 2014
 *      Author: sever212
 */
#ifdef __arm__
#if 0
#include "xparameters.h"
#include "xparameters_ps.h"	/* defines XPAR values */
#include "xil_cache.h"
#include "xscugic.h"
#include "xscutimer.h"
#include "lwip/tcp.h"
#include "lwip/tcp_impl.h"
#include "xil_printf.h"
#include "platform_config.h"
#include "netif/xadapter.h"
#endif
#include "project_include.h"

#define RESET_RX_CNTR_LIMIT	400

static int ResetRxCntr = 0;
volatile char bEnableEthTimer = FALSE;
static struct netif server_netif;
struct netif *echo_netif;

char bEth250ms = FALSE;

typedef struct lwip_comm_glue {
	char szRcvCmdEth[4096];
	void *socket;
	u32 CmdHndlStateEth;
} lwip_comm_glue;


lwip_comm_glue EthComm[MAX_BASE_SOCKETS];

/*****************************
 * init_tcpip_comm_data()
 * Set up the communication data structures.
 * Call on power up.
 */
static void init_tcpip_comm_data()
{
	int i;
	for (i = 0; i < MAX_BASE_SOCKETS; i++)
	{
		EthComm[i].socket = 0;
		EthComm[i].CmdHndlStateEth = 0;
	}
}

/*****************************
 * get_tcpip_comm_data()
 * Get a pointer to the receive data and the receive state for
 * this socket.
 *
 * socket: unique pointer to the socket (doesn't need to point
 * 	to the socket, just needs to be unique to the socket)
 * data: returned pointer to the data
 * state: returned pointer to the state
 *
 * Returns a status code: NOT_FOUND or SUCCESS
 */
static int get_tcpip_comm_data(void *socket, char **data, u32 **state)
{
	int retVal = NOT_FOUND;
	int i;
	int protect = EnterProtection();
	for (i = 0; i < MAX_BASE_SOCKETS; i++)
	{
		if (EthComm[i].socket == socket)
		{
			*data = EthComm[i].szRcvCmdEth;
			*state = &EthComm[i].CmdHndlStateEth;
			retVal = SUCCESS;
			break;
		}
	}
	LeaveProtection(protect);
	return retVal;
}

/*****************************
 * put_tcpip_comm_data()
 * Call this upon the connection of a new socket to setup a data
 * structure for it.
 *
 * socket: unique pointer to the socket (doesn't need to point
 * 	to the socket, just needs to be unique to the socket)
 *
 * Returns a status code: OVERFLOW (if no space) or SUCCESS
 */
static int put_tcpip_comm_data(void *socket)
{
	int retVal = OVERFLOW;
	int i;
	int protect = EnterProtection();
	for (i = 0; i < MAX_BASE_SOCKETS; i++)
	{
		if ((EthComm[i].socket == 0) || (EthComm[i].socket == socket))
		{
			EthComm[i].socket = socket;
			EthComm[i].CmdHndlStateEth = 0;
			retVal = SUCCESS;
			break;
		}
	}
	LeaveProtection(protect);
	return retVal;
}

/*****************************
 * remove_tcpip_comm_data()
 * Call this upon the disconnection of a socket to free up
 * the memory used for its data structure
 *
 * socket: unique pointer to the socket (doesn't need to point
 * 	to the socket, just needs to be unique to the socket)
 *
 * Returns a status code: NOT_FOUND or SUCCESS
 */
static int remove_tcpip_comm_data(void *socket)
{
	int retVal = NOT_FOUND;
	int i;
	int protect = EnterProtection();
	for (i = 0; i < MAX_BASE_SOCKETS; i++)
	{
		if (EthComm[i].socket == socket)
		{
			EthComm[i].socket = 0;
			EthComm[i].CmdHndlStateEth = 0;
			retVal = SUCCESS;
			break;
		}
	}
	LeaveProtection(protect);
	return retVal;
}



void lwip_timer_callback()
{
	/* we need to call tcp_fasttmr & tcp_slowtmr at intervals specified
	 * by lwIP. It is not important that the timing is absoluetly accurate.
	 */
	static int odd = 1;

	odd = !odd;
#ifndef USE_SOFTETH_ON_ZYNQ
	ResetRxCntr++;
#endif
	tcp_fasttmr();
	if (odd) {
		tcp_slowtmr();
	}

	/* For providing an SW alternative for the SI #692601. Under heavy
	 * Rx traffic if at some point the Rx path becomes unresponsive, the
	 * following API call will ensures a SW reset of the Rx path. The
	 * API xemacpsif_resetrx_on_no_rxdata is called every 100 milliseconds.
	 * This ensures that if the above HW bug is hit, in the worst case,
	 * the Rx path cannot become unresponsive for more than 100
	 * milliseconds.
	 */
#ifndef USE_SOFTETH_ON_ZYNQ
	if (ResetRxCntr >= RESET_RX_CNTR_LIMIT) {
		xemacpsif_resetrx_on_no_rxdata(echo_netif);
		ResetRxCntr = 0;
	}
#endif
}


void
print_ip(char *msg, struct ip_addr *ip)
{
	print(msg);
	xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip),
			ip4_addr3(ip), ip4_addr4(ip));
}

void
print_ip_settings(struct ip_addr *ip, struct ip_addr *mask, struct ip_addr *gw)
{

	print_ip("Board IP: ", ip);
	print_ip("Netmask : ", mask);
	print_ip("Gateway : ", gw);
}


int SetupLWIP()
{
	struct ip_addr ipaddr, netmask, gw;

	/* the mac address of the board. this should be unique per board */
		unsigned char mac_ethernet_address[] =
		{ 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

		echo_netif = &server_netif;

	/* initliaze IP addresses to be used */
		IP4_ADDR(&ipaddr,  192, 168,   1, 10);
		IP4_ADDR(&netmask, 255, 255, 255,  0);
		IP4_ADDR(&gw,      192, 168,   1,  1);

		print_ip_settings(&ipaddr, &netmask, &gw);

		lwip_init();

	/* Add network interface to the netif_list, and set it as default */
		if (!xemac_add(echo_netif, &ipaddr, &netmask,
							&gw, mac_ethernet_address,
							PLATFORM_EMAC_BASEADDR)) {
			xil_printf("Error adding N/W interface\n\r");
			return -1;
		}
		netif_set_default(echo_netif);

		bEnableEthTimer = TRUE;

		/* specify that the network if is up */
		netif_set_up(echo_netif);

		/*Setup the application's comm data memory*/
		init_tcpip_comm_data();
	return XST_SUCCESS;
}

//for now lump all ethernet commands together:

void tcpip_disconnect_callback(void *arg, err_t err)
{
	remove_tcpip_comm_data(arg);
	//if we are streaming out this port, stop the streaming.
	if (StreamFIFOComm == arg)
		StreamFIFOComm = 0;
}

err_t recv_callback(void *arg, struct tcp_pcb *tpcb,
                               struct pbuf *p, err_t err)
{
	char szOutDataEth[8192];
	int status;
	char *szRcvCmdEth;
	u32 *CmdHndlStateEth;

	status = get_tcpip_comm_data(arg, &szRcvCmdEth, &CmdHndlStateEth);
	/* do not read the packet if we are not in ESTABLISHED state */
	/* Also, abort connection if unable to find the comm data*/
	if ((!p) || (status != SUCCESS)) {
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);
		remove_tcpip_comm_data(tpcb);
		return ERR_OK;
	}
	u32 outLen = HandleCommandsGeneral(szOutDataEth, szRcvCmdEth, p->payload, p->len, CmdHndlStateEth, arg);
	/* indicate that the packet has been received */
	tcp_recved(tpcb, p->len);


	/* echo back the payload */
	/* in this case, we assume that the payload is < TCP_SND_BUF */
	if (outLen > 0)
	{
		if (tcp_sndbuf(tpcb) > outLen) { //(tcp_sndbuf(tpcb) > p->len) {
			err = tcp_write(tpcb, szOutDataEth, outLen, 1);
		} else
			xil_printf("no space in tcp_sndbuf\n\r");
	}
	/* free the received pbuf */
	pbuf_free(p);

	return ERR_OK;
}

err_t sent_callback(void *arg, struct tcp_pcb *pcb, unsigned short len)
{
	//if streaming data, send data
	if (StreamFIFOComm == pcb)
	{
		if (ReloadStreamFIFO(pcb) > 0)
			bPrimeStreamFIFO = FALSE;
		else
			bPrimeStreamFIFO = TRUE;
	}
	return ERR_OK;
}

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	static int connection = 1;

	put_tcpip_comm_data(newpcb);

	/* set the receive callback for this connection */
	tcp_recv(newpcb, recv_callback);

	/* just use an integer number indicating the connection id as the
	   callback argument */
	tcp_arg(newpcb, /*(void*)connection*/ newpcb);

	//eric added:
	tcp_sent(newpcb, sent_callback);

	tcp_err(newpcb, tcpip_disconnect_callback);
	/* increment for subsequent accepted connections */
	connection++;

	return ERR_OK;
}


int start_tcpip()
{
	struct tcp_pcb *pcb;
	err_t err;
	unsigned port = 7;

	/* create new TCP PCB structure */
	pcb = tcp_new();
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}

	pcb->so_options |= SOF_KEEPALIVE; //ELS added to try to get sockets to detect a disconnect

	/* bind to specified @port */
	err = tcp_bind(pcb, IP_ADDR_ANY, port);
	if (err != ERR_OK) {
		xil_printf("Unable to bind to port %d: err = %d\n\r", port, err);
		return -2;
	}

	/* we do not need any arguments to callback functions */
	tcp_arg(pcb, NULL);

	/* listen for connections */
	pcb = tcp_listen(pcb);
	if (!pcb) {
		xil_printf("Out of memory while tcp_listen\n\r");
		return -3;
	}

	/* specify callback to use for incoming connections */
	tcp_accept(pcb, accept_callback);

	xil_printf("TCP echo server started @ port %d\n\r", port);

	return XST_SUCCESS;
}


#endif

