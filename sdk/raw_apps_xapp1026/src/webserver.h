/*
 * Copyright (c) 2009 Xilinx, Inc.  All rights reserved.
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

#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

#include "lwip/tcp.h"

#define MAX_FILENAME 256

/* initialize file system layer */
int platform_init_fs();

/* initialize device layer */
int http_init_devices();

/* web_utils.c utilities */
void extract_file_name(char *filename, char *req, int rlen, int maxlen);

char *get_file_extension(char *buf);
int is_cmd_switch(char *buf);
int is_cmd_led(char *buf);


int generate_response(struct tcp_pcb *pcb, char *http_req, int http_req_len);

typedef struct {
	int count;
	int fd;
	int fsize;
} http_arg;

http_arg *palloc_arg();
void pfree_arg(http_arg *);

#endif
