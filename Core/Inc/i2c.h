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
uint32_t bmeTest();
uint32_t mpuTest();
void toggleDisplay();
void displayOLEDDMAConfig(uint8_t numberOfBytes);
void sendOLEDCommand(uint8_t cmd);
void testDisplayOn();
int I2C1_CheckBusyRegister();
int I2C1_TransferComplete();
int I2C1_StopFlagDetected();
int I2C1_NackFlagDetected();
int I2C1_RXNotEmpty();
int I2C1_TxIsEmpty();
int I2C4_CheckBusyRegister();
int I2C4_TransferComplete();
int I2C4_StopFlagDetected();
int I2C4_NackFlagDetected();
int I2C4_RXNotEmpty();
int I2C4_TxIsEmpty();

#endif /* INC_I2C_H_ */
