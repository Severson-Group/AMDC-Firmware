#include "lwip_glue.h"
#include "lwip/tcp.h"
#include "netif/xadapter.h"
#include "platform.h"
#include "platform_config.h"
#include "socket_manager.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xstatus.h"

// Missing declaration in lwIP of core init function
void lwip_init();

static struct netif server_netif;
struct netif *my_netif;

static void print_ip(char *msg, ip_addr_t *ip)
{
    print(msg);
    xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip), ip4_addr3(ip), ip4_addr4(ip));
}

static void print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{
    print_ip("Board IP: ", ip);
    print_ip("Netmask : ", mask);
    print_ip("Gateway : ", gw);
}

int setup_lwip(void)
{
    ip_addr_t ipaddr, netmask, gw;

    // The mac address of the board. This should be unique per board
    //
    // => Leave as default from Xilinx example app
    unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

    my_netif = &server_netif;

    // Initial IP addresses to be used
    IP4_ADDR(&ipaddr, 192, 168, 1, 10);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 1, 1);

    // Print local IP settings to UART
    print_ip_settings(&ipaddr, &netmask, &gw);

    // Core LwIP init
    lwip_init();

    // Add network interface to the netif_list, and set it as default
    if (!xemac_add(my_netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
        xil_printf("Error adding N/W interface\n\r");
        return XST_FAILURE;
    }

    netif_set_default(my_netif);

    // Now enable interrupts
    platform_enable_interrupts();

    // Specify that the network if is up
    netif_set_up(my_netif);

    // Setup the socket manager's data structs
    socket_manager_init();

    return XST_SUCCESS;
}

void tcpip_disconnect_callback(void *arg, err_t err)
{
    socket_manager_remove(arg);
}

err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    // Do not read the packet if we are not in ESTABLISHED state
    // Also, abort connection if socket has never been registered
    if ((!p) || (!socket_manager_is_registered(arg))) {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        socket_manager_remove(tpcb);
        return ERR_OK;
    }

    socket_manager_rx_data(arg, p->payload, p->len);

    // Indicate that the packet has been received
    tcp_recved(tpcb, p->len);

    // Free the received pbuf
    pbuf_free(p);

    return ERR_OK;
}

err_t sent_callback(void *arg, struct tcp_pcb *pcb, unsigned short len)
{
    //	//if streaming data, send data
    //	if (StreamFIFOComm == pcb)
    //	{
    //		if (ReloadStreamFIFO(pcb) > 0)
    //			bPrimeStreamFIFO = FALSE;
    //		else
    //			bPrimeStreamFIFO = TRUE;
    //	}
    return ERR_OK;
}

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    static int connection = 1;

    socket_manager_put(newpcb);

    // Pass the tcp_pcb struct to all callback functions
    //
    // It will appear in the "arg" parameter
    tcp_arg(newpcb, newpcb);

    tcp_recv(newpcb, recv_callback);
    tcp_sent(newpcb, sent_callback);
    tcp_err(newpcb, tcpip_disconnect_callback);

    // Increment for subsequent accepted connections
    connection++;

    return ERR_OK;
}

int start_tcpip(void)
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

    pcb->so_options |= SOF_KEEPALIVE; // ELS added to try to get sockets to detect a disconnect

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

    xil_printf("TCP listening for connections @ port %d\n\r", port);

    return XST_SUCCESS;
}
