/*
 * uartEx.h
 *
 *  Created on: 7 May 2026
 *      Author: tooka
 */

#ifndef INC_UARTEX_H_
#define INC_UARTEX_H_

#include "stm32g4xx_nucleo.h"
#include "stm32g4xx_hal.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

void uartPinConfig();
void togglePinPB1();
void togglePinPB2();
void UARTSendChar(char c);
void UARTSendString(char* str); //remember a string is just an array of chars, we could just store the first char in the array
void print_hex(uint32_t value);

#endif /* INC_UARTEX_H_ */
