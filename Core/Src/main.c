/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

COM_InitTypeDef BspCOMInit;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Initialize led */
  BSP_LED_Init(LED_GREEN);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
  BspCOMInit.BaudRate   = 115200;
  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
  BspCOMInit.StopBits   = COM_STOPBITS_1;
  BspCOMInit.Parity     = COM_PARITY_NONE;
  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  //define the port to use by finding it's register location in memory
  //why use define? why not a variable? among the many reasons, is that variables will live in memory,
  //#define is pure text substitution. During preprocessing anything that is defined is literally replaced. No RAM needed.
#define PORTC_OUT (*((volatile uint32_t *)0x48000818)) //this is the hardcoded address for Port Output for group C
#define PORTB_IN (*((volatile uint32_t *)0x48000410)) //this is the hardcoded address for Port input for group B
int count1 = 0;
uint8_t last_state = 0; // Keep track of what the button was doing last time


  //configure the ports; set them as GP input or output


	/*
  while (1) {
      uint8_t current_state = (PORTB_IN & (1 << 1)) ? 1 : 0;

      //edge detection: we are trying to detect rising edge which is 0 -> 1
      if (current_state == 1 && last_state == 0) {
          count++;
          HAL_Delay(100); // Debounce: Ignore the "bounces" for 50ms
      }

      last_state = current_state; // Update the memory for the next loop

      //LED logic
      if (count % 2 == 0) {
          PORTC_OUT = (1 << 9); //alter the bit using a bitmask, specifically targeting the 9th bit from right

          //GPIOC->BSRR = (1 << 9); //this is how we do it using the HAL
      } else {
          //PORTC_OUT = (1 << (9 + 16));

    	  //this is the HAL implementation
    	  //the HAL already includes the address calculation by slowly adding up all the relevant base addresses with relevant offsets
          GPIOC->BSRR = (1 << (9 + 16));
          in short, GPIOC if defined as using the address for GPIOC_BASE, itself an altered address for AHB2PERIPH_BASE, and so on
           * GPIOC is defined and is type casted as a (GPIO_TypeDef *), which is basically a "pointer of struct GPIO_TypeDef "
           * a feature of structs is that it's elements are always stored in order within memory. so if each element is 4 bytes,
           * you can get whatever element you want by skipping the memory by AxB bytes. where A is the size, B is the slot
           * BSRR is an element within the GPIO_TypeDef struct, it is the 7th element (index 6 since we count from 0)
           * so, to get the location of memory of BSRR for GPIOC, logically it would be GPIOC address + BSRR location,
           * which is like GPIOC + 6(4) bytes = 0x48000800 + 0x18.
           * that's how it works! it is a fundamental feature of C using pointers and structs.
      	  }
  	  } */

		#define GPIOC_BASE_ADDR	(0x48000800UL)
		#define GPIOC_ODR 	 (*((volatile uint32_t *) 0x48000814))
		#define GPIOC_BSRR		(*((volatile uint32_t *)(GPIOC_BASE_ADDR + 0x18UL)))
		#define TIM_2_BASE_ADDR		(*((volatile uint32_t *) 0x40000000)) //TIM2 base address
		#define TIM_3_BASE_ADDR		(0x40000400UL)
#define TIM3_CCR3     (*((volatile uint32_t *)(TIM_3_BASE_ADDR + 0x3CUL))) // Duty cycle for Ch3


	clockTest();

	PWM_Test();
	TIM3_CCR3 = 100;
	while(1){


		uint8_t current_state = (PORTB_IN & (1 << 1)) ? 1 : 0;





		if (current_state == 1 && last_state == 0) {
		    count1++;
		    HAL_Delay(100); // Debounce: Ignore the "bounces" for 50ms
		}
		last_state = current_state;

		//this logic is for the button, not seperated because fuck you
		if(count1 % 2 == 0){
			TIM_2_BASE_ADDR &= ~(1 << 0);
		} else {
			TIM_2_BASE_ADDR |= (1 << 0);
		}
	}

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */







// this is the system clock config, aka config how fast the system can think
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */


  //this is using the HAL, which involves preseting a struct and then giving the struct as a parameter

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI; //use the HSI oscillator
  RCC_OscInitStruct.HSIState = RCC_HSI_ON; //turn on HSI
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON; //Turn on PLL
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC8
   *
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

   USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {

  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
