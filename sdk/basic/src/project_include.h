#ifndef __PROJECT_INCLUDE_H
#define __PROJECT_INCLUDE_H
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "xparameters.h"
#include "xparameters_ps.h"	/* defines XPAR values */
#include "xil_cache.h"
#include "netif/xadapter.h"
// #include "xgpio.h"
#include "xscutimer.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xuartps_hw.h"
#include "xtmrctr.h"

//LWIP
#include "lwip/tcp.h"
#include "lwip/tcp_impl.h"
#include "lwip/init.h"

#include "platform_config.h"
#include "project_settings.h"
#include "commands.h"
#include "main.h"
#include "log.h"
#include "prog_timer.h"
#include "axi_timer.h"
#include "lwip_glue.h"
#include "control.h"
#include "io_pecb.h"

#include "../bsp/bsp.h"

//Control applications (maybe make this conditional)
#include "../control_apps/VSI_DQ_SVPWM.h"
#include "../control_apps/HBridge_CC.h"
#include "../control_apps/PosContTry1.h"
#include "../control_apps/TorqueVSI.h"
#include "ControlApp.h"



#endif //__PROJECT_INCLUDE_H

