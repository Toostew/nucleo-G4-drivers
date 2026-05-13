/*
 * spiEx.c
 *
 *  Created on: 8 May 2026
 *      Author: tooka
 */

#include "spiEx.h"
#include "stm32g4xx_hal.h"


//BASE ADDRESSES
#define SPI2_BASE_ADDR 		0x40003800 //base address for SPI2 related registers
#define RCC_BASE_ADDR		0x40021000 //base address for RCC, on the AHB1 bus
#define GPIOB_BASE_ADDR		0x48000400
#define GPIOA_BASE_ADDR		0x48000000

#define WRITE_BIT_HERE		0x7F // 0111 1111; mask every other digit with 1, AND, WRITEs need a 0
#define READ_BIT_HERE		0x80 // 1000 0000; mask every other digit with 0, OR, READS need a 1


							//type cast, then dereference so that we dont have to constantly use * with a pointer to access data
#define RCC_APB1ENR 		(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x58UL)))	//address for APB1 peripheral clock
#define RCC_AHB2ENR 		(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x4CUL)))	//address for AHB2 peripheral clock
#define SPI2_CR1			(*((volatile uint32_t *)(SPI2_BASE_ADDR))) //address for SPI4_CR1
#define	SPI2_CR2			(*((volatile uint32_t *)(SPI2_BASE_ADDR + 0x04))) //address for SPI4_CR1
#define	SPI2_SR				(*((volatile uint32_t *)(SPI2_BASE_ADDR + 0x08))) //address for Status register, flags found here
#define	SPI2_DR				(*((volatile uint32_t *)(SPI2_BASE_ADDR + 0x0C))) //address for Data register, data kept here


#define	GPIOA_MODER			(*((volatile uint32_t *)(GPIOA_BASE_ADDR)))
#define GPIOA_BSRR			(*((volatile uint32_t *)(GPIOA_BASE_ADDR + 0x18)))

#define GPIOB_MODER			(*((volatile uint32_t *)(GPIOB_BASE_ADDR))) //offset 0x00, MODER address
#define GPIOB_AFRH			(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x24UL))) //AFRH (High) for pins 8-15
#define GPIOB_OSPEEDR		(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x08UL))) //Configure IO output speed
#define GPIOB_BSRR			(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x18))) //Bit Set reset
#define GPIOB_IDR			(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x10))) //Input data register
#define GPIOB_PUPDR			(*((volatile uint32_t *)(GPIOB_BASE_ADDR + 0x0C)))

//pin pre-config
void spiPinConfig(){
    // 1. Enable Clocks
    RCC_APB1ENR |= (1 << 14);
    RCC_AHB2ENR |= (1 << 1);

    // 2. Clear only the pins we need (11-15)
    GPIOB_MODER &= ~((3 << 24) | (3 << 26) | (3 << 28) | (3 << 30) | (3 << 22));

    // 3. Set Modes: 11,12=Output(01), 13/14/15=AF(10)
    GPIOB_MODER |=  (1 << 22) | (1 << 24) | (2 << 26) | (2 << 28) | (2 << 30);

    SetCSGPIO(1); //turn it to high so that it knows it's not selected


    // 2. Setup PA5 (User LED) as Output
    GPIOA_MODER &= ~(3 << 10);
    GPIOA_MODER |=  (1 << 10);

    // 4. Set Alternate Function 5 (0101) for pins 13, 14, 15, (clear first dummy)
    GPIOB_AFRH &= ~((0xF << 20) | (0xF << 24) | (0xF << 28));
    GPIOB_AFRH |=  ((5 << 20) | (5 << 24) | (5 << 28));

    GPIOB_OSPEEDR |= (2 << 26) | (2 << 28) | (2 << 30); //set speed to very high

    // 5. Atomic SPI Config: Avoids Mode Faults
    // Bit 9 (SSM), Bit 8 (SSI), Bit 2 (MSTR), Bits 5:3 (BR = f/256)
    // We set these all at once so the SPI engine starts in a valid state.
    //fuck it set CPOL AND CPHA to 1
    SPI2_CR1 = (1 << 9) | (1 << 8) | (1 << 2) | (7 << 3) | (1 << 1) | (1 << 0);

    // 6. FIFO and Data Size
    // DS = 8-bit (0111), FRXTH = 1
    SPI2_CR2 = (7 << 8) | (1 << 12);

    // 7. ENABLE SPI (Always last!)
    SPI2_CR1 |= (1 << 6);

}

//deprecated, for debug
void spiPinConfig_Silent() {
    // 1. Enable Clocks
    RCC_APB1ENR |= (1 << 14); // SPI2
    RCC_AHB2ENR |= (1 << 1);  // GPIOB

    // 2. Set pins to Output Mode FIRST (to prevent the glitch)
    // 11=Power, 12=CS, 13=SCLK, 15=MOSI
    GPIOB_MODER &= ~((3 << 22) | (3 << 24) | (3 << 26) | (3 << 30));
    GPIOB_MODER |=  ((1 << 22) | (1 << 24) | (1 << 26) | (1 << 30));

    // 3. Force the SPI Mode 3 Idle state (Everything HIGH)
    GPIOB_BSRR = (1 << 11) | (1 << 12) | (1 << 13) | (1 << 15);
    HAL_Delay(10);

    // 4. NOW switch 13, 14, 15 to Alternate Function (2)
    GPIOB_MODER &= ~((3 << 26) | (3 << 28) | (3 << 30));
    GPIOB_MODER |=  ((2 << 26) | (2 << 28) | (2 << 30));

    // Enable internal Pull-up on PB14 (SDO/MISO)
    GPIOB_PUPDR &= ~(3 << 28);
    GPIOB_PUPDR |=  (1 << 28);

    // 5. Set AF5 (0101) for PB13, 14, 15
    GPIOB_AFRH &= ~((0xF << 20) | (0xF << 24) | (0xF << 28));
    GPIOB_AFRH |=  ((5 << 20) | (5 << 24) | (5 << 28));

    // 6. Final SPI Config (Same as before)
    SPI2_CR1 = (1 << 9) | (1 << 8) | (1 << 2) | (7 << 3) | (3 << 0);
    SPI2_CR2 = (7 << 8) | (1 << 12);
    SPI2_CR1 |= (1 << 6);
}

//reset all registers to initial values
void SPIPinReset(){
	GPIOB_MODER = 0xFFFFFFFF; //MODER reset
	GPIOB_BSRR = 0x00000000;
	GPIOB_OSPEEDR = 0x00000000;
	GPIOB_AFRH = 0x00000000;
	SPI2_CR1 = 0x0000;
	SPI2_CR2 = 0x0700;

}

//test BME 280, deprecated
void SPITest(){
	// 1. Set PB14 (SDO/MISO) as a standard Output instead of SPI
	GPIOB_MODER &= ~(3 << 28);
	GPIOB_MODER |=  (1 << 28);

	// 2. Try to drive it HIGH
	GPIOB_BSRR = (1 << 14);
	HAL_Delay(1000);

	// 3. Read it back
	int pinState = (GPIOB_IDR & (1 << 14));

	if (pinState == 0) {
	    // If the pin stays 0 even after we told it to be 1,
	    // it is physically shorted to GND on the BME board.
		GPIOA_BSRR = (1 << 5);
	} else {
		GPIOA_BSRR = (1 << 5);
		for(volatile int i=0; i<100000; i++);
		GPIOA_BSRR = (1 << 21); // Turn it back off
	}
}



//checks if the busy flag is one, if yes, return one
int checkBusy(){
	//busy flag on bit 7
	if(SPI2_SR & (1 << 7)){
		return 1;
	}
	return 0;

}

//check TransmitBuffer if Empty TXE
//1 if busy 0 otherwise
int checkTransmitBufferEmpty(){

	if(SPI2_SR & (1 << 1)){
		return 1;
	}
	return 0;
}

//RXNE; Receive Buffer not Empty, for consistency negating this
int checkReceiveBufferEmpty(){

	//if receive buffer bit is 1, then not empty
	if(SPI2_SR & (1 << 0)){
		return 0;
	}
	return 1;
}

//set GPIO pin 12 to either low or high for clock selection
void SetCSGPIO(int mode){

	if(mode >= 1){
		GPIOB_BSRR |= (1 << 12);
	} else {
		GPIOB_BSRR |= (1 << 28);
	}

}

//power on BME; deprecated
void powerBME(int mode){

	if(mode >= 1){
		GPIOB_BSRR |= (1 << 11);
	} else {
		GPIOB_BSRR |= (1 << 27);

	}
}


//simple function to get the ID from the device
uint8_t getID(void) {
    uint8_t data = 0;

    // 1. START SPI TRANSACTION
    // Pull PB12 (CS) LOW
    GPIOB_BSRR = (1 << 28);

    // 2. SEND ADDRESS (0xD0)
    // Wait until the transmit buffer is empty and ready for a new byte
    while(!checkTransmitBufferEmpty());
    *(volatile uint8_t *)&SPI2_DR = 0xD0;

    // Wait until the receive buffer has the shifted-in byte (garbage)
    while(checkReceiveBufferEmpty());
    (void)*(volatile uint8_t *)&SPI2_DR; // Clear garbage from RX FIFO

    // 3. SEND DUMMY (0x00) TO CLOCK OUT DATA
    // Wait for TXE again
    while(!checkTransmitBufferEmpty());
    *(volatile uint8_t *)&SPI2_DR = 0x00;

    // Wait for the actual ID byte to arrive in RXNE
    while(checkReceiveBufferEmpty());
    data = *(volatile uint8_t *)&SPI2_DR;

    // 4. END TRANSACTION
    // Critical: Wait for the Busy flag to clear so we don't
    // pull CS High while the clock is still finishing the last bit.
    while(checkBusy());

    GPIOB_BSRR = (1 << 12); // CS High



    return data;
}


