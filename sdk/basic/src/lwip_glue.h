
#ifndef __LWIP_GLUE_H
#define __LWIP_GLUE_H
extern void lwip_timer_callback();
extern int SetupLWIP();
extern int start_tcpip();

extern volatile char bEnableEthTimer;
extern struct netif *echo_netif;
extern char bEth250ms;

#endif //__LWIP_GLUE_H
