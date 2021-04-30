#ifdef APP_CAN

#include "usr/can/task_can.h"
#include "drv/can.h"
#include "drv/hardware_targets.h"
#include "sys/scheduler.h"
#include "usr/user_config.h"
#include "xcanps.h"
#include <stdint.h>
#include <stdio.h>

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

int task_can_init(void)
{

    // Initialize the can peripheral
    //	can_init(0);

    if (scheduler_tcb_is_registered(&tcb)) {
        return FAILURE;
    }

    // Fill TCB with parameters
    scheduler_tcb_init(&tcb, task_can_callback, NULL, "can", TASK_CAN_INTERVAL_USEC);

    // Register task with scheduler
    return scheduler_tcb_register(&tcb);
}

int task_can_deinit(void)
{
    // Unregister task with scheduler
    return scheduler_tcb_unregister(&tcb);
}

int task_can_loopback_test(void)
{
    int Status;
    Status = can_loopback_test();
    if (Status != SUCCESS) {
        return FAILURE;
    }
    return SUCCESS;
}

int task_can_sendmessage(uint8_t *packet, int num_bytes)
{
    int Status;

    Status = can_send(packet, num_bytes);
    if (Status != SUCCESS) {
        return FAILURE;
    }
    return SUCCESS;
}

int task_can_print()
{
    int Status;
    Status = can_print();
    if (Status != SUCCESS) {
        return FAILURE;
    }
    return SUCCESS;
}

int task_can_print_mode()
{
    can_print_mode();
    return SUCCESS;
}

int task_can_print_peripheral()
{
    can_print_peripheral();
    return SUCCESS;
}

int task_can_setmode(uint32_t mode)
{
    int Status;

    Status = can_setmode(mode);
    if (Status != SUCCESS) {
        return FAILURE;
    }
    return SUCCESS;
}

int task_can_setbaud(int baud)
{
    int Status;
    Status = can_setbaud(baud);
    if (Status != SUCCESS) {
        return FAILURE;
    }
    return SUCCESS;
}

int task_can_set_btr(int jump, int first_time, int second_time)
{
    int Status;
    Status = can_set_btr(jump, first_time, second_time);
    if (Status != SUCCESS) {
        return FAILURE;
    }
    return SUCCESS;
}

int task_can_set_peripheral(int device_id)
{
    int Status;
    Status = can_set_peripheral(device_id);
    if (Status != SUCCESS)
        return FAILURE;
    return SUCCESS;
}

void task_can_callback(void *arg)
{
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D

#endif // USER_CONFIG_HARDWARE_TARGET
}

#endif // APP_CAN
