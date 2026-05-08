/*
 * uartEx.c
 *
 *  Created on: 6 May 2026
 *      Author: tooka
 *      bare metal UART implementation in C
 */

#include "uartEx.h"

//address definitions

//base address
#define GPIOA_BASE_ADDR 0x48000000UL
#define RCC_AHB1_BASE_ADDR 0x40021000UL //APB2 sources from AHB1, refer to clock tree
#define APB2_USART1		0x40013800UL // Base address for USART in APB2 Bus



#define GPIOA_MODER 	(*((volatile uint32_t *)(GPIOA_BASE_ADDR + 0x000UL))) //literally the first part range
#define GPIOA_AFRH 		(*((volatile uint32_t *)(GPIOA_BASE_ADDR + 0x24UL))) //Alternate function Register High

#define RCC_APB2_ENR	(*((volatile uint32_t *)(RCC_AHB1_BASE_ADDR + 0x60UL))) //enable clock for APB2 bus
#define RCC_AHB2_ENR	(*((volatile uint32_t *)(RCC_AHB1_BASE_ADDR + 0x4CUL))) //enable clock for AHB2 bus

#define USART_CR1	(*((volatile uint32_t *)(APB2_USART1))) //enable clock for AHB2 bus
#define USART_BRR	(*((volatile uint32_t *)(APB2_USART1 + 0x0CUL))) //Baud Rate Register
#define USART_CR2	(*((volatile uint32_t *)(APB2_USART1 + 0x04UL))) //set stop bit

//addresses for ISR (Interrupt status register, where flags are kept), and TDR (Transmit Data Register, for transmitting data)
#define USART_ISR    (*((volatile uint32_t *)(APB2_USART1 + 0x1CUL)))
#define USART_TDR    (*((volatile uint32_t *)(APB2_USART1 + 0x28UL))) //max size is 1 byte (8 bits, 256 combinations)


//this function configures pins PA9 and PA10 for USART1 TX and RX respectively
void uartPinConfig(){

	//Reset Clock and control, enable the clocks, without them nothing works, so they come first!
	RCC_APB2_ENR |= (1 << 14); //Enable APB2 peripheral clock enable, for USART1, bit 14
	RCC_AHB2_ENR |= (1 << 0); // enable GPIOA peripheral clock within the AHB2 bus, bit 0


	//clear before setting, just in case
	GPIOA_MODER &= ~(3 << 18);  // clear PA9
	GPIOA_MODER &= ~(3 << 20);  // clear PA10
	GPIOA_AFRH &= ~(0xF << 4);  // clear PA9 AF bits
	GPIOA_AFRH &= ~(0xF << 8);  // clear PA10 AF bits


	//set PA9 and 10 to AF mode
	GPIOA_MODER |= (2 << 18); // 10; AF for bits 19:18 for pin 9
	GPIOA_MODER |= (2 << 20); // 10; AF for bits 21:20 for pin 10

	//specify AF mode as AF7 (UART)
	GPIOA_AFRH |= (7 << 4); // 0111 for AF7 between bits 7:4 pin 9
	GPIOA_AFRH |= (7 << 8); // 0111 for AF7 between bits 11:8, pin 10



	//USART config
	USART_CR1 &= ~(1 << 28); // 00, combination for 1 start bit, 8 data bit, force bit to be zero
	USART_CR1 &= ~(1 << 12);
	USART_CR1 &= ~(1 << 15); //Oversampling x16, meaning it'll sample an incoming bit 16 times, before accepting the value. Hence vey low chance of error


	// Baud Rate: 9600 @ 170MHz
	// Formula: 170,000,000 / 9,600 = 17708.33, round off
	USART_BRR = 17708; // to get baud rate of 9600 from clock of 170,000,000
	USART_CR2 &= ~(3 << 12); // Force bits 12 and 13 to 0 for 1 stop bit

	USART_CR1 |= (1 << 3); // Enable Transmitter
	USART_CR1 |= (1 << 2); // Enable Receiver

	USART_CR1 |= (1 << 0); //UE: USART ENABLE.
}




void UARTSendChar(char c) {
    // 1. Wait for TXE bit (Bit 7) to become 1
    // This means the "Outbox" is empty and ready for your byte
	//without {} this is a null statement, meaning that as long as true, it literally does nothing, preventing movement
    while (!(USART_ISR & (1 << 7)));

    // 2. Write the character to the Transmit Data Register
    USART_TDR = c;
}

void UARTSendString(char* str) {
    while (*str) {
        UARTSendChar(*str++);
    }
}

