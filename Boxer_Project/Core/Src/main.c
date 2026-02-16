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
#include "adc.h"
#include "rng.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "ILI9341/ILI9341_Touchscreen.h"
#include "ILI9341/ILI9341_STM32_Driver.h"
#include "ILI9341/ILI9341_GFX.h"
#include "ILI9341/snow_tiger.h"

#include "stm32f7xx_hal.h"
#include "ILI9341/5x5_font.h"

#include "stdio.h"
#include "string.h"

#include "picture.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SCREEN_W 240
#define SCREEN_H 320

#define P1_COLOR 0xf980
#define P2_COLOR 0x023f


#define BUZZER_PORT GPIOF
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
typedef enum { ready = 0, playing = 1, ended = 2} state;
typedef enum { reset = 0, set = 1} flag;

volatile uint16_t p1_controller = 0;
volatile uint16_t p2_controller = 0;

uint32_t timer_count = 0;
uint32_t timer = 0;
uint32_t last_timer = 0;

state game_state = ready;
uint8_t p1_ready = 0;
uint8_t p2_ready = 0;
uint8_t p1_hp = 3;
uint8_t p2_hp = 3;

flag p1_btn_flag = reset;
flag p1_sensor_flag = reset;
uint8_t p1_punch_val = 0;


flag p2_btn_flag = reset;
flag p2_sensor_flag = reset;
uint8_t p2_punch_val = 0;

uint8_t hp_pos_y = 120;
uint8_t hp_size = 30;

uint8_t p_pos_y = 90;
uint8_t p1_pos_x = 30;
uint8_t p2_pos_x = 170;

uint32_t last_blink = 0;
uint8_t animate_timer = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void Draw_Image_From_Data(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t* image_data)
{
    uint32_t pixel_index = 0;

    for (uint16_t j = 0; j < height; j++)
    {
        for (uint16_t i = 0; i < width; i++)
        {

            uint16_t color = image_data[pixel_index];

            ILI9341_Draw_Pixel(x + i, y + j, color);

            pixel_index += 1;
        }
    }
}

void draw_p1_hp(uint8_t hp){
	uint16_t x_positions[3] = {30, 63, 96};
	for (uint8_t i = 0; i < 3; i++) {
		if (i < hp) {
			Draw_Image_From_Data(x_positions[i], hp_pos_y, 30, 30, heart_icon);
		} else {
			ILI9341_Draw_Rectangle(x_positions[i], hp_pos_y, hp_size, hp_size, BLACK);
		}
	}
}

void draw_p2_hp(uint8_t hp){
	uint16_t x_positions[3] = {170, 203, 236};
	for (uint8_t i = 0; i < 3; i++) {
		if (i < hp) {
			Draw_Image_From_Data(x_positions[i], hp_pos_y, 30, 30, heart_icon_2);
		} else {
			ILI9341_Draw_Rectangle(x_positions[i], hp_pos_y, hp_size, hp_size, BLACK);
		}
	}
}

void handle_p1_punch(){
	p1_btn_flag = reset;
	p1_punch_val = !p1_punch_val;
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 500 + 2000*p1_punch_val);
	HAL_Delay(50);
}

void handle_p2_punch() {
	p2_btn_flag = reset;
	p2_punch_val = !p2_punch_val;
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 500 + 2000*p2_punch_val);
	HAL_Delay(50);
}

void control_p1_pos(){
	while (HAL_ADC_PollForConversion(&hadc2, 100) != HAL_OK){}
	p1_controller = HAL_ADC_GetValue(&hadc2);

	  uint32_t pulse = 1000 + (p1_controller * 500) / 4096;

	  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, pulse);
}

void control_p2_pos(){
	while (HAL_ADC_PollForConversion(&hadc1, 100) != HAL_OK){}
	p2_controller = HAL_ADC_GetValue(&hadc1);

	  uint32_t pulse = 1500 - (p2_controller * 500) / 4096;

	  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pulse);
}

void handle_p1_got_hit() {
	beep(50);
	HAL_Delay(20);
	beep(50);

    if (p1_hp > 0) (p1_hp)--;
    HAL_Delay(50);
    if (p1_hp == 0) {
        game_state = ended;
    }
    p1_sensor_flag = reset;
}

void handle_p2_got_hit() {
	beep(50);
	HAL_Delay(20);
	beep(50);

    if (p2_hp > 0) (p2_hp)--;
    HAL_Delay(50);
    if (p2_hp == 0) {
        game_state = ended;
    }
    p2_sensor_flag = reset;
}

void display_timer() {
    int time = timer_count / 1000;
    timer = 60 - time%61;

    if (timer != last_timer){
        char timer_str[20];

        int len = sprintf(timer_str, "%-2lu", timer);

        ILI9341_Draw_Text(timer_str, 142, 30, WHITE, 3, BLACK);
        last_timer = timer;
    }
}

void display_result() {
	HAL_Delay(1000);
	beep(50);
	HAL_Delay(100);
	beep(20);
	HAL_Delay(100);
	beep(50);

	for(int i = 0; i < 3; i++){
		if (p1_hp > p2_hp){
			ILI9341_Draw_Text("player 1 wins", 30, 180, P1_COLOR, 3, BLACK);
		}
		else if (p1_hp < p2_hp){
			ILI9341_Draw_Text("player 2 wins", 30, 180, P2_COLOR, 3, BLACK);
		}
		else {
			ILI9341_Draw_Text("TIE", 133, 180, WHITE, 3, BLACK);
		}
		HAL_Delay(500);
		ILI9341_Draw_Text("             ", 30, 180, BLACK, 3, BLACK);
		HAL_Delay(500);

	}
	HAL_Delay(1000);
}

void display_ready_screen(){
	ILI9341_Draw_Text("*BOXING ZONE*", 30, 35, WHITE, 3, BLACK);
	  ILI9341_Draw_Text("Player 1", p1_pos_x, p_pos_y, P1_COLOR, 2, BLACK);
	  ILI9341_Draw_Text("Player 2", p2_pos_x, p_pos_y, P2_COLOR, 2, BLACK);
	  ILI9341_Draw_Text("Press to Start!", 60, 200, WHITE, 2, BLACK);
}

void reset_game(){
  p1_sensor_flag = reset;
  p2_sensor_flag = reset;
	p1_ready = 0;
	p2_ready = 0;
	p1_hp = 3;
	p2_hp = 3;
	timer_count = 0;
	game_state = ready;
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 1250);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 1250);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 1250);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 1250);
}

void display_count_down_screen(){
	ILI9341_Fill_Screen(BLACK);
	ILI9341_Draw_Text("3", 136, 88, WHITE, 8, BLACK);
	beep(50);
	HAL_Delay(1000);

	ILI9341_Draw_Text("2", 136, 88, WHITE, 8, BLACK);
	beep(50);
	HAL_Delay(1000);

	ILI9341_Draw_Text("1", 136, 88, WHITE, 8, BLACK);
	beep(50);
	HAL_Delay(1000);
	beep(300);
	ILI9341_Fill_Screen(BLACK);
}

void handle_start_game(){
	  p1_btn_flag = reset;
	  p2_btn_flag = reset;
	  p1_sensor_flag = reset;
	  p2_sensor_flag = reset;
	  HAL_Delay(1000);
	  display_count_down_screen();
	  game_state = playing;

	  ILI9341_Draw_Text("Player 1", p1_pos_x, p_pos_y, P1_COLOR, 2, BLACK);
	  ILI9341_Draw_Text("Player 2", p2_pos_x, p_pos_y, P2_COLOR, 2, BLACK);
	  draw_p1_hp(p1_hp);
	  draw_p2_hp(p2_hp);
}

void display_press2start() {
    if (HAL_GetTick() - last_blink >= 500) {
        last_blink = HAL_GetTick();
        animate_timer = !animate_timer;
    }

    if (animate_timer) {
        ILI9341_Draw_Text("Press to Start!", 60, 200, WHITE, 2, BLACK);
    } else {
        ILI9341_Draw_Rectangle(60, 200, 180, 16, BLACK);
    }
}

void buzzer_on() {
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_Pin, GPIO_PIN_SET);
}

void buzzer_off() {
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_Pin, GPIO_PIN_RESET);
}

void beep(uint16_t duration_ms) {
	buzzer_on();
    HAL_Delay(duration_ms);
	buzzer_off();
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
  MX_USART3_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_SPI5_Init();
  MX_TIM1_Init();
  MX_RNG_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
//  start ADC for player controllers
  HAL_ADC_Start(&hadc1);
  HAL_ADC_Start(&hadc2);

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
// start pwm for servo
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

//  LCD Init
  ILI9341_Init();
  MX_TIM2_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//  LCD
  ILI9341_Fill_Screen(BLACK);
  ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);

//  set default servo direction
//  htim2.Instance -> CCR3 = 1250;
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 1250);
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 1250);

//  timer interrupt
  HAL_TIM_Base_Start_IT (&htim3);

  display_ready_screen();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  switch(game_state){
	  case ready:
		  display_press2start();
		  if (p1_ready == 1){
			  if (!p1_btn_flag) {
				  ILI9341_Draw_Text("Ready!", 50, 120, WHITE, 2, BLACK);
				  p1_btn_flag = set;
				  beep(50);
			  }
		  }
		  if (p2_ready == 1){
			  if (!p2_btn_flag) {
				  ILI9341_Draw_Text("Ready!", 180, 120, WHITE, 2, BLACK);
				  p2_btn_flag = set;
				  beep(50);
				  HAL_Delay(20);
				  beep(50);
			  }
		  }
		  if (p1_ready == 1 && p2_ready == 1){
			  handle_start_game();
		  }
		  break;

	  case playing:
		  control_p1_pos();
		  control_p2_pos();

		  display_timer();

		  if(timer == 0){
			  HAL_UART_Transmit(&huart3, (uint8_t*)"END", strlen("END"), 1000);
			  game_state= ended;
			  continue;
		  }

		  if (p1_btn_flag){
			  handle_p1_punch();
			  HAL_UART_Transmit(&huart3, (uint8_t*)"p1 punch\r\n", strlen("p1 punch\r\n"), 100);

			}

		  if (p2_btn_flag){
			  handle_p2_punch();
			  HAL_UART_Transmit(&huart3, (uint8_t*)"p2 punch\r\n", strlen("p2 punch\r\n"), 100);
		  }

		  if (p1_sensor_flag){
			  handle_p1_got_hit();
			  draw_p1_hp(p1_hp);
			  HAL_UART_Transmit(&huart3, (uint8_t*)"p1 got hit\r\n", strlen("p1 got hit\r\n"), 1000);
		  }

		  if (p2_sensor_flag){
			  handle_p2_got_hit();
			  draw_p2_hp(p2_hp);
			  HAL_UART_Transmit(&huart3, (uint8_t*)"p2 got hit\r\n", strlen("p2 got hit\r\n"), 1000);
		  }
		  break;

	  case ended:
		  display_result();
		  reset_game();
		  ILI9341_Fill_Screen(BLACK);
		  display_ready_screen();
		  break;
	  }
	   HAL_Delay(50);
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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin){
		case P1_SENSOR_1_Pin:
			p1_sensor_flag = set;
			break;

		case P2_SENSOR_1_Pin:
			p2_sensor_flag = set;
			break;

		case P1_SENSOR_2_Pin:
			p1_sensor_flag = set;
			break;

		case P2_SENSOR_2_Pin:
			p2_sensor_flag = set;
			break;

		case P1_BTN_Pin:
			if (game_state == 0){
				p1_ready = 1;

			}
			else{
				p1_btn_flag = set;
			}
			break;

		case P2_BTN_Pin:
			if (game_state == 0){
				p2_ready = 1;
			}
			else{
				p2_btn_flag = set;
			}
			break;

		default:
			break;
	}
}
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
