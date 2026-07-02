/*
 * interrupt.c
 *
 *  Created on: 3 Jul 2026
 *      Author: tooka
 */

#include "interrupt.h"



//this file contains code for Interrupts and the like

//In Arm-Cortex, Interrupts are handled using the NVIC, acting kinda like a traffic cop
//the NVIC listens for interrupts, and when it detects one or more, must decide if action must be taken,
//and who gets priority.
//In STM32, EXTI is used to detect external conditions on GPIOs.



//this function sets up interrupts for use in the sensor array.
//NOTE: this function does not turn on the Peripheral specific Interrupt config bits, that is done inside the appropriate function
void sensorArrayInterruptConfig(){
	/*In order to set up NVIC we need the following:
		- calculate the Interrupt Set-Enable Registers (ISER)
		- calculate the specific bit to flip within ISER, using the vector table
		- calculate the Interrupt Priority Register

		looking at the NVIC table, you might wonder what the Address part means. This is actually the actual address
		in flash memory where the related function for that Interrupt resides. So for example, if an I2C1 interrupt
		fires, the NVIC will go to 0x000000BC, which is where [void I2C1_EV_IRQHandler()] function resides

		speaking of functions, the names of the functions are already pre-written, within startup_stm32g474retx.s,
		follow the naming convention within the startup file to find the appropriate interrupt function
		that function, however you implement it, will run when that interrupt fires


	*/

	//step 1: calculate the array index for ISER
	//the index for the ISER array can be found by: IRQ_Number/32, whole number only





}

