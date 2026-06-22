/*
 * dmaEx.h
 *
 *  Created on: 21 May 2026
 *      Author: tooka
 */

#ifndef INC_DMAEX_H_
#define INC_DMAEX_H_


#include "stm32g4xx_hal.h"

#include "stm32g4xx_nucleo.h"
#include <stdio.h>


void dmaSetupSensorArray(uint32_t DMA1_MemoryBuffer, uint32_t DMA2_MemoryBuffer,
						uint32_t DMA3_MemoryBuffer, uint32_t DMA4_MemoryBuffer);
void dmaSetupOLED();
void transmitOLED(uint8_t* arrayPointer, uint16_t numberOfBytes);


#endif /* INC_DMAEX_H_ */
