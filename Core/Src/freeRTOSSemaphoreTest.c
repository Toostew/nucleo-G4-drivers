/*
 * freeRTOSSemaphoreTest.c
 *
 *  Created on: 7 Jun 2026
 *      Author: tooka
 */


//this file implements semaphores, also used to config External Interrupt (EXTI)
#include "freeRTOSSemaphoreTest.h"


#define GPIOB_BASE_ADDR		0x48000400UL
#define RCC_BASE_ADDR		0x40021000UL
#define SYSCFG_BASE_ADDR	0x40010000UL
#define EXTI_BASE_ADDR		0x40010400UL
#define NVIC_BASE_ADDR		0xE000E000UL //base address for NVIC, found in ARM Cortex-M4's reference manual, not STM32


#define GPIOB_MODER 	(*((volatile uint32_t *)(GPIOB_BASE_ADDR)))
#define SYSCFG_EXTICR4	(*((volatile uint32_t *)(SYSCFG_BASE_ADDR + 0x14UL))) //syscfg exti 4, for PB 15
#define EXTI_IMR1		(*((volatile uint32_t *)(EXTI_BASE_ADDR)))
#define EXTI_RTSR1		(*((volatile uint32_t *)(EXTI_BASE_ADDR + 0x08UL)))
#define EXTI_PR1		(*((volatile uint32_t *)(EXTI_BASE_ADDR + 0x14UL)))


#define NVIC_IPR			((volatile uint8_t  *)(NVIC_BASE_ADDR + 0x400UL)) //Interrupt priority register, 8 bit
#define NVIC_ISER1			(*((volatile uint32_t *)(NVIC_BASE_ADDR + 0x104UL))) //Interrupt Set-Enable Registers, 32 bit
#define EXTI15_IRQN			40



#define RCC_AHB2_ENR	(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x4CUL))) //enable clock for AHB2 bus


static SemaphoreHandle_t buttonSemaphore;



//pin configs for use in the semaphore demo
void pinConfig(){

	//turn on AHB2
	RCC_AHB2_ENR &= ~(1 << 1); //clear GPIOB at bit 1
	RCC_AHB2_ENR |= (1 << 1); // GPIOB enabled for IO

	//set PB15 to input
	GPIOB_MODER &= ~(0b11 << 30);

	//Enable exti on PB15
	SYSCFG_EXTICR4 &= ~(0xF << 12);    // Clear bits 15:12 safely
	SYSCFG_EXTICR4 |= (0b0001 << 12);


	//External Interrupt config
	EXTI_RTSR1 |= (1 << 15);
	EXTI_IMR1 |= (1 << 15); //enable EXTI line 15 for GPIO

	// 5. Configure NVIC Priority (Your byte-pointer trick works great here!)
	NVIC_IPR[EXTI15_IRQN] = (6 << 4);  // Sets Priority to 1 (using upper nibble)

	// 6. Enable Interrupt 40 inside ISER1 (Bit 8)
	NVIC_ISER1 |= (1 << (EXTI15_IRQN - 32));
}

//create semaphore
void semaphoreConfig(){

	buttonSemaphore = xSemaphoreCreateBinary();

	xTaskCreate(
			vButtonHandlerTask, //the function that implements this task
			"button_handler", //the name of this task
			128, // stack depth, or size of stack in words, so 128 x 4 bytes per word, that's 512 bytes
			NULL,
			1, //task priority, higher priority will allow it to cut in line for CPU time and maintain it
			NULL); //reference handle

}

//this funciton follows a specific name nomenclature.
void EXTI15_10_IRQHandler() {
    //check pending flag
    if (EXTI_PR1 & (1 << 15)) {

        // 2. Clear the hardware pending flag (Write 1 to clear)
        EXTI_PR1 = (1 << 15);

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // 3. Safely unlock the software task from the hardware domain
        xSemaphoreGiveFromISR(buttonSemaphore, &xHigherPriorityTaskWoken);

        // 4. Force your RTOS scheduler to instantly switch contexts if needed
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void vButtonHandlerTask(void *pvParameters) {
    while(1) {
        // Blocks indefinitely until the ISR "gives" the semaphore
        xSemaphoreTake(buttonSemaphore, portMAX_DELAY);

        UARTSendString("button pressed");

    }
}
