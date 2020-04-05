/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include"lcd.h"
#include"motor.h"
#include"SI1145.h"
#include <stdio.h>
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

 static rgb_lcd DataStruct;
signed long lum_g=0;
signed long lum_d=0;

uint8_t Data_lm_g[256];
uint8_t Data_lm_d[256];

signed long UV_g=0;
signed long UV_d=0;

uint8_t Data_UV_g[256];
uint8_t Data_UV_d[256];

signed long IR_g=0;
signed long IR_d=0;

uint8_t Data_IR_g[256];
uint8_t Data_IR_d[256];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  lcd_init(&hi2c1,&DataStruct);
  DeInit(&hi2c1);
  DeInit(&hi2c2);

  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */


     // calcul de la lumiere visible
	 lum_d=ReadVisible(&hi2c1,0X60);
	 lum_g=ReadVisible(&hi2c2,0x60);



	 /*
	  * calcul de IR et UV
	  UV_g=ReadUV(&hi2c1);
	  UV_d=ReadUV(&hi2c2);
	  IR_g=ReadIR(&hi2c1,0x60);
	  IR_d=ReadIR(&hi2c2,0x60);
	  */

	 // affichage des donées sur le port serie
	  HAL_UART_Transmit(&huart2, (uint8_t *)Data_lm_d, sprintf((char *)Data_lm_d, "d : %ld ",(uint32_t )(lum_d)), 1000);
	  HAL_UART_Transmit(&huart2, (uint8_t *)Data_lm_g, sprintf((char *)Data_lm_g, "g : %ld ",(uint32_t )(lum_g)), 1000);

	/*
	 * affichage des donées IR et UV sur le port serie
	  HAL_UART_Transmit(&huart2, (uint8_t *)Data_UV_g, sprintf((char *)Data_UV_g, "UV_g : %ld \n ",(uint32_t )(UV_g)), 1000);
	  HAL_UART_Transmit(&huart2, (uint8_t *)Data_UV_d, sprintf((char *)Data_UV_d, "UV_d : %ld \n",(uint32_t )(UV_d)), 1000);
	  HAL_UART_Transmit(&huart2, (uint8_t *)Data_IR_g, sprintf((char *)Data_IR_g, "IR_g:%ld ",(uint32_t )(IR_g)), 1000);
	  HAL_UART_Transmit(&huart2, (uint8_t *)Data_IR_d, sprintf((char *)Data_IR_d, "IR_d:%ld",(uint32_t )(IR_d)), 1000);
	  HAL_UART_Transmit(&huart2, (uint8_t *)Data_IR_h, sprintf((char *)Data_IR_h, "IR:%ld ",(uint32_t )(IR_h)), 1000);*/


    /* USER CODE BEGIN 3 */
	  // affichage des donées visual sur LCD
	 	 lcd_clear();
		 lcd_locate(&hi2c1,0,0);
	 	 lcd_print((char *)Data_lm_d);
	 	 lcd_locate(&hi2c1,0,1);
	 	 lcd_print((char *)Data_lm_g);

	 	// comande du moteur pas à pas
		 		if((int)(lum_g)-(int)(lum_d)<-200)
		 		 	 	{
		 		 	 		run_stepm(10);
		 		 	 	 lcd_locate(&hi2c1,10,0);
		 		 	     lcd_print("d");
		 		 	 	}
		 		if((int)(lum_g)-(int)(lum_d)>200)
		 		 	 	{
		 		 	 		 run_stepm(-10);
		 		 	 		lcd_locate(&hi2c1,10,1);
		 		 	 		 lcd_print("g");
		 		 	 	}

	 	 HAL_Delay(1000);

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
