/*
 * interrupt.h
 *
 *  Created on: 3 Jul 2026
 *      Author: tooka
 */

#ifndef INC_INTERRUPT_H_
#define INC_INTERRUPT_H_

#include "stm32g4xx_hal.h"

#include "stm32g4xx_nucleo.h"
#include <stdio.h>
#include <stdint.h>

#include "i2c.h"
#include "uartEx.h"
#include "dmaEx.h"
#include <freeRTOSQueueTest.h>
#include "freeRTOSSemaphoreTest.h"
#include "mpu6050.h"
#include "BME280.h"

//FreeRTOS specific
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"


void sensorArrayInterruptConfig();

#endif /* INC_INTERRUPT_H_ */
