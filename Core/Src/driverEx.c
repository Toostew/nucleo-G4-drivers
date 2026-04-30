/*
 * driverEx.c
 *
 *  Created on: 30 Apr 2026
 *      Author: tooka
 */

#include <driverEx.h>

//this function will run automatically upon interrupt request from TIM2, after being approved by NVIC.
//This function follows VERY specific nomenclature, outlined in Core/Startup/startup_stm32g474retx.s .
//it's purpose is to reset the UIF flag back to 0.
//Every time an overflow occurs, the status register UIF bit is set to 1.
int count = 0;
void TIM2_IRQHandler(void) {
	#define TIM2_SR (*((volatile uint32_t *)(0x40000000 + 0x010UL))) //TIM2 Base address + TIM2_SR offset
	#define PORTC_ODR (*((volatile uint32_t *)0x48000814))

    TIM2_SR &= ~(1 << 0);  // clear UIF — acknowledge the interrupt
    PORTC_ODR ^= (1 << 9);
    count++;
    Opposite_Flash();
}

void Opposite_Flash(){
	#define PORTC_BRR (*((volatile uint32_t *)0x48000818))
	if(count % 2 == 0){
		//if timer is even number; turn off
		PORTC_BRR = (1 << 8);
	} else {
		//if not even; turn on
		PORTC_BRR = (1 << (8 + 16));
	}
}


void clockTest(){
    #define RCC_BASE_ADDR   	0x40021000UL               // Just a number
    #define RCC_APB1ENR1    	(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x58UL))) //add offset

	#define TIM_2_BASE_ADDR		(0x40000000) //TIM2 base address
	#define TIM2_PRESCALER		(*((volatile uint32_t *)(TIM_2_BASE_ADDR + 0x028UL)))
	#define TIM2_ARR		(*((volatile uint32_t *)(TIM_2_BASE_ADDR + 0x02CUL))) //auto reload register
	#define TIM2_DIER		(*((volatile uint32_t *)(TIM_2_BASE_ADDR + 0x00CUL))) //DMA/Interrupt enable register
	#define TIM2_CR1		(*((volatile uint32_t *)(TIM_2_BASE_ADDR))) //TIM2 Control Register


	#define NVIC_BASE_ADDR	0xE000E000 //base address for NVIC, found in ARM Cortex-M4's reference manual, not STM32
	#define NVIC_IPR		((volatile uint8_t  *)(NVIC_BASE_ADDR + 0x400UL)) //Interrupt priority register, 8 bit
	#define NVIC_ISER0		(*((volatile uint32_t *)(NVIC_BASE_ADDR + 0x100UL))) //Interrupt Set-Enable Registers, 32 bit
	#define TIM2_IRQN		28

	//STEP 1: enable the clocks so that these timer registers can count. Tim2 (the timer register we wish to use) is under APB1 bus
    RCC_APB1ENR1 |= (1 << 0); //set the first bit at this address to 1.

    //STEP 2: prescale, this number will divide the sysclock so that it'll be slower. Note that by default it's x + 1
    TIM2_PRESCALER = 16999; // by the default the value is x + 1

    //at this point, sysclk has been scaled down from 170,000,000 to 10,000. We need to scale it down even more (goal is 1)
    //STEP 3: Auto Reload Register, if the timer reaches this value, it automatically resets to start value. also x + 1
    TIM2_ARR = 9999; // default is x + 1

    //STEP 4: Enable Update Interrupt, when overflow occurs, sends an Interrupt request to Nested Vectored Interrupt Controller (NVIC)
    //It's basically a configuration that makes any overflow on TIM2 causes an interrupt
    TIM2_DIER |= (1 << 0); //set first bit at this address to 1


    //STEP 5: set up Nested Vectored Interrupt Controller (NVIC), which is basically the controller for interrupts
    NVIC_IPR[TIM2_IRQN] |= (1 << 4); //set the priority for TIM2 (28th IRQN number) to 1. only the front nibble is used so need to shift it

	NVIC_ISER0 |= (1 << TIM2_IRQN); //enable TIM2 Interrupts to be responded by NVIC, turn on the 28th bit (bit corresponds to TIM2)


	//STEP 6: Finally, turn on the register we want to use (TIM2)
	TIM2_CR1 |= (1 << 0); //flip the first bit on, enables the counter if set to 1
}


