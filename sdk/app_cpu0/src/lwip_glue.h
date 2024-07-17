#ifndef LWIP_GLUE_H
#define LWIP_GLUE_H

#include "netif/xadapter.h"

int setup_lwip(void);
int start_tcpip(void);

extern struct netif *my_netif;

#endif // LWIP_GLUE_H
