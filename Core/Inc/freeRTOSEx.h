/*
 * freeRTOSEx.h
 *
 *  Created on: 5 Jun 2026
 *      Author: tooka
 */

#ifndef INC_FREERTOSEX_H_
#define INC_FREERTOSEX_H_



#include "stm32g4xx_nucleo.h"
#include "stm32g4xx_hal.h"
#include <stdio.h>
#include "uartEx.h"

//FreeRTOS specific
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


typedef struct{
	QueueHandle_t UARTQueue;
} UARTParameters;

void rtos_task_setup();
void toggleLED1();
void toggleLED2();
void UARTsendData();

#endif /* INC_FREERTOSEX_H_ */
