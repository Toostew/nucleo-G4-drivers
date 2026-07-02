/*
 * BME280.c
 *
 *  Created on: 25 Jun 2026
 *      Author: tooka
 */

#include "BME280.h"

static uint8_t DMA1Buffer[32]; //DMA channel 1, RX, I2C1, BME280
static uint8_t DMA2Buffer[32];
static uint8_t DMA3Buffer[32];
static uint8_t DMA4Buffer[32];


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

#define DMA_ISR				(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x00)))
#define DMA_IFCR			(*((volatile uint32_t *)(DMA1_BASE_ADDR + 0x04)))


//memory address without dereferencing
//deprecated, old define we wont use
#define I2C_TXDR			(((volatile uint32_t *)(I2C1_BASE_ADDR + 0x28UL))) //transmit memory address for I2C1
#define I2C_CR2				(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x04UL)))


#define I2C1_TXDR			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x28UL)))
#define I2C1_RXDR			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x24UL)))
#define I2C1_CR1			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x00UL)))
#define I2C1_CR2			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x04UL)))
#define I2C1_ISR			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x18UL)))
#define I2C1_ICR			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x1CUL)))


#define I2C4_TXDR			(*((volatile uint32_t *)(I2C4_BASE_ADDR + 0x28UL)))
#define I2C4_RXDR			(*((volatile uint32_t *)(I2C4_BASE_ADDR + 0x24UL)))
#define I2C4_CR1			(*((volatile uint32_t *)(I2C4_BASE_ADDR + 0x00UL)))
#define I2C4_CR2			(*((volatile uint32_t *)(I2C4_BASE_ADDR + 0x04UL)))
#define I2C4_ICR			(*((volatile uint32_t *)(I2C4_BASE_ADDR + 0x1CUL)))

#define MPU6050_SLAVE_ADDR	0x68
#define BME280_SLAVE_ADDR	0x76




//the BME280 is a sensor by Bosch to measure humidity, pressure and temperature
//unlike the MPU6050 this sensor needs quite a bit of pre-processing before we can dsplay the information
//the BME features register shadowing, wherein, when we begin reading the sensor data registers, the sensor stops updating the registers
//this is done to prevent innacurate data from accidental overwriting while reading (this is race condition!)
//the sensor also requires calibration with dedicated calibration registers, which you'll have to read too (just once at boot)
//in the BME280, burst reads automatically increment an internal pointer so you can read sequential registers faster


//we ought to capture the 18 registers during this
void BME_I2C_Setup(){


	//set up DMA with these buffers
	dmaSetupSensorArray((uint32_t)DMA1Buffer,
			(uint32_t)DMA2Buffer,
			(uint32_t)DMA3Buffer,
			(uint32_t)DMA4Buffer);

	//read 24 bytes 0x88 - 0x9F

	I2C_DMA_BurstRead(1, 24, 0x88);


}




