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

#ifndef __TFTP_UTILS_H_
#define __TFTP_UTILS_H_

#include "tftpserver.h"

tftp_opcode tftp_decode_op(char *buf);
void tftp_extract_filename(char *fname, char *buf);
u16_t tftp_extract_block(char *buf);
void tftp_set_opcode(char *buf, tftp_opcode op);
void tftp_set_errorcode(char *buf, tftp_errorcode err);
void tftp_set_errormsg(char *buf, char *errormsg);
int tftp_is_correct_ack(char *buf, int block);
void tftp_set_data_message(char *packet, char *buf, int n);
void tftp_set_block(char *packet, int block);

#endif
