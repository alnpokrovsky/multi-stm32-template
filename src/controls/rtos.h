#ifndef __CONTROLS_RTOS_H__
#define __CONTROLS_RTOS_H__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef enum {
    RTOS_ULTRA_PRIORITY = 15,
    RTOS_HIGH_PRIORITY = 10,
    RTOS_MEDIUM_PRIORITY = 5,
    RTOS_LOW_PRIORITY = 1,
} RTOS_TASK_PRIORITY;

#define GET_MACRO(_1,_2,_3,_4,NAME,...) NAME
#define RTOS_TASK(...) GET_MACRO(__VA_ARGS__, RTOS_TASK4, RTOS_TAS3, RTOS_TASK2)(__VA_ARGS__)

#define RTOS_TASK4(priority, vTask, arg, mem)   xTaskCreate(&vTask, #vTask, configMINIMAL_STACK_SIZE, arg, priority, NULL)
#define RTOS_TASK3(priority, vTask, arg)        RTOS_TASK4(priority, vTask, arg, configMINIMAL_STACK_SIZE)
#define RTOS_TASK2(priority, vTask)             RTOS_TASK3(priority, vTask, NULL)

#define RTOS_START() vTaskStartScheduler()

#define RTOS_DELAY_MS(t) vTaskDelay(t)
#define RTOS_DELAY_LOOP  portTickType xLastWakeTime = xTaskGetTickCount(); while (1)
#define RTOS_DELAY_NEXT_MS(t) vTaskDelayUntil( &xLastWakeTime, t )

#define RTOS_CRITICAL_ENTER()   taskENTER_CRITICAL()
#define RTOS_CRITICAL_EXIT()    taskEXIT_CRITICAL()

#endif