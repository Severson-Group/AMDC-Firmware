/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
/* other includes */
#include "sys/control_task.h"
#include "sys/defines.h"
#include "sys/commands.h"

static TaskHandle_t *control_task_slots[CONTROL_TASK_SLOTS] = {NULL};
static TaskFunction_t control_tasks[CONTROL_TASK_SLOTS] = {cTask_0, cTask_1, cTask_2, cTask_3};
static TaskFunction_t control_task_callback[CONTROL_TASK_SLOTS] = {NULL};
static TickType_t control_task_frequency[CONTROL_TASK_SLOTS] = {0};

int create_control_task(TaskHandle_t *tcb, TaskFunction_t pxTaskCode, char *name, uint32_t updates_per_second) {
	int32_t first_open_slot = -1;
	for (uint32_t i = 0; i < CONTROL_TASK_SLOTS; i++) {
		if (control_task_slots[i] == NULL) {
			first_open_slot = i;
			break;
		}
	}
	if (first_open_slot == -1) {
		return FAILURE;
	}
	cmd_resp_printf("first open slot: %d\n", first_open_slot);
	control_task_slots[first_open_slot] = tcb;
	control_task_callback[first_open_slot] = pxTaskCode;
	control_task_frequency[first_open_slot] = pdMS_TO_TICKS(1000.0 / updates_per_second);
	xTaskCreate(control_tasks[first_open_slot], (const char *) name, 1024,
					NULL, tskHIGH_PRIORITY, tcb);
	return SUCCESS;
}

void delete_control_task(TaskHandle_t *tcb) {
	for (uint32_t i = 0; i < CONTROL_TASK_SLOTS; i++) {
		if (control_task_slots[i] == tcb) {
			control_task_slots[i] = NULL;
			control_task_callback[i] = NULL;
			control_task_frequency[i] = 0;
		}
	}
	vTaskDelete(*tcb);
}

void cTask_0(void *arg) {
	TickType_t last_wake = xTaskGetTickCount();
	for (;;) {
		vTaskDelayUntil(&last_wake, control_task_frequency[0]);
		control_task_callback[0](arg);
	}
}

void cTask_1(void *arg) {
	TickType_t last_wake = xTaskGetTickCount();
	for (;;) {
		vTaskDelayUntil(&last_wake, control_task_frequency[1]);
		control_task_callback[1](arg);
	}
}

void cTask_2(void *arg) {
	TickType_t last_wake = xTaskGetTickCount();
	for (;;) {
		vTaskDelayUntil(&last_wake, control_task_frequency[2]);
		control_task_callback[2](arg);
	}
}

void cTask_3(void *arg) {
	TickType_t last_wake = xTaskGetTickCount();
	for (;;) {
		vTaskDelayUntil(&last_wake, control_task_frequency[3]);
		control_task_callback[3](arg);
	}
}
