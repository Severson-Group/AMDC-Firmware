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

#include <string.h>

#include "mfs_config.h"
#include "lwip/inet.h"

#include "webserver.h"
#ifndef __PPC__
#include "xil_printf.h"
#endif



char *notfound_header =
	"<html> \
	<head> \
		<title>404</title> \
  		<style type=\"text/css\"> \
		div#request {background: #eeeeee} \
		</style> \
	</head> \
	<body> \
	<h1>404 Page Not Found</h1> \
	<div id=\"request\">";

char *notfound_footer =
	"</div> \
	</body> \
	</html>";

int generate_http_header(char *buf, char *fext, int fsize);
int toggle_leds();
unsigned int get_switch_state();
int mfs_file_read(int fd, char *buf, int buflen);
/* dynamically generate 404 response:
 *	this inserts the original request string in betwween the notfound_header & footer strings
 */
int do_404(struct tcp_pcb *pcb, char *req, int rlen)
{
	int len, hlen;
	int BUFSIZE = 1024;
	char buf[BUFSIZE];
	err_t err;

	len = strlen(notfound_header) + strlen(notfound_footer) + rlen;

	hlen = generate_http_header((char *)buf, "html", len);

	if (tcp_sndbuf(pcb) < hlen) {
		xil_printf("cannot send 404 message, tcp_sndbuf = %d bytes, message length = %d bytes\r\n",
				tcp_sndbuf(pcb), hlen);
		return -1;
	}
	if ((err = tcp_write(pcb, buf, hlen, 1)) != ERR_OK) {
		xil_printf("%s: error (%d) writing 404 http header\r\n", __FUNCTION__, err);
		return -1;
	}
	tcp_write(pcb, notfound_header, strlen(notfound_header), 1);
	tcp_write(pcb, req, rlen, 1);
	tcp_write(pcb, notfound_footer, strlen(notfound_footer), 1);

	return 0;
}

int do_http_post(struct tcp_pcb *pcb, char *req, int rlen)
{
	int BUFSIZE = 1024;
	unsigned char buf[BUFSIZE];
	int len, n;
    char *p;

	if (is_cmd_led(req)) {
		n = toggle_leds();
		len = generate_http_header((char *)buf, "js", 1);
        p = (char *)buf + len;
        *p++ = n?'1':'0';
        *p = 0;
		len++;
		xil_printf("http POST: ledstatus: %x\r\n", n);
	} else if (is_cmd_switch(req)) {
		unsigned s = get_switch_state();
		int n_switches = 8;

		xil_printf("http POST: switch state: %x\r\n", s);
		len = generate_http_header((char *)buf, "js", n_switches);
		p = (char *)buf + len;
		for (n = 0; n < n_switches; n++, p++) {
				*p = '0' + (s & 0x1);
				s >>= 1;
		}
		*p = 0;

		len += n_switches;

	}

	if (tcp_write(pcb, buf, len, 1) != ERR_OK) {
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}

	return 0;
}

/* respond for a file GET request */
int do_http_get(struct tcp_pcb *pcb, char *req, int rlen)
{

	int BUFSIZE = 1400;
	char filename[MAX_FILENAME];
	unsigned char buf[BUFSIZE];
	signed int fsize, hlen, n;
	int fd;
	char *fext;
	err_t err;

	/* determine file name */
	extract_file_name(filename, req, rlen, MAX_FILENAME);

	/* respond with 404 if not present */
	if (mfs_exists_file(filename) != 1) {
		xil_printf("requested file %s not found, returning 404\r\n", filename);
		do_404(pcb, req, rlen);
		return -1;
	}

	/* respond with correct file */

	/* debug statement on UART */
        xil_printf("http GET: %s\r\n", filename);

        /* get a pointer to file extension */
	fext = get_file_extension(filename);


	fd = mfs_file_open(filename, MFS_MODE_READ);
	if (fd == -1) {
		platform_init_fs();
		extract_file_name(filename, req, rlen, MAX_FILENAME);
		if (mfs_exists_file(filename) != 1) {
				xil_printf("requested file %s not found, returning 404\r\n", filename);
				do_404(pcb, req, rlen);
				return -1;
		}
		fext = get_file_extension(filename);
		fd = mfs_file_open(filename, MFS_MODE_READ);
		return -1;
	}


	/* obtain file size,
	 * note that lseek with offset 0, MFS_SEEK_END does not move file pointer */
	fsize = mfs_file_lseek(fd, 0, MFS_SEEK_END);
	if (fsize == -1) {
		xil_printf("\r\nFile Read Error\r\n");
		return -1;
	}

	/* write the http headers */
	hlen = generate_http_header((char *)buf, fext, fsize);
	if ((err = tcp_write(pcb, buf, hlen, 3)) != ERR_OK) {
		xil_printf("error (%d) writing http header to socket\r\n", err);
		xil_printf("attempted to write #bytes = %d, tcp_sndbuf = %d\r\n", hlen, tcp_sndbuf(pcb));
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	tcp_output(pcb);

	/* now write the file */
	while (fsize > 0) {
		int sndbuf;
		sndbuf = tcp_sndbuf(pcb);

		if (sndbuf < BUFSIZE) {
			/* not enough space in sndbuf, so send remaining bytes when there is space */
			/* this is done by storing the fd in as part of the tcp_arg, so that the sent
			   callback handler knows to send data */
			http_arg *a = (http_arg *)pcb->callback_arg;
			a->fd = fd;
			a->fsize = fsize;
			return -1;
		}

		n = mfs_file_read(fd, (char *)buf, BUFSIZE);

		if ((err = tcp_write(pcb, buf, n, 3)) != ERR_OK) {
			xil_printf("error writing file (%s) to socket, remaining unwritten bytes = %d\r\n",
					filename, fsize - n);
			xil_printf("attempted to lwip_write %d bytes, tcp write error = %d\r\n", n, err);
			break;
		}
		tcp_output(pcb);
		if (fsize >= n)
			fsize -= n;
		else
			fsize = 0;
	}

	mfs_file_close(fd);


	return 0;

}

enum http_req_type { HTTP_GET, HTTP_POST, HTTP_UNKNOWN };
enum http_req_type decode_http_request(char *req, int l)
{
	char *get_str = "GET";
	char *post_str = "POST";

	if (!strncmp(req, get_str, strlen(get_str)))
		return HTTP_GET;

	if (!strncmp(req, post_str, strlen(post_str)))
		return HTTP_POST;

	return HTTP_UNKNOWN;
}

void dump_payload(char *p, int len)
{
	int i, j;

	for (i = 0; i < len; i+=16) {
		for (j = 0; j < 16; j++)
			xil_printf("%c ", p[i+j]);
		xil_printf("\r\n");
	}
	xil_printf("total len = %d\r\n", len);
}

/* generate and write out an appropriate response for the http request */
/* 	this assumes that tcp_sndbuf is high enough to send atleast 1 packet */
int generate_response(struct tcp_pcb *pcb, char *http_req, int http_req_len)
{
	enum http_req_type request_type = decode_http_request(http_req, http_req_len);

	switch(request_type) {
	case HTTP_GET:
		return do_http_get(pcb, http_req, http_req_len);
	case HTTP_POST:
		return do_http_post(pcb, http_req, http_req_len);
	default:
		xil_printf("request_type != GET|POST\r\n");
		dump_payload(http_req, http_req_len);
		return do_404(pcb, http_req, http_req_len);
	}

}

