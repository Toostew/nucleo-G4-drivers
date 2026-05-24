/*
 * dmaEx.c
 *
 *  Created on: 21 May 2026
 *      Author: tooka
 */
// This is the code for DMA
//STM32G474RE is category 3, so DMAMUX channels 0-7 are connected to DMA1.
//for this demo we are gonna use DMA 1 Channel 2 so we can do just a little more math

#include "dmaEx.h"


#define DMA1_BASE_ADDR		0x40020000
#define RCC_BASE_ADDR 		0x40021000
#define DMAMUX_BASE_ADDR	0x40020800
#define I2C1_BASE_ADDR		0x40005400


#define RCC_AHB1ENR			(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x48UL)))


#define DMA_CMAR2			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x14 + (0x14 * (2 - 1)))))
#define DMA_CPAR2			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x10 + (0x14 * (2 - 1)))))

#define DMAMUX_C1CR			(*((volatile uint32_t *)(DMAMUX_BASE_ADDR + (0x04 * 1)))) //for DMA1 channel 2


//memory address without dereferencing
#define I2C_TXDR			(((volatile uint32_t *)(I2C1_BASE_ADDR + 0x28UL))) //transmit memory address for I2C1



//this is the DMA setup for the OLED screen
void dmaSetupOLED(uint32_t oledBuffer){

	RCC_AHB1ENR |= ((1 << 0) | (1 << 2)); //enable DMA1 and DMAUX1EN


	//remember DMAMUX channel 1 is mapped to DMA1 Channel 2
	DMAMUX_C1CR = 0x00000000; //reset
	DMAMUX_C1CR |= (17 << 0); //set DMAREQ_ID to I2C_TX. OLED display write only so we dont need an RX

	DMA_CMAR2 = (oledBuffer);
	DMA_CPAR2 = (uint32_t)(I2C_TXDR);


}


