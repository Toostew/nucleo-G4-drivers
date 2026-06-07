/*
 * freeRTOSSemaphoreTest.h
 *
 *  Created on: 7 Jun 2026
 *      Author: tooka
 */

#ifndef INC_FREERTOSSEMAPHORETEST_H_
#define INC_FREERTOSSEMAPHORETEST_H_

#include "stm32g4xx_hal.h"

#include "stm32g4xx_nucleo.h"
#include <stdio.h>

#include "i2c.h"
#include "uartEx.h"
#include "dmaEx.h"
#include <freeRTOSQueueTest.h>


//FreeRTOS specific
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"


void pinConfig();
void semaphoreConfig();
void EXTI15_10_IRQHandler();
void vButtonHandlerTask();

#endif /* INC_FREERTOSSEMAPHORETEST_H_ */
