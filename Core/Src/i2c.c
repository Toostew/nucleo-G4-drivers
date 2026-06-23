/*
 * i2c.c
 *	implements I2C in bare metal for GY-521
 *  Created on: 13 May 2026
 *      Author: tooka
 */
#include "i2c.h"


//we will be using PB7 and PB8 for I2C1_SDA and SCL respectively

#define I2C1_BASE_ADDR		0x40005400
#define I2C4_BASE_ADDR		0x40008400
#define RCC_BASE_ADDR 		0x40021000
#define GPIOB_BASE_ADDR		0x48000400
#define GPIOC_BASE_ADDR		0x48000800
#define MPU6050_SLAVE_ADDR	0x68
#define BME280_SLAVE_ADDR	0x76


#define RCC_APB1ENR 		(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x58UL)))	//for APB1 clock enable; for I2C1
#define RCC_APB1ENR2		(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x5CUL)))	//for I2C4 clock enable
#define RCC_AHB2ENR			(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x4CUL)))	//for GPIOB
#define RCC_CCIPR			(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x88UL)))	//for I2C1 clock source config
#define RCC_CCIPR2			(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x9CUL)))	//for I2C4 clock source config




//I2C1
#define I2C1_CR1				(*((volatile uint32_t *)(I2C1_BASE_ADDR))) //CR1 has no offset
#define I2C1_CR2				(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x04UL)))
#define I2C1_TIMINGR			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x10UL)))
#define I2C1_ISR				(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x18UL)))
#define I2C1_ICR				(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x1CUL)))
#define I2C1_RXDR			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x24UL)))
#define I2C1_TXDR			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x28UL)))

//I2C4
#define I2C4_CR1				(*((volatile uint32_t *)(I2C4_BASE_ADDR)))
#define I2C4_CR2				(*((volatile uint32_t *)(I2C4_BASE_ADDR + 0x04UL)))
#define I2C4_TIMINGR			(*((volatile uint32_t *)(I2C4_BASE_ADDR + 0x10UL)))
#define I2C4_ISR				(*((volatile uint32_t *)(I2C4_BASE_ADDR + 0x18UL)))
#define I2C4_ICR				(*((volatile uint32_t *)(I2C4_BASE_ADDR + 0x1CUL)))
#define I2C4_RXDR			(*((volatile uint32_t *)(I2C4_BASE_ADDR + 0x24UL)))
#define I2C4_TXDR			(*((volatile uint32_t *)(I2C4_BASE_ADDR + 0x28UL)))

//GPIOB
#define GPIOB_MODER			(((volatile uint32_t *)(GPIOB_BASE_ADDR)))
#define GPIOB_OTYPER		(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x04UL)))
#define GPIOB_OSPEEDR		(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x08UL)))
#define GPIOB_PUPDR			(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x0CUL)))
#define GPIOB_AFRL			(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x20UL)))
#define GPIOB_AFRH			(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x24UL)))

//GPIOC
#define GPIOC_MODER			(*((volatile uint32_t *)(GPIOC_BASE_ADDR)))
#define GPIOC_OTYPER		(*((volatile uint32_t *)(GPIOC_BASE_ADDR + 0x04UL)))
#define GPIOC_OSPEEDR		(*((volatile uint32_t *)(GPIOC_BASE_ADDR + 0x08UL)))
#define GPIOC_PUPDR			(*((volatile uint32_t *)(GPIOC_BASE_ADDR + 0x0CUL)))
#define GPIOC_AFRL			(*((volatile uint32_t *)(GPIOC_BASE_ADDR + 0x20UL))) //PC6 and PC7


//configuration function for I2C1 and I2C4
//I2C1 is
void I2C_Configuration(){

	//clock config
	//enable clocks
	RCC_APB1ENR |= (1 << 21);
	RCC_APB1ENR2 |= (1 << 1); //turn on I2C4 DUMBASS!!!!!!!!!!!!!!!!!
	RCC_AHB2ENR |= (1 << 1);

	//change source clock for I2C1 to use HSI, 16 MHZ. makes my life easier
	RCC_CCIPR &= ~(3 << 12);
	RCC_CCIPR |= (2 << 12); //select HSI for clock source, 16 MHz

	//change source clock for I2C4 to use HSI 16 MHz
	RCC_CCIPR2	&= ~(0b11 << 0);
	RCC_CCIPR2	|= (0b10 << 0); //HSI mode, 0b10 or 2


	//GPIOC config
	GPIOC_MODER	&= ~(3 << 12); //PC6
	GPIOC_MODER	&= ~(3 << 14); //PC7
	GPIOC_MODER	|= (0b10 << 12); // AF
	GPIOC_MODER |= (0b10 << 14);

	GPIOC_OTYPER |= (1 << 6); //PC6
	GPIOC_OTYPER |= (1 << 7);

	GPIOC_OSPEEDR &= ~(3 << 12); //PC6
	GPIOC_OSPEEDR &= ~(3 << 14); //PC7
	GPIOC_OSPEEDR |= (2 << 12); //High speed
	GPIOC_OSPEEDR |= (2 << 14);

	GPIOC_PUPDR	 &= ~(3 << 12); //PC6
	GPIOC_PUPDR	 &= ~(3 << 14); //PC7
	GPIOC_PUPDR	 |= (1 << 12); //pull-down
	GPIOC_PUPDR	 |= (1 << 14);

	GPIOC_AFRL	&= ~(0b1111 << 24); //PC6
	GPIOC_AFRL	&= ~(0b1111 << 28); //PC7
	GPIOC_AFRL	|= (0b1000 << 24); //AF8 I2C4_SCL
	GPIOC_AFRL	|= (0b1000 << 28); //AF8 I2C4_SDA

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
	GPIOB_AFRL	|= (4 << 28); //pin B7
	GPIOB_AFRH	&= ~(15 << 0);
	GPIOB_AFRH	|= (4 << 0); //pin B8


	//I2C1 Configuration
	//disable I2C1 just in case
	I2C1_CR1 &= ~((1 << 0) | (1 << 14));

	I2C1_TIMINGR = 0x00000000; //reset values to default

	//these are the specific configs to get the clock to
	//in order, PRESCALER, SCL Low oeriod, SCL High period,  Data setup time, Data hold time
	//target is 100KHz
	I2C1_TIMINGR |= ((0x3 << 28) | (0x13 << 0) | (0xF << 8) | (0x4 << 20) | (0x2 << 16));

	//TXDMAEN enabled, turn on DMA transmussuib requests
	I2C1_CR1 |= (1 << 14); // disable if not using DMA

	//enable I2C
	I2C1_CR1 |= (1 << 0);


	//I2C4
	//disable I2C4 just in case
	I2C4_CR1 &= ~((1 << 0) | (1 << 14));

	I2C4_TIMINGR = 0x00000000; //reset values to default
	//these are the specific configs to get the clock to
	//in order, PRESCALER, SCL Low oeriod, SCL High period,  Data setup time, Data hold time
	//we copy exactly with I2C1 since we target 100KHz
	I2C4_TIMINGR |= ((0x3 << 28) | (0x13 << 0) | (0xF << 8) | (0x4 << 20) | (0x2 << 16));

	//TXDMAEN enabled, turn on DMA transmussuib requests
	I2C4_CR1 |= (1 << 14); // disable if not using DMA

	//enable I2C
	I2C4_CR1 |= (1 << 0);


}

//generic read operator
uint32_t I2C_read(uint8_t slaveAddress, uint8_t targetRegister, int device){

	//device number dictates operation
	//MPU6050
	if(device == 0){
		//clear flags
		I2C4_ICR |= (1 << 5) | (1 << 4) | (1 << 8);

		//first message for read
		//CR2 setup
		//clear AUTOEND, NBYTES, STOP, START, ADD10, RD_WRN, and SADD
		I2C4_CR2 &= ~((1 << 25) | (0b11111111 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

		//set number of bytes, START, write mode, and target address
		I2C4_CR2 = ((1 << 16)  | (0 << 10) | (((uint32_t)(slaveAddress)) << 1));

		I2C4_CR2 |= (1 << 13); //enable start bit

		while(!I2C4_TxIsEmpty()){
			//during transmission there could be a chance nothing comes back, NACK is flagged if nothing responds
			if(I2C4_NackFlagDetected()){

				//generate a stop to end the transmission early
				I2C4_CR2 = (1 << 14);

				//clear nack flag
				I2C4_ICR = (1 << 4);

				return 0xFFFFFFFF;
			}
		} //wait for transmit data register is empty

		I2C4_TXDR = (uint32_t)targetRegister;

		while(!I2C4_TransferComplete());

		//listen phase
		//number of bytes, start and target address dont need to change, but rewrite for redundancy
		//set start bit LAST
		I2C4_CR2 &= ~((1 << 25) | (0b11111111 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

		//bit 10 = 1, read operation.
		I2C4_CR2 = ((1 << 25) | (1 << 16) | (0 << 11) | (1 << 10) | (((uint32_t)(slaveAddress)) << 1));

		I2C4_CR2 |= (1 << 13); //enable start bit

		while(!I2C4_RXNotEmpty()); //hold until RX is empty
		uint32_t data = I2C4_RXDR; //the register is already 32 bits no need to type cast


		//use stop flags as more like milestones and not actual conditionals to dictate logics directly
		while(!I2C4_StopFlagDetected());

		I2C4_ICR |= (1 << 5);


		return data;


	}
	//BME280
	else if(device == 1){

		//clear Bus Error, NACK, STOP flag registers, these are event flags and are sticky, need to reset manually
		I2C1_ICR |= (1 << 5) | (1 << 4) | (1 << 8);


		//first message for read
		//CR2 setup
		//clear AUTOEND, NBYTES, STOP, START, ADD10, RD_WRN,
		I2C1_CR2 &= ~((1 << 25) | (0b11111111 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

		//set number of bytes, START, write mode, and target address
		I2C1_CR2 = ((1 << 16)  | (0 << 10) | (((uint32_t)(BME280_SLAVE_ADDR)) << 1));

		I2C1_CR2 |= (1 << 13); //enable start bit

		while(!I2C1_TxIsEmpty()){
			//during transmission there could be a chance nothing comes back, NACK is flagged if nothing responds
			if(I2C1_NackFlagDetected()){

				//generate a stop to end the transmission early
				I2C1_CR2 = (1 << 14);

				//clear nack flag
				I2C1_ICR = (1 << 4);

				return 0xFFFFFFFF;
			}
		} //wait for transmit data register is empty

		I2C1_TXDR = (uint32_t)(targetRegister);

		while(!I2C1_TransferComplete());

		//listen phase
		//number of bytes, start and target address dont need to change, but rewrite for redundancy
		//set start bit LAST
		I2C1_CR2 &= ~((1 << 25) | (0b11111111 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

		I2C1_CR2 = ((1 << 25) | (1 << 16) | (0 << 11) | (1 << 10) | (((uint32_t)(BME280_SLAVE_ADDR)) << 1));

		I2C1_CR2 |= (1 << 13); //enable start bit

		while(!I2C1_RXNotEmpty()); //hold until RX is empty
		uint32_t data = I2C1_RXDR;


		//use stop flags as more like milestones and not actual conditionals to dictate logics directly
		while(!I2C1_StopFlagDetected());

		I2C1_ICR |= (1 << 5);


		return data;





	}
	return 0xFFFFFFFF; //undefined

}


//we will read the WHOAMI address on the BME280
//slave address for BME is 0x76, WHOAMI = 0x68
uint32_t bmeTest(){

	//clear Bus Error, NACK, STOP flag registers, these are event flags and are sticky, need to reset manually
	I2C1_ICR |= (1 << 5) | (1 << 4) | (1 << 8);


	//first message for read
	//CR2 setup
	//clear AUTOEND, NBYTES, STOP, START, ADD10, RD_WRN,
	I2C1_CR2 &= ~((1 << 25) | (0b11111111 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

	//set number of bytes, START, write mode, and target address
	I2C1_CR2 = ((1 << 16)  | (0 << 10) | (((uint32_t)(BME280_SLAVE_ADDR)) << 1));

	I2C1_CR2 |= (1 << 13); //enable start bit

	while(!I2C1_TxIsEmpty()){
		//during transmission there could be a chance nothing comes back, NACK is flagged if nothing responds
		if(I2C1_NackFlagDetected()){

			//generate a stop to end the transmission early
			I2C1_CR2 = (1 << 14);

			//clear nack flag
			I2C1_ICR = (1 << 4);

			return 0xFFFFFFFF;
		}
	} //wait for transmit data register is empty

	I2C1_TXDR = 0xD0;

	while(!I2C1_TransferComplete());

	//listen phase
	//number of bytes, start and target address dont need to change, but rewrite for redundancy
	//set start bit LAST
	I2C1_CR2 &= ~((1 << 25) | (0b11111111 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

	I2C1_CR2 = ((1 << 25) | (1 << 16) | (0 << 11) | (1 << 10) | (((uint32_t)(BME280_SLAVE_ADDR)) << 1));

	I2C1_CR2 |= (1 << 13); //enable start bit

	while(!I2C1_RXNotEmpty()); //hold until RX is empty
	uint32_t data = I2C1_RXDR;


	//use stop flags as more like milestones and not actual conditionals to dictate logics directly
	while(!I2C1_StopFlagDetected());

	I2C1_ICR |= (1 << 5);


	return data;
}

//we will read the WHOAMI address on the MPU6050
//slave address for BME is 0x76, WHOAMI = 0x68
uint32_t mpuTest(){

	//clear Bus Error, NACK, STOP flag registers, these are event flags and are sticky, need to reset manually
	I2C4_ICR |= (1 << 5) | (1 << 4) | (1 << 8);


	//first message for read
	//CR2 setup
	//clear AUTOEND, NBYTES, STOP, START, ADD10, RD_WRN, and SADD
	I2C4_CR2 &= ~((1 << 25) | (0b11111111 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

	//set number of bytes, START, write mode, and target address
	I2C4_CR2 = ((1 << 16)  | (0 << 10) | (((uint32_t)(MPU6050_SLAVE_ADDR)) << 1));

	I2C4_CR2 |= (1 << 13); //enable start bit

	while(!I2C4_TxIsEmpty()){
		//during transmission there could be a chance nothing comes back, NACK is flagged if nothing responds
		if(I2C4_NackFlagDetected()){

			//generate a stop to end the transmission early
			I2C4_CR2 = (1 << 14);

			//clear nack flag
			I2C4_ICR = (1 << 4);

			return 0xFFFFFFFF;
		}
	} //wait for transmit data register is empty

	I2C4_TXDR = 0x75;

	while(!I2C4_TransferComplete());

	//listen phase
	//number of bytes, start and target address dont need to change, but rewrite for redundancy
	//set start bit LAST
	I2C4_CR2 &= ~((1 << 25) | (1 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

	I2C4_CR2 = ((1 << 25) | (1 << 16) | (0 << 11) | (1 << 10) | (((uint32_t)(MPU6050_SLAVE_ADDR)) << 1));

	I2C4_CR2 |= (1 << 13); //enable start bit

	while(!I2C4_RXNotEmpty()); //hold until RX is empty
	uint32_t data = I2C4_RXDR;


	//use stop flags as more like milestones and not actual conditionals to dictate logics directly
	while(!I2C4_StopFlagDetected());

	I2C4_ICR |= (1 << 5);


	return data;
}


//initial setup for the MPU6050, including wake up, init and such
//the register order is power > config(gyro/accel) > read data registers
//RUN THIS AFTER i2C INIT BUT BEFORE ACTUALLY USING I2C
void mpuSetup(){
	//turn on the MPU by setting the SLEEP register to 0 at 0x6B
	//TODO: abstract writing operations into a single function bruh you can't keep writing ts


	I2C4_ICR |= (1 << 5) | (1 << 4) | (1 << 8);

	//clear AUTOEND, NBYTES, STOP, START, ADD10, RD_WRN, and SADD
	I2C4_CR2 &= ~((1 << 25) | (0b11111111 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

	//set number of bytes, START, write mode, and target address
	I2C4_CR2 = ((2 << 16)  | (0 << 10) | (((uint32_t)(MPU6050_SLAVE_ADDR)) << 1));

	I2C4_CR2 |= (1 << 13); //enable start bit

	while(!I2C4_TxIsEmpty()){
			//during transmission there could be a chance nothing comes back, NACK is flagged if nothing responds
			if(I2C4_NackFlagDetected()){

				//generate a stop to end the transmission early
				I2C4_CR2 = (1 << 14);

				//clear nack flag
				I2C4_ICR = (1 << 4);

				return;
			}
	} //wait for transmit data register is empty

	I2C4_TXDR = 0x6B; //Address of power management 1 register

	while(!I2C4_TxIsEmpty()){
			//during transmission there could be a chance nothing comes back, NACK is flagged if nothing responds
			if(I2C4_NackFlagDetected()){

				//generate a stop to end the transmission early
				I2C4_CR2 = (1 << 14);

				//clear nack flag
				I2C4_ICR = (1 << 4);

				return;
			}
	} //wait for transmit data register is empty
	I2C4_TXDR = 0b00000001; //turn off SLEEP at bit 6, set CLKSEL to mode 1 at bit 2:0

	//first AUTOENDs here
	//resend another frame
	//TODO:abstract ts pls
	I2C4_ICR |= (1 << 5) | (1 << 4) | (1 << 8);

	//clear AUTOEND, NBYTES, STOP, START, ADD10, RD_WRN, and SADD
	I2C4_CR2 &= ~((1 << 25) | (0b11111111 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

	//set number of bytes, START, write mode, and target address
	I2C4_CR2 = ((2 << 16)  | (0 << 10) | (((uint32_t)(MPU6050_SLAVE_ADDR)) << 1));

	I2C4_CR2 |= (1 << 13); //enable start bit

	while(!I2C4_TxIsEmpty()){
			//during transmission there could be a chance nothing comes back, NACK is flagged if nothing responds
			if(I2C4_NackFlagDetected()){

				//generate a stop to end the transmission early
				I2C4_CR2 = (1 << 14);

				//clear nack flag
				I2C4_ICR = (1 << 4);

				return;
			}
	} //wait for transmit data register is empty

	I2C4_TXDR = 0x1B; //Address of GYRO config

	while(!I2C4_TxIsEmpty()){
			//during transmission there could be a chance nothing comes back, NACK is flagged if nothing responds
			if(I2C4_NackFlagDetected()){

				//generate a stop to end the transmission early
				I2C4_CR2 = (1 << 14);

				//clear nack flag
				I2C4_ICR = (1 << 4);

				return;
			}
	} //wait for transmit data register is empty
	I2C4_TXDR = 0b00000001; //turn off SLEEP at bit 6, set CLKSEL to mode 1 at bit 2:0


}


//refer to datasheet of GM009605
void toggleDisplay(){
    // 1. Clear sticky event flags
    I2C1_ICR |= (1 << 5) | (1 << 4) | (1 << 8);

    // 2. Clear old CR2 configurations
    I2C1_CR2 &= ~((1 << 25) | (1 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

    // 3. Set NBYTES = 5, Write Mode (0<<10), Address (0x3C << 1), and AUTOEND = 1 (1 << 25)
    I2C1_CR2 = (5 << 16) | (0 << 10) | (0x3C << 1) | (1 << 25);

    // 4. Fire the START bit
    I2C1_CR2 |= (1 << 13);

    // first byte is control byte, 0x00
    while(!I2C1_TxIsEmpty()) { if(I2C1_NackFlagDetected()) { goto handle_nack; } }
    I2C1_TXDR = 0x00;

    //second byte is CHARGE PUMP, 0x8D
    while(!I2C1_TxIsEmpty()) { if(I2C1_NackFlagDetected()) { goto handle_nack; } }
    I2C1_TXDR = 0x8D;

    //third byte is enable charge pump, 0x14
    while(!I2C1_TxIsEmpty()) { if(I2C1_NackFlagDetected()) { goto handle_nack; } }
    I2C1_TXDR = 0x14;

    //fourth byte is turn on whole screen, 0xA5
    while(!I2C1_TxIsEmpty()) { if(I2C1_NackFlagDetected()) { goto handle_nack; } }
    I2C1_TXDR = 0xA5;

    //5th byte is all pixels on, 0xAF
    while(!I2C1_TxIsEmpty()) { if(I2C1_NackFlagDetected()) { goto handle_nack; } }
    I2C1_TXDR = 0xAF;

    //Wait for Auto-End to generate the STOP condition safely
    while(!I2C1_StopFlagDetected());
    I2C1_ICR |= (1 << 5); // Clear stop flag
    return;

handle_nack:
    I2C1_CR2 |= (1 << 14); // Generate software STOP
    I2C1_ICR = (1 << 4);   // Clear NACK flag
}

//special toggleDisplay using DMA
//deprecated, everything is consolidated in dmaEx/transmitOLED
void displayOLEDDMAConfig(uint8_t numberOfBytes){
	//clear cr2 config
	I2C1_CR2 &= ~((1 << 25) | (1 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

	//Set NBYTES = 5, Write Mode (0<<10), Address (0x3C << 1), and AUTOEND = 1 (1 << 25)
	I2C1_CR2 |= (numberOfBytes << 16) | (0 << 10) | (0x3C << 1) | (1 << 25);


}

// Helper function to send a single independent command
void sendOLEDCommand(uint8_t cmd) {
    I2C1_ICR |= (1 << 5) | (1 << 4) | (1 << 8); // Clear flags
    I2C1_CR2 &= ~((1 << 25) | (1 << 16) | (1 << 14) | (1 << 13) | (1 << 11) | (1 << 10));

    // NBYTES = 2, Address = 0x3C, AUTOEND = 1
    I2C1_CR2 = (2 << 16) | (0x3C << 1) | (1 << 25);
    I2C1_CR2 |= (1 << 13); // START

    // Byte 1: Control Byte (0x00 means next byte is a command)
    while(!I2C1_TxIsEmpty());
    I2C1_TXDR = 0x00;

    // Byte 2: The actual command
    while(!I2C1_TxIsEmpty());
    I2C1_TXDR = cmd;

    while(!I2C1_StopFlagDetected());
    I2C1_ICR |= (1 << 5); // Clear STOP
}

// Call them sequentially in your test function:
void testDisplayOn() {
    sendOLEDCommand(0x8D); // Setup Charge Pump
    sendOLEDCommand(0x14); // Enable Charge Pump
    sendOLEDCommand(0xA5); // Entire Display ON
    sendOLEDCommand(0xAF); // Display ON Panel
}


//return 1 if Busy
int I2C1_CheckBusyRegister(){
	if(I2C1_ISR & (1 << 15)){
		return 1;
	}
	return 0;
}

int I2C1_TransferComplete(){
	if(I2C1_ISR & (1 << 6)){
		return 1;
	}
	return 0;
}

//STOPF triggered
int I2C1_StopFlagDetected(){
	if(I2C1_ISR & (1 << 5)){
		return 1;
	}
	return 0;
}

//NACK received flag
int I2C1_NackFlagDetected(){
	if(I2C1_ISR & (1 << 4)){
		return 1;
	}
	return 0;
}

//RX not empty; 1 for empty, 0 for filled
int I2C1_RXNotEmpty(){
	if(I2C1_ISR & (1 << 2)){
		return 1;
	}
	return 0;
}



//transmit interrupt status
//is 1 if TXDR is empty, ready for next byte or end
int I2C1_TxIsEmpty(){
	if(I2C1_ISR & (1 << 1)){
		return 1;
	}
	return 0;
}

//I2C4 variants
//return 1 if Busy
int I2C4_CheckBusyRegister(){
	if(I2C4_ISR & (1 << 15)){
		return 1;
	}
	return 0;
}

int I2C4_TransferComplete(){
	if(I2C4_ISR & (1 << 6)){
		return 1;
	}
	return 0;
}

//STOPF triggered
int I2C4_StopFlagDetected(){
	if(I2C4_ISR & (1 << 5)){
		return 1;
	}
	return 0;
}

//NACK received flag
int I2C4_NackFlagDetected(){
	if(I2C4_ISR & (1 << 4)){
		return 1;
	}
	return 0;
}

//RX not empty; 1 for empty, 0 for filled
int I2C4_RXNotEmpty(){
	if(I2C4_ISR & (1 << 2)){
		return 1;
	}
	return 0;
}



//transmit interrupt status
//is 1 if TXDR is empty, ready for next byte or end
int I2C4_TxIsEmpty(){
	if(I2C4_ISR & (1 << 1)){
		return 1;
	}
	return 0;
}


//incase of bus problems
int busError(){
	if(I2C1_ISR & (1 << 8)){
		return 1;
	}
	return 0;
}





