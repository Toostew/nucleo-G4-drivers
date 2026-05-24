/*
 * i2c.h
 *
 *  Created on: 14 May 2026
 *      Author: tooka
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "stm32g4xx_hal.h"

#include "stm32g4xx_nucleo.h"
#include <stdio.h>

void I2C_Configuration();
uint32_t pingSensorTest();
void toggleDisplay();
uint8_t* toggleDisplayDMA();
void sendOLEDCommand(uint8_t cmd);
void testDisplayOn();
int checkBusyRegister();
int transferComplete();
int stopFlagDetected();
int nackFlagDetected();
int rxNotEmpty();
int txEmpty();
int txisEmpty();

#endif /* INC_I2C_H_ */
