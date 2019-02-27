/*
 * Copyright (c) 2007 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/* webserver.c: An example Webserver application using the RAW API
 *	This program serves web pages resident on Xilinx Memory File
 * System (MFS) using lwIP's RAW API. Use of RAW API implies that the
 * webserver is blazingly fast, but the design is not obvious since a
 * lot of the work happens in asynchronous callback functions.
 *
 * The webserver works as follows:
 *	- on every accepted connection, only 1 read is performed to
 * identify the file requested. Further reads are avoided by sending
 * a "Connection: close" in the HTTP response header, as well as setting
 * the callback function to NULL on that pcb
 *	- the read determines what file needs to be set (by parsing
 * "GET / HTTP/1.1" request
 *	- once the file to be sent is determined, tcp_write is called
 * in chunks of size tcp_sndbuf() until the whole file is sent
 *
 */

#include <stdio.h>
#include <string.h>

#include "lwip/err.h"
#include "lwip/tcp.h"

#include "webserver.h"
#ifndef __PPC__
#include "xil_printf.h"
#endif
#include "xilmfs.h"

/* static variables controlling debug printf's in this file */
static int g_webserver_debug = 0;

static unsigned http_port = 80;
static unsigned http_server_running = 0;

void platform_init_gpios();

int transfer_web_data() {
	return 0;
}

err_t
http_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	int BUFSIZE = 1400, sndbuf, n;
	char buf[BUFSIZE];
	http_arg *a = (http_arg*)arg;

	if (g_webserver_debug)
		xil_printf("%d (%d): S%d..\r\n", a?a->count:0, tpcb->state, len);

	if (tpcb->state > ESTABLISHED) {
		if (a) {
			pfree_arg(a);
			a = NULL;
		}
		tcp_close(tpcb);
        return ERR_OK;
	}

	if (a->fd == -1 || a->fsize <= 0) /* no more data to be sent */
		return ERR_OK;

	/* read more data out of the file and send it */
        while (1) {
            sndbuf = tcp_sndbuf(tpcb);
            if (sndbuf < BUFSIZE)
                return ERR_OK;

            xil_printf("attempting to read %d bytes, left = %d bytes\r\n", BUFSIZE, a->fsize);
            n = mfs_file_read(a->fd, buf, BUFSIZE);
            tcp_write(tpcb, buf, n, 1);
            a->fsize -= n;

            if (a->fsize <= 0) {
                mfs_file_close(a->fd);
                a->fd = 0;
                break;
            }
        }
	return ERR_OK;
}

err_t
http_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	http_arg *a = (http_arg*)arg;

		if (g_webserver_debug)
			xil_printf("%d (%d): R%d %d..\r\n", a?a->count:0, tpcb->state, p->len, p->tot_len);

		/* do not read the packet if we are not in ESTABLISHED state */
		if (tpcb->state >= 5 && tpcb->state <= 8) {
			pbuf_free(p);
			return -1;
		} else if (tpcb->state > 8) {
			pbuf_free(p);
			return -1;
		}

		/* acknowledge that we've read the payload */
		tcp_recved(tpcb, p->len);

		/* read and decipher the request */
		/* this function takes care of generating a request, sending it,
		 *	and closing the connection if all data can been sent. If
		 *	not, then it sets up the appropriate arguments to the sent
		 *	callback handler.
		 */
		generate_response(tpcb, p->payload, p->len);

		/* free received packet */
		pbuf_free(p);

		return ERR_OK;
}

static err_t
http_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	/* keep a count of connection # */
	tcp_arg(newpcb, (void*)palloc_arg());

	tcp_recv(newpcb, http_recv_callback);
	tcp_sent(newpcb, http_sent_callback);

	return ERR_OK;
}

int
start_web_application()
{
	struct tcp_pcb *pcb;
	err_t err;

	/* initialize file system layer */
	platform_init_fs();

	/* initialize devices */
	platform_init_gpios();

	/* create new TCP PCB structure */
	pcb = tcp_new();
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\r\n");
		return -1;
	}

	/* bind to http port 80 */
	err = tcp_bind(pcb, IP_ADDR_ANY, http_port);
	if (err != ERR_OK) {
		xil_printf("Unable to bind to port 80: err = %d\r\n", err);
		return -2;
	}

	/* we do not need any arguments to the first callback */
	tcp_arg(pcb, NULL);

	/* listen for connections */
	pcb = tcp_listen(pcb);
	if (!pcb) {
		xil_printf("Out of memory while tcp_listen\r\n");
		return -3;
	}

	/* specify callback to use for incoming connections */
	tcp_accept(pcb, http_accept_callback);

        http_server_running = 1;

	return 0;
}

void print_web_app_header()
{
    xil_printf("%20s %6d %s\r\n", "http server",
                        http_port,
                        "Point your web browser to http://192.168.1.10");
}
