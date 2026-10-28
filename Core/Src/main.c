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
#define VK16K33_ADDR (0x70 << 1)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
static const uint8_t g_number_table[] = {
    0x3F, /* 0 */
	0x06, /* 1 */
	0x5B, /* 2 */
	0x4F, /* 3 */
    0x66, /* 4 */
	0x6D, /* 5 */
	0x7D, /* 6 */
	0x07, /* 7 */
    0x7F, /* 8 */
	0x6F, /* 9 */
	0x77, /* a */
	0x7C, /* b */
    0x39, /* C */
	0x5E, /* d */
	0x79, /* E */
	0x71  /* F */
};

uint32_t display_timer = 0;

static uint8_t i2c_tx_buffer[11];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
void Display_Value(int sensor_grams);
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
  /* USER CODE BEGIN 2 */
  // Turn on system oscillator
  uint8_t osc_cmd = 0x21;
  if (HAL_I2C_Master_Transmit(&hi2c1, VK16K33_ADDR, &osc_cmd, 1, 100) != HAL_OK) {
      Error_Handler();
  }

  // Turn on display
  uint8_t display_cmd = 0x81;
  HAL_I2C_Master_Transmit(&hi2c1, VK16K33_ADDR, &display_cmd, 1, 100);

  // Set brightness to medium (0xEF for max, 0xE7 for mid)
  uint8_t bright_cmd = 0xE7;
  HAL_I2C_Master_Transmit(&hi2c1, VK16K33_ADDR, &bright_cmd, 1, 100);
//
//  // 4. Test pattern: Show '8' on all 4 digits and turn colon on
//  //    Register address: 0x00
//  //    0x7F = binary 01111111 (all segments A-G active)
//  uint8_t test_frame[11] = {
//      0x00,       // Start at register 0x00
//      0x7F, 0x00, // Digit 0: '8'
//      0x7F, 0x00, // Digit 1: '8'
//      0x02, 0x00, // Colon: ON (bit 1)
//      0x7F, 0x00, // Digit 2: '8'
//      0x7F, 0x00  // Digit 3: '8'
//  };
//  HAL_I2C_Master_Transmit(&hi2c1, VK16K33_ADDR, test_frame, sizeof(test_frame), 100);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//      // Break the count down into individual base-10 digits
//      uint8_t d0 = (count / 1000) % 10;
//      uint8_t d1 = (count / 100)  % 10;
//      uint8_t d2 = (count / 10)   % 10;
//      uint8_t d3 = count % 10;
//
//      // Toggle colon on even counts (0x02 = ON, 0x00 = OFF)
//      uint8_t colon = (count % 2 == 0) ? 0x02 : 0x00;
//
//      // Build the 11-byte transaction:
//      // Byte 0: Register pointer (0x00)
//      // Bytes 1-2: Digit 0 (lower, upper)
//      // Bytes 3-4: Digit 1 (lower, upper)
//      // Bytes 5-6: Colon   (lower, upper)
//      // Bytes 7-8: Digit 2 (lower, upper)
//      // Bytes 9-10: Digit 3 (lower, upper)
//      uint8_t frame[11] = {
//          0x00,
//          g_number_table[d0], 0x00,
//          g_number_table[d1], 0x00,
//          colon,              0x00,
//          g_number_table[d2], 0x00,
//          g_number_table[d3], 0x00
//      };
//
//      HAL_I2C_Master_Transmit(&hi2c1, VK16K33_ADDR, frame, sizeof(frame), 100);
//
//      count++;
//      if (count > 9999) {
//          count = 0;
//      }
//
//      HAL_Delay(1000); // Wait 2 seconds before next update
//    }

	uint32_t current_time = HAL_GetTick();
	// Update the display every 100ms
	if (current_time - display_timer >= 100) {
		display_timer = current_time;

		// Simulate sensor input - replace this with your actual sensor reading variable
		int simulated_sensor_input = 1236;
		Display_Value(simulated_sensor_input);
	}
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void Display_Value(int sensor_grams) { //0-180 as a float (only have display of 4 so show tenth place as well)
    if (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) return;

    if (display_val < 0) display_val = 0;
    if (display_val > 9999) display_val = 9999;

    uint8_t d0 = (display_val / 1000) % 10;
    uint8_t d1 = (display_val / 100)  % 10;
    uint8_t d2 = (display_val / 10)   % 10;
    uint8_t d3 = display_val % 10;

    // Static frame means interrupt I2C HAL can run without  having data overwritten
    static uint8_t frame[11];

    frame[0] = 0x00;
    frame[1] = g_number_table[d0]; frame[2] = 0x00;
    frame[3] = g_number_table[d1]; frame[4] = 0x00;
    frame[5] = 0x00;               frame[6] = 0x00;
    frame[7] = g_number_table[d2] | 0x80; frame[8] = 0x00;
    frame[9] = g_number_table[d3]; frame[10] = 0x00;

    HAL_I2C_Master_Transmit_IT(&hi2c1, VK16K33_ADDR, frame, 11);
}

void Display_Show_Scale_ID(uint8_t scale_num) {
    if (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) return;

    i2c_tx_buffer[0] = 0x00;
    i2c_tx_buffer[1] = 0x6D; i2c_tx_buffer[2] = 0x00; // 'S'
    i2c_tx_buffer[3] = 0x39; i2c_tx_buffer[4] = 0x00; // 'C'
    i2c_tx_buffer[5] = 0x02; i2c_tx_buffer[6] = 0x00; // Colon ON
    i2c_tx_buffer[7] = g_number_table[(scale_num / 10) % 10]; i2c_tx_buffer[8] = 0x00;
    i2c_tx_buffer[9] = g_number_table[scale_num % 10];        i2c_tx_buffer[10] = 0x00;

    HAL_I2C_Master_Transmit_IT(&hi2c1, VK16K33_ADDR, i2c_tx_buffer, 11);
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
