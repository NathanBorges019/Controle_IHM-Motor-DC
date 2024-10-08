 /* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include <stdio.h>
#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#define EEPROM_SIZE		256
#define PAGE_SIZE		16
#define EEPROM_ADDRESS	0xA0

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
uint8_t menu, start;
uint8_t inc, dec, enter, aux_init;
uint8_t aux_menu, aux_start, accel_time, set_ok;
uint8_t aux_accel, aux_running, aux_decel, config_finish;
uint8_t a_inc, r_inc, d_inc, inc_running, inc_decel;
uint8_t config_init, start_init, pwm;
uint8_t pwm_preset, aux_pwmPreset, motor;
uint8_t eeWrite, eeRead;


char buffer_accel [15] = {0x00};
char buffer_running [15] = {0x00};
char buffer_decel [15] = {0x00};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
void Buzzer_Teclas();
void ENTER_Boucing();
void DEC_Boucing();
void INC_Boucing();
void start_preset();
void start_config();
void sys_init();
void set_converter();
void converter_preset();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void EE_Write(uint8_t devAddress, uint8_t memAddress, uint8_t data){
  		HAL_StatusTypeDef Error;

  		if(memAddress >= 128){
  			devAddress |= 0x2;
  			memAddress -= 128;
  		}
  		Error =  HAL_I2C_Mem_Write(&hi2c1, devAddress, memAddress, 1, &data, 1, 100);
  		if(Error != HAL_OK){
  			Error_Handler();
  		}
  	}
  	uint8_t EE_Read(uint8_t devAddress, uint16_t memAddress){
  		HAL_StatusTypeDef Error;
  		uint8_t data;

  		if(memAddress >= 128){
  			devAddress |= 0x2;
  			memAddress -= 128;
  		}
  		Error =  HAL_I2C_Mem_Read(&hi2c1, devAddress, memAddress, 1, &data, 1, 100);
  		if(Error != HAL_OK){
  			Error_Handler();
  		}
  		return data;
  	}
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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
   sys_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  	   if ((start == 1) && (HAL_GPIO_ReadPin(ENTER_GPIO_Port, ENTER_Pin)))
	  		{
	  			ENTER_Boucing();
	  			Buzzer_Teclas();
	  			start = 3;
	  			converter_preset();
	  		}

	  		if (HAL_GPIO_ReadPin(INC_GPIO_Port, INC_Pin) && (start == 1))
	  		{
	  			INC_Boucing();
	  			Buzzer_Teclas();
	  			LCD_Clear();
	  			menu = 1;
	  			start = 0;
	  		}

	  		if ((menu == 1) && (HAL_GPIO_ReadPin(DEC_GPIO_Port, DEC_Pin)))
	  		{
	  			DEC_Boucing();
	  			Buzzer_Teclas();
	  			LCD_Clear();
	  			menu = 0;
	  			start = 2;
	  		}

	  		if ((aux_start == 1) && (HAL_GPIO_ReadPin(ENTER_GPIO_Port, ENTER_Pin)))
	  		{
	  			ENTER_Boucing();
	  			Buzzer_Teclas();
	  			start = 3;
	  			aux_start = 0;
	  			converter_preset();
	  		}

	  		if ((aux_menu == 1) && (HAL_GPIO_ReadPin(ENTER_GPIO_Port, ENTER_Pin)))
	  		{
	  			ENTER_Boucing();
	  			Buzzer_Teclas();
	  			LCD_Clear();
	  			menu = 2;
	  			aux_menu = 0;
	  			a_inc = 5;
	  			sprintf(buffer_accel, "%d", a_inc);
	  		}

	  		if ((aux_accel == 1) && (HAL_GPIO_ReadPin(INC_GPIO_Port, INC_Pin) && (a_inc <= 80)))
	  		{
	  			aux_menu =0;
	  			INC_Boucing();
	  			Buzzer_Teclas();
	  			LCD_Clear();
	  			a_inc += 5;
	  			sprintf(buffer_accel, "%d", a_inc);
	  			menu = 3;
	  		}
	  		else if (((aux_accel == 1) && (HAL_GPIO_ReadPin(DEC_GPIO_Port, DEC_Pin) && (a_inc <= 80) && (a_inc >5))))
	  		{
	  			aux_menu = 0;
	  			DEC_Boucing();
	  			Buzzer_Teclas();
	  			LCD_Clear();
	  			a_inc -= 5;
	  			sprintf(buffer_accel, "%d", a_inc);
	  			menu = 3;
	  		}

	  		if ((aux_running == 1) && (HAL_GPIO_ReadPin(ENTER_GPIO_Port, ENTER_Pin)))
	  		{
	  			a_inc = a_inc;
	  			//uint8_t memAddressAccel = a_inc;
	  			//EE_Write(EEPROM_ADDRESS, memAddressAccel, a_inc);
	  			aux_accel = 0;
	  			ENTER_Boucing();
	  			Buzzer_Teclas();
	  			LCD_Clear();
	  			menu = 4;
	  			r_inc = 5;
	  			sprintf(buffer_running, "%d", r_inc);
	  		}

	  		if ((inc_running == 1) && (HAL_GPIO_ReadPin(INC_GPIO_Port, INC_Pin) && (r_inc <= 180)))
	  		{
	  			aux_running = 0;
	  			INC_Boucing();
	  			Buzzer_Teclas();
	  			LCD_Clear();
	  			r_inc += 5;
	  			sprintf(buffer_running, "%d", r_inc);
	  			menu = 5;
	  		}
	  		else if ((inc_running == 1) && (HAL_GPIO_ReadPin(DEC_GPIO_Port, DEC_Pin) && (r_inc <= 180) && (r_inc > 5)))
	  		{
	  			aux_running = 0;
	  			DEC_Boucing();
	  			Buzzer_Teclas();
	  			LCD_Clear();
	  			r_inc -= 5;
	  			sprintf(buffer_running, "%d", r_inc);
	  			menu = 5;
	  		}

	  		if ((aux_decel == 1) && (HAL_GPIO_ReadPin(ENTER_GPIO_Port, ENTER_Pin)))
	  		{
	  			r_inc = r_inc;
	  			//uint8_t memAddressRun = r_inc;
	  			//EE_Write(EEPROM_ADDRESS, memAddressRun, r_inc);
	  			inc_running = 0;
	  			ENTER_Boucing();
	  			Buzzer_Teclas();
	  			LCD_Clear();
	  			menu = 6;
	  			d_inc = 5;
	  			sprintf(buffer_decel, "%d", d_inc);
	  		}

	  		if ((inc_decel == 1) && (HAL_GPIO_ReadPin(INC_GPIO_Port, INC_Pin) && (d_inc <= 80)))
	  		{
	  			aux_decel = 0;
	  			INC_Boucing();
	  			Buzzer_Teclas();
	  			LCD_Clear();
	  			d_inc += 5;
	  			sprintf(buffer_decel, "%d", d_inc);
	  			menu = 7;
	  		}
	  		else if ((inc_decel == 1) && (HAL_GPIO_ReadPin(DEC_GPIO_Port, DEC_Pin) && (d_inc <= 80) && (d_inc > 5)))
	  		{
	  			aux_decel = 0;
	  			DEC_Boucing();
	  			Buzzer_Teclas();
	  			LCD_Clear();
	  			d_inc -= 5;
	  			sprintf(buffer_decel, "%d", d_inc);
	  			menu = 7;
	  		}

	  		if ((config_finish == 1) && (HAL_GPIO_ReadPin(ENTER_GPIO_Port, ENTER_Pin)))
	  		{
	  			d_inc = d_inc;
	  			//uint8_t memAddress = 0x02;
	  			//EE_Write(EEPROM_ADDRESS, memAddress, d_inc);
	  			inc_decel = 0;
	  			ENTER_Boucing();
	  			Buzzer_Teclas();
	  			LCD_Clear();
	  			start = 4;
	  			menu = 0;
	  		}

	  		if ((HAL_GPIO_ReadPin(ENTER_GPIO_Port, ENTER_Pin)) && (config_init = 1))
	  		{
	  			ENTER_Boucing();
	  			Buzzer_Teclas();
	  			LCD_Clear();
	  			set_converter();
	  			start = 5;
	  		}

	  		switch (menu)
	  		{
	  		case 1:
	  			LCD_Cursor(0, 2);
	  			LCD_String("DC MOTOR SYS");
	  			LCD_Cursor(1, 0);
	  			LCD_String("<     MENU    ");
	  			aux_menu = 1;
	  			break;

	  		case 2:
	  			LCD_Cursor(0, 0);
	  			LCD_String("   ACCEL TIME   ");
	  			LCD_Cursor(1, 7);
	  			LCD_String(buffer_accel);
	  			LCD_Cursor(1, 8);
	  			LCD_String("s");
	  			aux_accel = 1;
	  			break;

	  		case 3:
	  			LCD_Cursor(0, 0);
	  			LCD_String("   ACCEL TIME   ");
	  			LCD_Cursor(1, 6);
	  			LCD_String(buffer_accel);
	  			LCD_Cursor(1, 8);
	  			LCD_String("s");
	  			aux_running = 1;
	  			break;

	  		case 4:
	  			LCD_Cursor(0, 0);
	  			LCD_String("  RUNNING TIME  ");
	  			LCD_Cursor(1, 7);
	  			LCD_String(buffer_running);
	  			LCD_Cursor(1, 8);
	  			LCD_String("s");
	  			inc_running = 1;
	  			break;

	  		case 5:
	  			LCD_Cursor(0, 0);
	  			LCD_String("  RUNNING TIME  ");
	  			LCD_Cursor(1, 6);
	  			LCD_String(buffer_running);
	  			LCD_Cursor(1, 9);
	  			LCD_String("s");
	  			aux_decel = 1;
	  			break;

	  		case 6:
	  			LCD_Cursor(0, 0);
	  			LCD_String("   DECEL TIME   ");
	  			LCD_Cursor(1, 7);
	  			LCD_String(buffer_decel);
	  			LCD_Cursor(1, 8);
	  			LCD_String("s");
	  			inc_decel = 1;
	  			break;

	  		case 7:
	  			LCD_Cursor(0, 0);
	  			LCD_String("   DECEL TIME   ");
	  			LCD_Cursor(1, 6);
	  			LCD_String(buffer_decel);
	  			LCD_Cursor(1, 8);
	  			LCD_String("s");
	  			config_finish = 1;
	  			break;
	  		}

	  		switch (start)
	  		{

	  		case 1:
	  			LCD_Cursor(0, 2);
	  			LCD_String("DC MOTOR SYS");
	  			LCD_Cursor(1, 0);
	  			LCD_String("     START     >");
	  			break;

	  		case 2:
	  			LCD_Cursor(0, 2);
	  			LCD_String("DC MOTOR SYS");
	  			LCD_Cursor(1, 0);
	  			LCD_String("     START     >");
	  			aux_start = 1;
	  			break;

	  		case 3:
	  			aux_pwmPreset = 1;
	  			start_preset();
	  			break;

	  		case 4:
	  			LCD_Cursor(0, 2);
	  			LCD_String("DC MOTOR SYS");
	  			LCD_Cursor(1, 0);
	  			LCD_String("     START     >");
	  			config_init = 1;
	  			config_finish = 0;
	  			break;

	  		case 5:
	  			start_config();
	  			break;
	  		}
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  void Buzzer_Teclas()
  	{
  		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
  		HAL_Delay(50);
  		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
  		HAL_Delay(50);
  	}

  	void INC_Boucing()
  	{
  		HAL_Delay(10);
  		while (HAL_GPIO_ReadPin(INC_GPIO_Port, INC_Pin));
  	}
  	void DEC_Boucing()
  	{
  		HAL_Delay(10);
  		while (HAL_GPIO_ReadPin(DEC_GPIO_Port, DEC_Pin));
  	}
  	void ENTER_Boucing()
  	{
  		HAL_Delay(10);
  		while (HAL_GPIO_ReadPin(ENTER_GPIO_Port, ENTER_Pin));
  	}

  	void start_preset()
  	{
  	LCD_Clear();
  	LCD_Cursor(0, 0);
  	LCD_String("STATUS:    ACCEL");
  	LCD_Cursor(1, 0);
  	LCD_String("RPM: 1000");
  	LCD_Cursor(1, 11);
  	LCD_String("t:");
  	LCD_Cursor(1, 13);
  	LCD_String(buffer_accel);
  	LCD_Cursor(1, 15);
  	LCD_String("s");
  	HAL_Delay(a_inc * 1000);
  	LCD_Clear();

  	LCD_Cursor(0, 0);
  	LCD_String("STATUS:  RUNNING");
  	LCD_Cursor(1, 0);
  	LCD_String("RPM: 1000");
  	LCD_Cursor(1, 10);
  	LCD_String("t:");
  	LCD_Cursor(1, 12);
  	LCD_String(buffer_running);
  	LCD_Cursor(1, 15);
  	LCD_String("s");
  	HAL_Delay(r_inc * 1000);
  	LCD_Clear();

  	LCD_Cursor(0, 0);
  	LCD_String("STATUS:    DECEL");
  	LCD_Cursor(1, 0);
  	LCD_String("RPM: 1000");
  	LCD_Cursor(1, 11);
  	LCD_String("t:");
  	LCD_Cursor(1, 13);
  	LCD_String(buffer_decel);
  	LCD_Cursor(1, 15);
  	LCD_String("s");
  	HAL_Delay(d_inc * 1000);
  	LCD_Clear();
  	start = 1;
  	}

  	void start_config()
  	{
  	LCD_Cursor(0, 0);
  	LCD_String("STATUS:    ACCEL");
  	LCD_Cursor(1, 0);
  	LCD_String("RPM: 1000");
  	LCD_Cursor(1, 11);
  	LCD_String("t:");
  	LCD_Cursor(1, 13);
  	LCD_String(buffer_accel);
  	LCD_Cursor(1, 15);
  	LCD_String("s");
  	HAL_Delay(a_inc * 1000);
  	LCD_Clear();

  	LCD_Cursor(0, 0);
  	LCD_String("STATUS:  RUNNING");
  	LCD_Cursor(1, 0);
  	LCD_String("RPM: 1000");
  	LCD_Cursor(1, 10);
  	LCD_String("t:");
  	LCD_Cursor(1, 12);
  	LCD_String(buffer_running);
  	LCD_Cursor(1, 15);
  	LCD_String("s");
  	HAL_Delay(r_inc * 1000);
  	LCD_Clear();

  	LCD_Cursor(0, 0);
  	LCD_String("STATUS:    DECEL");
  	LCD_Cursor(1, 0);
  	LCD_String("RPM: 1000");
  	LCD_Cursor(1, 11);
  	LCD_String("t:");
  	LCD_Cursor(1, 13);
  	LCD_String(buffer_decel);
  	LCD_Cursor(1, 15);
  	LCD_String("s");
  	HAL_Delay(d_inc * 1000);
  	LCD_Clear();
  	start = 4;
  	}

  	void sys_init()
  	{
  	LCD_Init();
  	LCD_Cursor(0, 2);
  	LCD_String("DC MOTOR SYS");
  	LCD_Cursor(1, 2);
  	LCD_String("VERSION: 1.0");
  	HAL_Delay(3000);
  	LCD_Clear();
  	start = 1;
  	}

  	void set_converter()
  	{
  	a_inc = a_inc;
  	sprintf(buffer_accel, "%d", a_inc);
  	r_inc = r_inc;
  	sprintf(buffer_running, "%d", r_inc);
  	d_inc = d_inc;
  	sprintf(buffer_decel, "%d", d_inc);
  	}

  	void converter_preset()
  	{
	a_inc = 10;
	sprintf(buffer_accel, "%d", a_inc);
	r_inc = 120;
	sprintf(buffer_running, "%d", r_inc);
	d_inc = 10;
	sprintf(buffer_decel, "%d", d_inc);

  	}
  /* USER CODE END 3 */
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, EN_Pin|RS_Pin|D4_Pin|D5_Pin
                          |D6_Pin|D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : EN_Pin RS_Pin D4_Pin D5_Pin
                           D6_Pin D7_Pin */
  GPIO_InitStruct.Pin = EN_Pin|RS_Pin|D4_Pin|D5_Pin
                          |D6_Pin|D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BUZZER_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : INC_Pin ENTER_Pin DEC_Pin */
  GPIO_InitStruct.Pin = INC_Pin|ENTER_Pin|DEC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
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

#ifdef  USE_FULL_ASSERT
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
