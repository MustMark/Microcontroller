/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
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

/* USER CODE BEGIN PV */
// 2.
uint8_t pwm;

//3.
//uint8_t red_pwm;
//uint8_t green_pwm;
//uint8_t blue_pwm;
//
//float red = 0;
//float green = 0;
//float blue = 0;
//
//char rx_cmd;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

//void adjust_duty_cycle(float *duty_cycle){
//	if(*duty_cycle >= 1.0){
//		*duty_cycle = 0;
//	}
//	else{
//		*duty_cycle += 0.2;
//	}
//
//}
//
//void display_duty_cycles(float red, float green, float blue){
//    char divider[40] = "______________________________________\r\n";
//	char message[30];
//	int len = sprintf(message, "| R %3d%% | G %3d%% | B %3d%% |\n\r",
//	                  (int)(red * 100),
//	                  (int)(green * 100),
//	                  (int)(blue * 100));
//
//	while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
//	HAL_UART_Transmit(&huart3, message, len, 100);
//
//    while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
//    HAL_UART_Transmit(&huart3, (uint8_t*)divider, 40, 100);
//}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if (huart->Instance == USART3)
//	{
//		switch (rx_cmd) {
//			case 'r':
//				adjust_duty_cycle(&red);
//				break;
//			case 'g':
//				adjust_duty_cycle(&green);
//				break;
//			case 'b':
//				adjust_duty_cycle(&blue);
//				break;
//			default:
//				break;
//		}
//
//		htim2.Instance -> CCR3 = (10000 - 1) * red;
//		htim1.Instance -> CCR4 = (10000 - 1) * green;
//		htim4.Instance -> CCR1 = (10000 - 1) * blue;
//
//		display_duty_cycles(red, green, blue);
//
//		// Restart receive for next character
////	    const char *msg = "press: ";
////	    HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), 100);
//
//		HAL_UART_Receive_IT(&huart3, (uint8_t*)&rx_cmd, 1);
//	}
//}
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

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

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
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
//  2.
//  float dutyCycle = 0.1;

//  uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim2);
//  HAL_Delay(100);

//  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
//  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
//  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
//
//  const char *msg = "press: ";
//  HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
//
//  HAL_UART_Receive_IT(&huart3, &rx_cmd, 1);

//  	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
//	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
//	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
//
//    HAL_UART_Receive_IT(&huart3, (uint8_t*) &rx_cmd, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

//		red_pwm   = (GPIOB->IDR & GPIO_PIN_10) >> 10;
//		green_pwm = (GPIOE->IDR & GPIO_PIN_14) >> 14;
//		blue_pwm  = (GPIOD->IDR & GPIO_PIN_12) >> 12;
//
//		HAL_Delay(100);
//	  2.
//	  htim2.Instance -> CCR3 = (20000-1)* dutyCycle;
	  htim2.Instance->CCMR2 |= TIM_CCMR2_OC3M_1;
	  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	  HAL_Delay(1000);
	  HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);

	  htim2.Instance->CCMR2 |= TIM_CCMR2_OC3M_2;
	  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	  HAL_Delay(1000);
	  HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
//	  pwm = (GPIOB->IDR & GPIO_PIN_10) >> 10;

//	  3.
//	  char cmd;
//
//	  const char *msg = "press: ";
//	  HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), 100);
//
//	  while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_RXNE)== RESET){}
//	  HAL_UART_Receive(&huart3, (uint8_t*) &cmd, 1, 100);
//
//	  HAL_UART_Transmit(&huart3, (uint8_t*)&cmd, 1, 100);
//	  HAL_UART_Transmit(&huart3, (uint8_t*)"\r\n", 2, 100);
//
//
//	  switch(cmd){
//	  	  case 'r':
//	  		adjust_duty_cycle(&red);
//
//	  		break;
//	  	  case 'g':
//	  		adjust_duty_cycle(&green);
//
//	  		break;
//	  	  case 'b':
//	  		adjust_duty_cycle(&blue);
//			break;
//	  	  default:
//	  		  continue;
//	  }
//
//	  htim2.Instance -> CCR3 = (10000 - 1) * red;
//	  htim1.Instance -> CCR4 = (10000 - 1) * green;
//	  htim4.Instance -> CCR1 = (10000 - 1) * blue;
//
//	  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
//	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
//	  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
//
//	  HAL_Delay(100);
//
//	  HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
//	  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
//	  HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
//
//	  display_duty_cycles(red, green, blue);
//
//	  red_pwm = (GPIOB->IDR & GPIO_PIN_10) >> 10;
//	  green_pwm = (GPIOE->IDR & GPIO_PIN_14) >> 14;
//	  blue_pwm = (GPIOD->IDR & GPIO_PIN_12) >> 12;
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

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
