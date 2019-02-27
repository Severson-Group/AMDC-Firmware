/*
 * Copyright (c) 2008 Xilinx, Inc.  All rights reserved.
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

#include "mfs_config.h"
#ifndef __PPC__
#include "xil_printf.h"
#endif

int
platform_init_fs()
{
	/* initialize the memory file system (MFS) image pre-loaded into memory */
	mfs_init_genimage(MFS_NUMBYTES, (char *)(MFS_BASE_ADDRESS), MFS_INIT_TYPE);

	/* check if we can access index.html */
	if (mfs_exists_file("index.html") == 0) {
		xil_printf("%s: ERROR: unable to locate index.html in MFS\r\n", __FUNCTION__);
		xil_printf("One of your applications requires a Memory File System to be loaded.\r\n");
                xil_printf("Please check if MFS has been loaded, "
				"and it has index.html file in root directory\r\n");
		return -1;
	}
	return 0;
}
