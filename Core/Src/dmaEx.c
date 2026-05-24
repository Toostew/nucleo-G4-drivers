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
#define DMA_CCR2			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x08 + (0x14 * (2 - 1)))))
#define DMA_CNDTR2			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x0C + (0x14 * (2 - 1)))))

#define DMAMUX_C1CR			(*((volatile uint32_t *)(DMAMUX_BASE_ADDR + (0x04 * 1)))) //for DMA1 channel 2


//memory address without dereferencing
#define I2C_TXDR			(((volatile uint32_t *)(I2C1_BASE_ADDR + 0x28UL))) //transmit memory address for I2C1



//this is the DMA setup for the OLED screen
//we set it up so that we send data for every pixel in the oled display
//it's 128 x 64 pixels, 8192 pixels, 8192 bits, or 1024 bytes (8 bits each)
//so we need to send 1024 bytes to change 1 screen.
void dmaSetupOLED(uint8_t * oledBuffer){

	RCC_AHB1ENR |= ((1 << 0) | (1 << 2)); //enable DMA1 and DMAUX1EN


	//remember DMAMUX channel 1 is mapped to DMA1 Channel 2
	DMAMUX_C1CR = 0x00000000; //reset
	DMAMUX_C1CR |= (17 << 0); //set DMAREQ_ID to I2C_TX. OLED display write only so we dont need an RX

	DMA_CMAR2 = (uint32_t)(oledBuffer);		//CMAR; Controller Memory Address; basically where we store our array of data to send
	DMA_CPAR2 = (uint32_t)(I2C_TXDR); //CPAR; Peripheral Address, where we are going to send the data to sequentially

	DMA_CCR2 = 0x00000000; //clear out CCR2 before setting it
	DMA_CCR2 |= ((1 << 7) | (1 << 4)); //bit 7:  memory increment, for incrementing down the array, bit 4: Data transfer direction, since we transmit, it's read from memory

	DMA_CNDTR2 = 0x00000000; //reset values
	DMA_CNDTR2 = (5 << 0); //4 bytes to be moved

	DMA_CCR2 |= (1 << 0); //enable dma channel 2

}




