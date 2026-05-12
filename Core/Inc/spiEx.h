/*
 * spiEx.h
 *
 *  Created on: 9 May 2026
 *      Author: tooka
 */

#ifndef INC_SPIEX_H_
#define INC_SPIEX_H_

#include "stm32g4xx_nucleo.h"
#include "stm32g4xx_hal.h"
#include <stdio.h>

void spiPinConfig();
void spiPinConfig_Silent();
void SPIPinReset();
int checkBusy();
int checkTransmitBufferEmpty();
int checkReceiveBufferEmpty();
void SetCSGPIO(int mode);
void powerBME(int mode);
void SPITest();
uint8_t getID();


#endif /* INC_SPIEX_H_ */
