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
#define RCC_CCIPR			(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x88UL)))	//for I2C1 clock source config

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

	//clock config
	//enable clocks
	RCC_APB1ENR |= (1 << 21);
	RCC_AHB2ENR |= (1 << 1);

	//change source clock for I2C1 to use HSI, 16 MHZ. makes my life easier
	RCC_CCIPR &= ~(3 << 12);
	RCC_CCIPR |= (2 << 12); //select HSI for clock source, 16 MHz




	//GPIOB config

	//MODER setup, AF4; clear first
	//NOTE: notice that we didnt put a dereference on the define, because of that we need to manually dereference here
	*GPIOB_MODER &= ~(3 << 14);
	*GPIOB_MODER &= ~(3 << 16);
	*GPIOB_MODER |= (2 << 14);
	*GPIOB_MODER	|= (2 << 16);

	//configure OTYPER
	GPIOB_OTYPER |= (1 << 7);
	GPIOB_OTYPER |= (1 << 8);

	//configure OSPEEDR
	GPIOB_OSPEEDR &= ~(3 << 14);
	GPIOB_OSPEEDR &= ~(3 << 16);
	GPIOB_OSPEEDR |= (2 << 14);
	GPIOB_OSPEEDR |= (2 << 16);

	//PUPDR setup, set for pull up
	GPIOB_PUPDR &= ~(3 << 14); //reset
	GPIOB_PUPDR &= ~(3 << 16);
	GPIOB_PUPDR |= (1 << 14); // Set to pull up
	GPIOB_PUPDR	|= (1 << 16);

	//AFR L and H setup (L for pin 7, H for pin 8)
	GPIOB_AFRL	&= ~(15 << 28);
	GPIOB_AFRL	|= (4 << 28); //pin 7
	GPIOB_AFRH	&= ~(15 << 0);
	GPIOB_AFRH	|= (4 << 0); //pin 8


	//I2C Configuration
	//disable I2C just in case
	I2C_CR1 &= ~((1 << 0) | (1 << 14));

	I2C_TIMINGR = 0x00000000; //reset values to default

	I2C_TIMINGR |= ((0x3 << 28) | (0x13 << 0) | (0xF << 8) | (0x4 << 20) | (0x2 << 16));

	//TXDMAEN enabled, turn on DMA transmussuib requests
	I2C_CR1 |= (1 << 14); // disable if not using DMA

	//enable I2C
	I2C_CR1 |= (1 << 0);

}

//we will read the WHOAMI address on the mpu 6050
uint32_t pingSensorTest(){

	//clear Bus Error, NACK, STOP flag registers, these are event flags and are sticky, need to reset manually
	I2C_ICR |= (1 << 5) | (1 << 4) | (1 << 8);


	//first message for read
	//CR2 setup
	//clear AUTOEND, NBYTES, STOP, START, ADD10, RD_WRN, and SADD
	I2C_CR2 &= ~((1 << 25) | (1 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

	//set number of bytes, START, write mode, and target address
	I2C_CR2 = ((1 << 16)  | (0 << 10) | (0x68 << 1));

	I2C_CR2 |= (1 << 13); //enable start bit

	while(!txisEmpty()){
		//during transmission there could be a chance nothing comes back, NACK is flagged if nothing responds
		if(nackFlagDetected()){

			//generate a stop to end the transmission early
			I2C_CR2 = (1 << 14);

			//clear nack flag
			I2C_ICR = (1 << 4);

			return 0xFFFFFFFF;
		}
	} //wait for transmit data register is empty

	I2C_TXDR = 0x75;

	while(!transferComplete());

	//listen phase
	//number of bytes, start and target address dont need to change, but rewrite for redundancy
	//set start bit LAST
	I2C_CR2 &= ~((1 << 25) | (1 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

	I2C_CR2 = ((1 << 25) | (1 << 16) | (0 << 11) | (1 << 10) | (0x68 << 1));

	I2C_CR2 |= (1 << 13); //enable start bit

	while(!rxNotEmpty()); //hold until RX is empty
	uint32_t data = I2C_RXDR;


	//use stop flags as more like milestones and not actual conditionals to dictate logics directly
	while(!stopFlagDetected());

	I2C_ICR |= (1 << 5);


	return data;
}



void toggleDisplay(){
    // 1. Clear sticky event flags
    I2C_ICR |= (1 << 5) | (1 << 4) | (1 << 8);

    // 2. Clear old CR2 configurations
    I2C_CR2 &= ~((1 << 25) | (1 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

    // 3. Set NBYTES = 5, Write Mode (0<<10), Address (0x3C << 1), and AUTOEND = 1 (1 << 25)
    I2C_CR2 = (5 << 16) | (0 << 10) | (0x3C << 1) | (1 << 25);

    // 4. Fire the START bit
    I2C_CR2 |= (1 << 13);

    // --- BYTE 1: Control Byte (0x00) ---
    while(!txisEmpty()) { if(nackFlagDetected()) { goto handle_nack; } }
    I2C_TXDR = 0x00;

    // --- BYTE 2: Charge Pump Set (0x8D) ---
    while(!txisEmpty()) { if(nackFlagDetected()) { goto handle_nack; } }
    I2C_TXDR = 0x8D;

    // --- BYTE 3: Enable Charge Pump (0x14) ---
    while(!txisEmpty()) { if(nackFlagDetected()) { goto handle_nack; } }
    I2C_TXDR = 0x14;

    // --- BYTE 4: Entire Display ON (0xA5) ---
    while(!txisEmpty()) { if(nackFlagDetected()) { goto handle_nack; } }
    I2C_TXDR = 0xA5;

    // --- BYTE 5: Wake Display Up (0xAF) ---
    while(!txisEmpty()) { if(nackFlagDetected()) { goto handle_nack; } }
    I2C_TXDR = 0xAF;

    // 5. Wait for Auto-End to generate the STOP condition safely
    while(!stopFlagDetected());
    I2C_ICR |= (1 << 5); // Clear stop flag
    return;

handle_nack:
    I2C_CR2 |= (1 << 14); // Generate software STOP
    I2C_ICR = (1 << 4);   // Clear NACK flag
}

//special toggleDisplay using DMA
uint8_t* toggleDisplayDMA(){
	//clear cr2 config
	I2C_CR2 &= ~((1 << 25) | (1 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

	//Set NBYTES = 5, Write Mode (0<<10), Address (0x3C << 1), and AUTOEND = 1 (1 << 25)
	I2C_CR2 |= (5 << 16) | (0 << 10) | (0x3C << 1) | (1 << 25);

	static uint8_t commands[5] = {0x00, 0x8D, 0x14, 0xA5, 0xAF};



	//returning commands and not &commands since commands already acts as a pointer to the first object in array
	return commands;
}

// Helper function to send a single independent command
void sendOLEDCommand(uint8_t cmd) {
    I2C_ICR |= (1 << 5) | (1 << 4) | (1 << 8); // Clear flags
    I2C_CR2 &= ~((1 << 25) | (1 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

    // NBYTES = 2, Address = 0x3C, AUTOEND = 1
    I2C_CR2 = (2 << 16) | (0x3C << 1) | (1 << 25);
    I2C_CR2 |= (1 << 13); // START

    // Byte 1: Control Byte (0x00 means next byte is a command)
    while(!txisEmpty());
    I2C_TXDR = 0x00;

    // Byte 2: The actual command
    while(!txisEmpty());
    I2C_TXDR = cmd;

    while(!stopFlagDetected());
    I2C_ICR |= (1 << 5); // Clear STOP
}

// Call them sequentially in your test function:
void testDisplayOn() {
    sendOLEDCommand(0x8D); // Setup Charge Pump
    sendOLEDCommand(0x14); // Enable Charge Pump
    sendOLEDCommand(0xA5); // Entire Display ON
    sendOLEDCommand(0xAF); // Display ON Panel
}


//return 1 if Busy
int checkBusyRegister(){
	if(I2C_ISR & (1 << 15)){
		return 1;
	}
	return 0;
}

int transferComplete(){
	if(I2C_ISR & (1 << 6)){
		return 1;
	}
	return 0;
}

//STOPF triggered
int stopFlagDetected(){
	if(I2C_ISR & (1 << 5)){
		return 1;
	}
	return 0;
}

//NACK received flag
int nackFlagDetected(){
	if(I2C_ISR & (1 << 4)){
		return 1;
	}
	return 0;
}

//RX not empty; 1 for empty, 0 for filled
int rxNotEmpty(){
	if(I2C_ISR & (1 << 2)){
		return 1;
	}
	return 0;
}

//TX empty
int txEmpty(){
	if(I2C_ISR & (1 << 0)){
		return 1;
	}
	return 0;
}


//transmit interrupt status
//is 1 if TXDR is empty, ready for next byte or end
int txisEmpty(){
	if(I2C_ISR & (1 << 1)){
		return 1;
	}
	return 0;
}

//incase of bus problems
int busError(){
	if(I2C_ISR & (1 << 8)){
		return 1;
	}
	return 0;
}





