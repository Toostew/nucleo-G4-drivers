/*
 * freeRTOSEx.c
 *
 *  Created on: 5 Jun 2026
 *      Author: tooka
 */


#include <freeRTOSQueueTest.h>



//setup for RTOS including setup
void rtos_task_setup(){


	QueueHandle_t xUARTQueue = xQueueCreate(64, sizeof(char *));

	    //The base struct is found within the uartEx header file
	    static UARTParameters UARTParameter_Config;

		UARTParameter_Config.UARTQueue = xUARTQueue;



	  xTaskCreate(
			  UARTsendData,
			  "UART_SEND_DATA", //name of task
			  128,	//size of stack in words
			  (void *)&UARTParameter_Config, //we are expecting a void pointer so we type def it
			  1,
			  NULL);
}


void toggleLED1(void * pvParameters){
	//same as while(1)
	for(;;){
		togglePinPB1();
		vTaskDelay(500); //delay 500 ticks, 1 tick = 1 ms
	}

}

void toggleLED2(void * pvParameters){
	UARTParameters *parameter_struct = (UARTParameters *)pvParameters;

	//the message is actually just an array of chars in memory
	char * message;
	while(1){
		if(togglePinPB2()){
			message = "LED2 ON";
			xQueueSend(parameter_struct->UARTQueue, &message, portMAX_DELAY);
		} else {
			message = "LED2 OFF"; //the message is a string, but message stores only the pointer to the first char
			xQueueSend(parameter_struct->UARTQueue, &message, portMAX_DELAY);

		}
		vTaskDelay(333);
	}

}

void UARTsendData(void * pvParameters){
	UARTParameters *parameter_struct = (UARTParameters *)pvParameters;

	char * message;
	while(1){
		//only runs when the queue is populated
		xQueueReceive(parameter_struct->UARTQueue, &message, portMAX_DELAY);

		UARTSendString(message);
	}
}




