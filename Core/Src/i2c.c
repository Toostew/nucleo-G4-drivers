/*
 * i2c.c
 *	implements I2C in bare metal for GY-521
 *  Created on: 13 May 2026
 *      Author: tooka
 */
#include "i2c.h"


//we will be using PB7 and PB8 for I2C1_SDA and SCL respectively

#define I2C1_BASE_ADDR		0x40005400
#define RCC_BASE_ADDR 		0x40021000
#define GPIOB_BASE_ADDR		0x48000400

#define RCC_APB1ENR 		(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x58UL)))	//for APB1 clock enable; for I2C1
#define RCC_AHB2ENR			(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x4CUL)))	//for GPIOB

#define I2C_CR1				(*((volatile uint32_t *)(I2C1_BASE_ADDR))) //CR1 has no offset
#define I2C_CR2				(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x04UL)))
#define I2C_TIMINGR			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x10UL)))
#define I2C_ISR				(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x18UL)))
#define I2C_ICR				(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x1CUL)))
#define I2C_RXDR			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x24UL)))
#define I2C_TXDR			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x28UL)))

#define GPIOB_MODER			(((volatile uint32_t *)(GPIOB_BASE_ADDR)))
#define GPIOB_OTYPER		(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x04UL)))
#define GPIOB_OSPEEDR		(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x08UL)))
#define GPIOB_PUPDR			(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x0CUL)))
#define GPIOB_AFRL			(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x20UL)))
#define GPIOB_AFRH			(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x24UL)))

//configuration function for I2C
void I2C_Configuration(){

	//GPIOB config

	//MODER setup, AF4; clear first
	*GPIOB_MODER &= ~(3 << 14);
	*GPIOB_MODER &= ~(3 << 16);
	*GPIOB_MODER |= (2 << 14);
	*GPIOB_MODER	|= (2 << 16);

	//configure OTYPER
	GPIOB_OTYPER |= (1 << 7);
	GPIOB_OTYPER |= (1 << 8);


}


