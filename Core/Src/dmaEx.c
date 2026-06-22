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
#define I2C4_BASE_ADDR		0x40008400


#define RCC_AHB1ENR			(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x48UL)))

//DMA channels 1-4
#define DMA_CMAR1			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x14 + (0x14 * (1 - 1)))))
#define DMA_CPAR1			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x10 + (0x14 * (1 - 1)))))
#define DMA_CCR1			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x08 + (0x14 * (1 - 1)))))
#define DMA_CNDTR1			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x0C + (0x14 * (1 - 1)))))

#define DMA_CMAR2			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x14 + (0x14 * (2 - 1)))))
#define DMA_CPAR2			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x10 + (0x14 * (2 - 1)))))
#define DMA_CCR2			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x08 + (0x14 * (2 - 1)))))
#define DMA_CNDTR2			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x0C + (0x14 * (2 - 1)))))

#define DMA_CMAR3			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x14 + (0x14 * (3 - 1)))))
#define DMA_CPAR3			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x10 + (0x14 * (3 - 1)))))
#define DMA_CCR3			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x08 + (0x14 * (3 - 1)))))
#define DMA_CNDTR3			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x0C + (0x14 * (3 - 1)))))

#define DMA_CMAR4			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x14 + (0x14 * (4 - 1)))))
#define DMA_CPAR4			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x10 + (0x14 * (4 - 1)))))
#define DMA_CCR4			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x08 + (0x14 * (4 - 1)))))
#define DMA_CNDTR4			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x0C + (0x14 * (4- 1)))))


#define DMAMUX_C0CR			(*((volatile uint32_t *)(DMAMUX_BASE_ADDR + (0x04 * 0)))) //DMA1 channel 1
#define DMAMUX_C1CR			(*((volatile uint32_t *)(DMAMUX_BASE_ADDR + (0x04 * 1)))) //DMA1 channel 2
#define DMAMUX_C2CR			(*((volatile uint32_t *)(DMAMUX_BASE_ADDR + (0x04 * 2)))) //DMA1 channel 3
#define DMAMUX_C3CR			(*((volatile uint32_t *)(DMAMUX_BASE_ADDR + (0x04 * 3)))) //DMA1 channel 4


//memory address without dereferencing
//deprecated, old define we wont use
#define I2C_TXDR			(((volatile uint32_t *)(I2C1_BASE_ADDR + 0x28UL))) //transmit memory address for I2C1
#define I2C_CR2				(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x04UL)))


#define I2C1_TXDR			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x28UL)))
#define I2C1_RXDR			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x24UL)))
#define I2C1_CR2			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x04UL)))

#define I2C4_TXDR			(*((volatile uint32_t *)(I2C4_BASE_ADDR + 0x28UL)))
#define I2C4_RXDR			(*((volatile uint32_t *)(I2C4_BASE_ADDR + 0x24UL)))
#define I2C4_CR2			(*((volatile uint32_t *)(I2C4_BASE_ADDR + 0x04UL)))



//This is the DMA setup for BME280 and MPU6050
//we need 2 different channels for the 2 I2C devices, in this case DMA1 Channel 1 and 2
//in order to use them on this board we need to configure the DMAMUX to expect I2Cx traffic on DMA channels 1 and 2
//since we are operating 2 I2C devices we need to input commands and output data, so 2x2 channels will be needed
//RUN THIS BEFORE THE OTHER I2C1 and I2C4 PERIPHERAL TEST!!!!!!!!!!!!!!!!
void dmaSetupSensorArray(uint32_t DMA1_MemoryBuffer, uint32_t DMA2_MemoryBuffer,
						uint32_t DMA3_MemoryBuffer, uint32_t DMA4_MemoryBuffer){


	//RCC, enable DMA1 and DMAMUX1 (despite the name there is only 1 DMAMUX)
	RCC->AHB1ENR |= ((1 << 0) | (1 << 2));

	//DMAMUX setup
	//DMA channel 1 - 8 is mapped to DMAMUX channel 0 - 7
	//DMA request IDs can be found at page 420 in RM0440
	DMAMUX_C0CR &= ~(0b1111111 << 0); //clear DMAREQ_ID reg
	DMAMUX_C0CR |= (16 << 0); //DMA channel 1 is for I2C1 RX

	DMAMUX_C1CR &= ~(0b1111111 << 0); //clear DMAREQ_ID reg
	DMAMUX_C1CR |= (17 << 0); //DMA channel 2 is for I2C1 TX

	DMAMUX_C2CR &= ~(0b1111111 << 0); //clear DMAREQ_ID reg
	DMAMUX_C2CR |= (22 << 0); //DMA channel 3 is for I2C4 RX

	DMAMUX_C3CR &= ~(0b1111111 << 0); //clear DMAREQ_ID reg
	DMAMUX_C3CR |= (23 << 0); //DMA channel 4 is for I2C4 TX

	//DMA CMAR and CPAR
	//CMAR is the memory buffer DMA will read and write to for data input and output
	//CPAR is the address of the peripheral's transfer/receive reg. Specify in DMA_CCRx
	DMA_CMAR1 = (DMA1_MemoryBuffer);
	DMA_CMAR2 = (DMA2_MemoryBuffer);
	DMA_CMAR3 = (DMA3_MemoryBuffer);
	DMA_CMAR4 = (DMA4_MemoryBuffer);

	//CPAR setup, for I2C RX and TX
	DMA_CPAR1 = (uint32_t)(I2C1_RXDR);
	DMA_CPAR2 = (uint32_t)(I2C1_TXDR);
	DMA_CPAR3 = (uint32_t)(I2C4_RXDR);
	DMA_CPAR4 = (uint32_t)(I2C4_TXDR);


	DMA_CCR1 = ((1 << 7) | (0 << 4)); //I2C1 rx
	DMA_CCR2 = ((1 << 7) | (1 << 4)); //I2C1 tx
	DMA_CCR3 = ((1 << 7) | (0 << 4)); //I2C4 rx
	DMA_CCR4 = ((1 << 7) | (1 << 4)); //I2C4 tx

}

void setupMPU_6050(){

}



void testMPU_6050(){

}






//transmit based on CMAR location
void transmitI2C(){



}







//this is the DMA setup for the OLED screen
//we set it up so that we send data for every pixel in the oled display
//it's 128 x 64 pixels, 8192 pixels, 8192 bits, or 1024 bytes (8 bits each)
//so we need to send 1024 bytes to change 1 screen.
void dmaSetupOLED(){

	RCC_AHB1ENR |= ((1 << 0) | (1 << 2)); //enable DMA1 and DMAUX1EN


	//remember DMAMUX channel 1 is mapped to DMA1 Channel 2
	DMAMUX_C1CR = 0x00000000; //reset
	DMAMUX_C1CR |= (17 << 0); //set DMAREQ_ID to I2C_TX. OLED display write only so we dont need an RX

	//DMA_CMAR2 = (uint32_t)(oledBuffer);		//CMAR; Controller Memory Address; basically where we store our array of data to send
	DMA_CPAR2 = (uint32_t)(I2C_TXDR); //CPAR; Peripheral Address, where we are going to send the data to sequentially

	DMA_CCR2 = 0x00000000; //clear out CCR2 before setting it
	DMA_CCR2 |= ((1 << 7) | (1 << 4)); //bit 7:  memory increment, for incrementing down the array, bit 4: Data transfer direction, since we transmit, it's read from memory



	//deprecated, turn on dma channel 2 inside the transmit OLED
	//DMA_CCR2 |= (1 << 0); //enable dma channel 2

}


//dynamic transmission protocol
//this function is called every time you want to send new data to the OLED,
//TODO: in the future maybe make it so that it can support multiple devices
void transmitOLED(uint8_t* arrayPointer, uint16_t numberOfBytes){

	//wait for all bytes from the previous transmission to complete
	while (DMA_CNDTR2 > 0);

	//disable dma channel 2 and the I2C peripheral before we do anything
	DMA_CCR2 &= ~(1 << 0);


	//reset and set bytes to be moved
	DMA_CNDTR2 = 0x00000000; //reset values
	DMA_CNDTR2 = (numberOfBytes << 0); //x bytes to be moved

	DMA_CMAR2 = (uint32_t)(arrayPointer);		//CMAR; Controller Memory Address; basically where we store our array of data to send, typecast to 32 bit unsigned int

	//clear cr2 config, //bit 23 - 16 are the number of bytes we have to clear the whole thing
	I2C_CR2 &= ~((1 << 25) | (0b11111111 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

	//Set NBYTES = 5, Write Mode (0<<10), Address (0x3C << 1), and AUTOEND = 1 (1 << 25)
	I2C_CR2 |= (numberOfBytes << 16) | (0 << 10) | (0x3C << 1) | (1 << 25);

	DMA_CCR2 |= (1 << 0); //enable dma channel 2

	I2C_CR2 |= (1 << 13); //launch start bit

}




