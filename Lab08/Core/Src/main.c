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
#include "i2c.h"
#include "rng.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ILI9341/ILI9341_Touchscreen.h"

#include "ILI9341/ILI9341_STM32_Driver.h"
#include "ILI9341/ILI9341_GFX.h"

#include "ILI9341/snow_tiger.h"

#include "stdio.h"
#include "string.h"

#include "stm32f7xx_hal.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CUSTOM_LIGHT_RED 0xfe79
#define CUSTOM_LIGHT_GREEN 0xe7fb
#define CUSTOM_LIGHT_BLUE 0xdedf

#define CUSTOM_YELLOW 0xfff9
#define CUSTOM_ORANGE 0xf966
#define CUSTOM_BLUE 0x331f
#define CUSTOM_PURPLE 0xb47f
#define CUSTOM_PINK 0xfcdb
#define SCREEN_W 240
#define SCREEN_H 320

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
  float temp = 40.0;
  float humid = 30.0;
  uint8_t step = 0;

  float red = 0.8;
  float green = 0.4;
  float blue = 0.7;

//  render var
  char temp_str[6];
  char humid_str[10];

  char red_str[5];
  char green_str[5];
  char blue_str[5];

  uint8_t gap = 6;

  uint8_t button_r = 22;
  uint8_t button_y = 30;
  uint8_t bar_w = 140;
  uint8_t bar_h = 30;

  uint8_t red_pos;
  uint8_t green_pos;
  uint8_t blue_pos;
  uint8_t x_bar_pos;
  uint8_t offset;

  uint32_t lastTempTime = 0;
  uint32_t lastButtonTime = 0;
  /* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void init_positions(void) {
    red_pos   = 110;
    green_pos = red_pos + button_r*2 + gap;
    blue_pos  = green_pos + button_r*2 + gap;
    x_bar_pos = button_y + button_r + gap;
    offset    = bar_h/2;
}

uint16_t CRC16_2(uint8_t *ptr, uint8_t length){
	uint16_t crc = 0xFFFF;
	uint8_t s = 0x00;

	while (length--) {
		crc ^= *ptr++;
		for(s = 0; s < 8; s++) {
			if((crc & 0x01) != 0) {
				crc >>= 1;
				crc ^= 0xA001;
			} else crc >>= 1;
		}
	}
	return crc;
}

uint16_t RGB_to_HEX(float r, float g, float b)
{
    uint16_t red   = (uint16_t)(r * 31) & 0x1F;   // 5 bits
    uint16_t green = (uint16_t)(g * 63) & 0x3F;   // 6 bits
    uint16_t blue  = (uint16_t)(b * 31) & 0x1F;   // 5 bits

    return (red << 11) | (green << 5) | blue;
}

void update_color(float *color)
{
	if(*color >= 1.0){
		*color = 0;
	}
	else{
		*color += 0.1;
	}
}

void render_screen()
{
	ILI9341_Draw_Text(temp_str, 20, 35, BLACK, 2, WHITE);
	ILI9341_Draw_Text(humid_str, 170, 35, BLACK, 2, WHITE);

	ILI9341_Draw_Filled_Circle(130, 50, 25, RGB_to_HEX(red, green, blue));

	ILI9341_Draw_Filled_Circle(button_y, red_pos, button_r, RED);
	ILI9341_Draw_Filled_Circle(button_y, green_pos, button_r, GREEN);
	ILI9341_Draw_Filled_Circle(button_y, blue_pos, button_r, BLUE);

	ILI9341_Draw_Rectangle(x_bar_pos, red_pos - offset, bar_w, bar_h, CUSTOM_LIGHT_RED);
	ILI9341_Draw_Rectangle(x_bar_pos, green_pos - offset, bar_w, bar_h, CUSTOM_LIGHT_GREEN);
	ILI9341_Draw_Rectangle(x_bar_pos, blue_pos - offset, bar_w, bar_h, CUSTOM_LIGHT_BLUE);

	ILI9341_Draw_Rectangle(x_bar_pos, red_pos - offset, bar_w * red, bar_h, RED);
	ILI9341_Draw_Rectangle(x_bar_pos, green_pos - offset , bar_w * green, bar_h, GREEN);
	ILI9341_Draw_Rectangle(x_bar_pos, blue_pos - offset, bar_w * blue, bar_h, BLUE);

	ILI9341_Draw_Text(red_str, x_bar_pos + bar_w + 10, red_pos - 7, BLACK, 2, WHITE);
	ILI9341_Draw_Text(green_str,  x_bar_pos + bar_w + 10, green_pos - 7, BLACK, 2, WHITE);
	ILI9341_Draw_Text(blue_str,  x_bar_pos + bar_w + 10, blue_pos - 7, BLACK, 2, WHITE);
}

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
  uint8_t cmdBuffer[3] = {0x03, 0x00, 0x04};
  uint8_t dataBuffer[8];
  /* USER CODE END 1 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

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
  MX_RNG_Init();
  MX_SPI5_Init();
  MX_TIM1_Init();
  MX_I2C1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  ILI9341_Init();
  init_positions();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint8_t gap = 6;

  uint8_t button_r = 22;
  uint8_t button_y = 30;
  uint8_t bar_w = 140;
  uint8_t bar_h = 30;

  uint8_t red_pos = 110;
  uint8_t green_pos = red_pos + button_r*2 + gap;
  uint8_t blue_pos = green_pos + button_r*2 + gap;

  uint8_t x_bar_pos = button_y + button_r + gap;

  uint8_t offset = bar_h/2;

  uint32_t lastTempTime = 0;
  uint32_t lastButtonTime = 0;

  ILI9341_Fill_Screen(WHITE);
  ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);

	sprintf(temp_str, "%.1f C", temp);
	sprintf(humid_str, "%.1f %%RH", humid);


sprintf(red_str, "%3u%%", (uint8_t)(red * 100));
sprintf(green_str, "%3u%%", (uint8_t)(green * 100));
sprintf(blue_str, "%3u%%", (uint8_t)(blue * 100));

  render_screen();

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    uint32_t now = HAL_GetTick();

//	Sensor
    if (now - lastTempTime > 5000) {
    	HAL_I2C_Master_Transmit(&hi2c1, 0x5C << 1, NULL, 0, 1000);
//		HAL_Delay(2);
		HAL_I2C_Master_Transmit(&hi2c1, 0x5C<<1, cmdBuffer, 3, 1000);
		HAL_Delay(1);

		HAL_I2C_Master_Receive(&hi2c1, 0x5C<<1, dataBuffer, 8, 1000);

		for (int i = 0; i < 8; i++) {
			char buf[20];
			sprintf(buf, "B[%d]=%02X\r\n", i, dataBuffer[i]);
			HAL_UART_Transmit(&huart3, (uint8_t*)buf, strlen(buf), 100);
		}

//		// ฟังก์ชันการคำนวณ CRC8
//		uint8_t CRC8(uint8_t *data, uint8_t length) {
//		    uint8_t crc = 0xFF;  // เริ่มต้น CRC เป็น 0xFF
//		    for (uint8_t i = 0; i < length; i++) {
//		        crc ^= data[i];  // XOR กับข้อมูลที่ส่ง
//		        for (uint8_t bit = 8; bit > 0; bit--) {  // ทำการวนลูป 8 บิต
//		            if (crc & 0x80) {  // ถ้าบิตที่สูงสุดเป็น 1
//		                crc = (crc << 1) ^ 0x31;  // เลื่อนบิตและ XOR กับ polynomial
//		            } else {
//		                crc <<= 1;  // เลื่อนบิตไปซ้าย
//		            }
//		        }
//		    }
//		    return crc;  // คืนค่า CRC ที่คำนวณได้
//		}
//
//		// คำนวณ CRC8 จากข้อมูลที่ได้รับ (7 ไบต์แรก)
//		uint8_t calculatedCRC = CRC8(dataBuffer, 7);  // คำนวณจาก 7 ไบต์แรก (ไม่รวม checksum)
//
//		// แสดงผล CRC8 ที่คำนวณได้
//		char buf[20];
//		sprintf(buf, "Calculated CRC = %02X\r\n", calculatedCRC);
//		HAL_UART_Transmit(&huart3, (uint8_t*)buf, strlen(buf), 100);


		uint16_t Rcrc = (dataBuffer[6] & 0xFF) | (dataBuffer[7] << 8);

		uint16_t crc16 = CRC16_2(dataBuffer, 6);

		if (Rcrc != crc16) continue;
		uint16_t temperature = ((dataBuffer[4] & 0x7F) << 8) + dataBuffer[5];
		temp = temperature / 10.0;
		if (dataBuffer[4] & 0x80) temp = -temp;

		uint16_t humidity = (dataBuffer[2] << 8) + dataBuffer[3];
		humid = humidity / 10.0;

		sprintf(temp_str, "%.1f C", temp);
		sprintf(humid_str, "%.1f %%RH", humid);

		render_screen();

		lastTempTime = now;
	}

// touch pad
    if (now - lastButtonTime >= 50) {
		if(TP_Touchpad_Pressed()){
			uint16_t x_pos = 0;
			uint16_t y_pos = 0;

			uint16_t position_array[2];

			if(TP_Read_Coordinates(position_array) == TOUCHPAD_DATA_OK)
			{
				x_pos = position_array[0];
				y_pos = position_array[1];
				uint8_t message;

				if (y_pos >= ILI9341_SCREEN_HEIGHT - button_y - button_r && y_pos <= ILI9341_SCREEN_HEIGHT - button_y + button_r){
					if ((x_pos >= red_pos - button_r) &&(x_pos <= red_pos + button_r)) {
						update_color(&red);
						HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);
					}
					else if ((x_pos >= green_pos - button_r) &&(x_pos <= green_pos + button_r)) {
						update_color(&green);
						HAL_GPIO_WritePin(GPIOB, LD1_Pin, GPIO_PIN_SET);
					}
					else if ((x_pos >= blue_pos - button_r) &&(x_pos <= blue_pos + button_r)) {
						update_color(&blue);
						HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_SET);
					}
				}

				sprintf(red_str, "%3u%%", (uint8_t)(red * 100));
				sprintf(green_str, "%3u%%", (uint8_t)(green * 100));
				sprintf(blue_str, "%3u%%", (uint8_t)(blue * 100));

				render_screen();

				HAL_Delay(200);

			}
			HAL_GPIO_WritePin(GPIOB, LD3_Pin | LD2_Pin | LD1_Pin, GPIO_PIN_RESET);
		}
		lastButtonTime = now;
	}
  }  /* USER CODE END 3 */
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
  RCC_OscInitStruct.PLL.PLLQ = 9;
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
