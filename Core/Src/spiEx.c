/*
 * spiEx.c
 *
 *  Created on: 8 May 2026
 *      Author: tooka
 */

//BASE ADDRESSES
#define SPI4_BASE_ADDR 		0x40013C00 //base address for SPI4 related registers
#define RCC_BASE_ADDR		0x40021000 //base address for RCC, on the AHB1 bus
#define GPIOB_BASE_ADDR		0x48000400


							//type cast, then dereference so that we dont have to constantly use * with a pointer to access data
#define RCC_APB2ENR			(*((volatile uint32_t *)(RCC_BASE_ADDR) + 0x60UL)) //address for APB2 peripheral clock enable
#define RCC_AHB2ENR  		(*((volatile uint32_t *)(RCC_BASE_ADDR) + 0x4CUL))	//address for AHB2 peripheral clock
#define SPI4_CR1			(*((volatile uint32_t *)(SPI4_BASE_ADDR))) //address for SPI4_CR1

#define GPIOB_MODER			(*((volatile uint32_t *)(GPIOB_BASE_ADDR))) //offset 0x00, MODER address
#define GPIOB_AFRH			(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x24UL))) //AFRH (High) for pins 8-15
#define GPIOB_OSPEEDR		(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x08UL))) //Configure IO output speed
#define GPIOB_BSRR			(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x18))) //Bit Set reset


//pin pre-config
void spiPinConfig(){

	//enable clock for APB2, specifically for SPI4, on bit 15
	RCC_APB2ENR |= (1 << 15); //turn on, 1 on bit 15
	RCC_AHB2ENR |= (1 << 1); //bit 1, gpioB

	//clear moder pins 12-15
	GPIOB_MODER &= ~(3 << 24); //pin 12
	GPIOB_MODER &= ~(3 << 26); //pin 13
	GPIOB_MODER &= ~(3 << 28); //pin 14
	GPIOB_MODER &= ~(3 << 30);

	//set mode for GPIOB pins 13-15 to alternate functions
	GPIOB_MODER |= (2 << 26); //pin 13
	GPIOB_MODER |= (2 << 28); //pin 14
	GPIOB_MODER |= (2 << 30); //pin 15

	//set mode for GPIOB pin 12 as GPO
	GPIOB_MODER |= (1 << 24); //pin 12

	//set GPIOB pin 12 to high by default, SPI CS HIGH means the slave is not selected
	GPIOB_BSRR |= (1 << 12); //pin 12, set 1

	//SPI runs at high speeds so we need to configure the GPIO speed to run at high speed
	GPIOB_OSPEEDR &= ~(3 << 26); //clear bits 26 - 30
	GPIOB_OSPEEDR &= ~(3 << 28);
	GPIOB_OSPEEDR &= ~(3 << 30);

	GPIOB_OSPEEDR |= (2 << 26); //pin 13
	GPIOB_OSPEEDR |= (2 << 28); //pin 14
	GPIOB_OSPEEDR |= (2 << 30); //pin 15

	//set AF to AF5, using AFR high, because pin within range 8-15
	GPIOB_AFRH |= (5 << 20); // pin 13
	GPIOB_AFRH |= (5 << 24); //pin 14
	GPIOB_AFRH |= (5 << 28); //pin 15



}



