#ifndef __CONTROLS_RTOS_H__
#define __CONTROLS_RTOS_H__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef enum {
    RTOS_ULTRA_PRIORITY = 4,
    RTOS_HIGH_PRIORITY = 3,
    RTOS_MEDIUM_PRIORITY = 2,
    RTOS_LOW_PRIORITY = 1,
} RTOS_TASK_PRIORITY;

#define RTOS_TASK(vTask, arg, priority) xTaskCreate(&vTask, #vTask, configMINIMAL_STACK_SIZE, arg, priority, NULL) 
#define RTOS_START() vTaskStartScheduler()

#endif