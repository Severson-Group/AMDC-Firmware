#ifndef SYS_CONTROL_TASK_H
#define SYS_CONTROL_TASK_H

#define CONTROL_TASK_SLOTS 4

int create_control_task(TaskHandle_t *tcb, TaskFunction_t pxTaskCode, char *name, uint32_t updates_per_second);

void delete_control_task(TaskHandle_t *tcb);

void cTask_0(void *arg);
void cTask_1(void *arg);
void cTask_2(void *arg);
void cTask_3(void *arg);

#endif /* SYS_CONTROL_TASK_H */
