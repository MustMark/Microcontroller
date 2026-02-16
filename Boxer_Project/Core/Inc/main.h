/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define T_IRQ_Pin GPIO_PIN_2
#define T_IRQ_GPIO_Port GPIOE
#define T_CLK_Pin GPIO_PIN_3
#define T_CLK_GPIO_Port GPIOE
#define T_MISO_Pin GPIO_PIN_4
#define T_MISO_GPIO_Port GPIOE
#define T_MOSI_Pin GPIO_PIN_5
#define T_MOSI_GPIO_Port GPIOE
#define User_Blue_Button_Pin GPIO_PIN_13
#define User_Blue_Button_GPIO_Port GPIOC
#define P2_CONTROLLER_Pin GPIO_PIN_0
#define P2_CONTROLLER_GPIO_Port GPIOC
#define P1_CONTROLLER_Pin GPIO_PIN_3
#define P1_CONTROLLER_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_13
#define BUZZER_GPIO_Port GPIOF
#define P2_SENSOR_1_Pin GPIO_PIN_14
#define P2_SENSOR_1_GPIO_Port GPIOF
#define P2_SENSOR_1_EXTI_IRQn EXTI15_10_IRQn
#define P2_BTN_Pin GPIO_PIN_15
#define P2_BTN_GPIO_Port GPIOF
#define P2_BTN_EXTI_IRQn EXTI15_10_IRQn
#define P1_SENSOR_1_Pin GPIO_PIN_11
#define P1_SENSOR_1_GPIO_Port GPIOE
#define P1_SENSOR_1_EXTI_IRQn EXTI15_10_IRQn
#define P1_BTN_Pin GPIO_PIN_13
#define P1_BTN_GPIO_Port GPIOE
#define P1_BTN_EXTI_IRQn EXTI15_10_IRQn
#define CS_Pin GPIO_PIN_8
#define CS_GPIO_Port GPIOC
#define DC_Pin GPIO_PIN_9
#define DC_GPIO_Port GPIOC
#define RST_Pin GPIO_PIN_10
#define RST_GPIO_Port GPIOC
#define P1_SENSOR_2_Pin GPIO_PIN_8
#define P1_SENSOR_2_GPIO_Port GPIOB
#define P1_SENSOR_2_EXTI_IRQn EXTI9_5_IRQn
#define P2_SENSOR_2_Pin GPIO_PIN_9
#define P2_SENSOR_2_GPIO_Port GPIOB
#define P2_SENSOR_2_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
