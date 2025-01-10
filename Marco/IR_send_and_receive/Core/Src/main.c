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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define RCLK_PIN GPIOB, GPIO_PIN_6

#define PIN_C0 GPIOC, GPIO_PIN_11
#define PIN_C1 GPIOC, GPIO_PIN_10
#define PIN_C2 GPIOC, GPIO_PIN_9
#define PIN_C3 GPIOC, GPIO_PIN_8

#define PIN_R0 GPIOC, GPIO_PIN_3
#define PIN_R1 GPIOC, GPIO_PIN_2
#define PIN_R2 GPIOC, GPIO_PIN_13
#define PIN_R3 GPIOC, GPIO_PIN_12

#define DEBOUNCE_TIME 2

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

char map[16] = "FB73EA62D951C840";

uint32_t keypress[16]; //Keep track of key presses
uint32_t ack[16];	   //Key press acknowledge

int column_index = 0;
char string[64] = {0}; //Where the character will be stored and outputted

char RX_byte, RX_byte2 = 0; //Received bytes
char command = 0; //The received byte copied from RX_byte in the UART rx callback
int new_command = 0; //If 1 a new data has been received

uint8_t BaudElapsedFlag = 0; //Check if the bit has been sent (during baud duration)

int column_index_rec = 0; //Check what column to print

uint8_t matrix[5][2] =
{
		{0, 16},
		{0, 8},
		{0, 4},
		{0, 2},
		{0, 1}
};

uint8_t matrix_QM[5][2] =
{
		{32, 16},
		{64, 8},
		{69, 4},
		{72, 2},
		{48, 1}
};

uint8_t matrix_A[5][2] =
{
		{31, 16},
		{36, 8},
		{68, 4},
		{36, 2},
		{31, 1}
};

uint8_t matrix_B[5][2] =
{
		{127, 16},
		{73, 8},
		{73, 4},
		{85, 2},
		{34, 1}
};

uint8_t matrix_C[5][2] =
{
		{62, 16},
		{65, 8},
		{65, 4},
		{65, 2},
		{34, 1}
};

uint8_t matrix_D[5][2] =
{
		{127, 16},
		{65, 8},
		{65, 4},
		{65, 2},
		{62, 1}
};

uint8_t matrix_E[5][2] =
{
		{127, 16},
		{73, 8},
		{73, 4},
		{73, 2},
		{73, 1}
};

uint8_t matrix_F[5][2] =
{
		{127, 16},
		{72, 8},
		{72, 4},
		{72, 2},
		{72, 1}
};

uint8_t matrix_0[5][2] =
{
		{62, 16},
		{113, 8},
		{73, 4},
		{71, 2},
		{62, 1}
};

uint8_t matrix_1[5][2] =
{
		{16, 16},
		{33, 8},
		{127, 4},
		{1, 2},
		{0, 1}
};

uint8_t matrix_2[5][2] =
{
		{33, 16},
		{67, 8},
		{69, 4},
		{73, 2},
		{49, 1}
};

uint8_t matrix_3[5][2] =
{
		{34, 16},
		{65, 8},
		{73, 4},
		{73, 2},
		{54, 1}
};

uint8_t matrix_4[5][2] =
{
		{12, 16},
		{20, 8},
		{36, 4},
		{68, 2},
		{127, 1}
};

uint8_t matrix_5[5][2] =
{
		{113, 16},
		{73, 8},
		{73, 4},
		{73, 2},
		{70, 1}
};

uint8_t matrix_6[5][2] =
{
		{30, 16},
		{41, 8},
		{73, 4},
		{73, 2},
		{6, 1}
};

uint8_t matrix_7[5][2] =
{
		{64, 16},
		{67, 8},
		{68, 4},
		{72, 2},
		{48, 1}
};

uint8_t matrix_8[5][2] =
{
		{54, 16},
		{73, 8},
		{73, 4},
		{73, 2},
		{54, 1}
};

uint8_t matrix_9[5][2] =
{
		{48, 16},
		{73, 8},
		{73, 4},
		{73, 2},
		{54, 1}
};

uint8_t CharMAP[256][5][2] =
{
		{32, 16},
		{64, 8},
		{69, 4},
		{72, 2},
		{48, 1}
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM10_Init(void);
static void MX_TIM11_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM5_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if(htim == &htim10)
	{
		BaudElapsedFlag = 1;
	}

	if(htim == &htim11)
	{
		// ---------- Row Pins reading -------------- //

		//Reading operation before the setting of the column

		if(HAL_GPIO_ReadPin(PIN_R0) == GPIO_PIN_SET)
		{
			keypress[4*column_index] = 0;
		}
		else
		{
			keypress[4*column_index]++; //A "software filter", if this value is bigger than 2 is not a debounce phenomena
		}


		if(HAL_GPIO_ReadPin(PIN_R1) == GPIO_PIN_SET)
		{
			keypress[4*column_index+1] = 0;
		}
		else
		{
			keypress[4*column_index+1]++;
		}

		if(HAL_GPIO_ReadPin(PIN_R2) == GPIO_PIN_SET)
		{
			keypress[4*column_index+2] = 0;
		}
		else
		{
			keypress[4*column_index+2]++;
		}

		if(HAL_GPIO_ReadPin(PIN_R3) == GPIO_PIN_SET)
		{
			keypress[4*column_index+3] = 0;
		}
		else
		{
			keypress[4*column_index+3]++;
		}

		// --------------------------------------- //

		column_index = (++column_index) % 4;

		HAL_GPIO_WritePin(PIN_C0, (column_index == 0)?GPIO_PIN_SET:GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PIN_C1, (column_index == 1)?GPIO_PIN_SET:GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PIN_C2, (column_index == 2)?GPIO_PIN_SET:GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PIN_C3, (column_index == 3)?GPIO_PIN_SET:GPIO_PIN_RESET);

	}

	if(htim == &htim5)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); //When the communication is finished, the
																//values are allowed to be written in the LED Matrix

		if(column_index_rec > 4)  //if we have reached the last column
			column_index_rec = 0;  //return to the first column
		else
			column_index_rec++;	//switch to the next column

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET); //We need to bring this signal back in order to not mix signals

		HAL_SPI_Transmit_DMA(&hspi1, matrix[column_index_rec], 2); //Send the column of the character

	}

}

void UART_IR_sendByte(char byte)
{
	HAL_TIM_Base_Start_IT(&htim10); //Start the timer10 to control the baudrate

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); //send start bit
	BaudElapsedFlag = 0;

	while(BaudElapsedFlag == 0) //Wait the correct baudrate elapse time
	{

	}

	for(int bit_ctr = 0; bit_ctr < 8; bit_ctr++) //for each bit of the data byte
	{
		if((byte & (0x01<<bit_ctr)) == 0) //select the correct bit, if is 0 send a 1 with the PWM Start
		{
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); //send a 0
		}
		else
		{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3); //send a 1
		}

		BaudElapsedFlag = 0;

		while(BaudElapsedFlag == 0) //Wait the correct baudrate elapse time
		{

		}

	}

	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3); //stop bit

	while(BaudElapsedFlag == 0)
	{

	}

	HAL_TIM_Base_Stop_IT(&htim10); //Stop the timer10

	BaudElapsedFlag = 0;



}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{

		HAL_UART_Receive_IT(&huart1, &RX_byte, 1);
		command = RX_byte;
		new_command = 1; //the data has been received, let the main allow the showletter function

	}

	if(huart == &huart2)
	{

		HAL_UART_Receive_IT(&huart2, &RX_byte2, 1);
		command = RX_byte2;
		new_command = 1;

	}
}

void initCharMAP()
{
	for(int i = 0; i <256; i++)
	{
		memcpy(CharMAP[i], matrix_QM, sizeof(matrix_QM)); // Initialize the ASCII character, if is not one of the list below, print a question mark
	}
	memcpy(CharMAP[(uint8_t) '0'], matrix_0, sizeof(matrix_0));
	memcpy(CharMAP[(uint8_t) '1'], matrix_1, sizeof(matrix_1));
	memcpy(CharMAP[(uint8_t) '2'], matrix_2, sizeof(matrix_2));
	memcpy(CharMAP[(uint8_t) '3'], matrix_3, sizeof(matrix_3));
	memcpy(CharMAP[(uint8_t) '4'], matrix_4, sizeof(matrix_4));
	memcpy(CharMAP[(uint8_t) '5'], matrix_5, sizeof(matrix_5));
	memcpy(CharMAP[(uint8_t) '6'], matrix_6, sizeof(matrix_6));
	memcpy(CharMAP[(uint8_t) '7'], matrix_7, sizeof(matrix_7));
	memcpy(CharMAP[(uint8_t) '8'], matrix_8, sizeof(matrix_8));
	memcpy(CharMAP[(uint8_t) '9'], matrix_9, sizeof(matrix_9));
	memcpy(CharMAP[(uint8_t) 'A'], matrix_A, sizeof(matrix_A));
	memcpy(CharMAP[(uint8_t) 'B'], matrix_B, sizeof(matrix_B));
	memcpy(CharMAP[(uint8_t) 'C'], matrix_C, sizeof(matrix_C));
	memcpy(CharMAP[(uint8_t) 'D'], matrix_D, sizeof(matrix_D));
	memcpy(CharMAP[(uint8_t) 'E'], matrix_E, sizeof(matrix_E));
	memcpy(CharMAP[(uint8_t) 'F'], matrix_F, sizeof(matrix_F));

}

void showletter(char digit)
{
	memcpy(matrix, CharMAP[(uint8_t) digit], sizeof(matrix)); //Copy this value in "matrix", this variable will be used in the SPI send
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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_TIM5_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  //HAL_TIM_Base_Start(&htim2);	//PWM timer
  //HAL_TIM_Base_Start_IT(&htim10); // uart send timer

  HAL_TIM_Base_Start_IT(&htim11); //keyboard timer

  HAL_TIM_Base_Start_IT(&htim5); // SPI send timer

  initCharMAP();

  HAL_UART_Receive_IT(&huart1, &RX_byte, 1); //Prepare the UART1 to receive data

  HAL_UART_Receive_IT(&huart2, &RX_byte2, 1);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  if(new_command) //If the IR UART receive callback has received something
	  {
		  showletter(command); //Copy the received byte (RX_byte) into "matrix" that will be sent through SPI to the LED matrix
		  new_command = 0;
	  }

	  for(int i = 0; i < 16; i++)
		{
			if(keypress[i] > DEBOUNCE_TIME)
			{
				if(ack[i] == 0)
				{
					int string_length = snprintf(string, sizeof(string), "%c\n", map[i]);
					HAL_UART_Transmit(&huart2, string, string_length, 1); //Do not use DMA because if we press 2 contemporary buttons the second is discarded and print only one

					UART_IR_sendByte(map[i]);

					ack[i] = 1;
				}
			}
			else
			{
				ack[i] = 0;
			}
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2210;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1105;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 84;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 2000;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 10 - 1;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 3500 - 1;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 84 - 1;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 5000 - 1;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 2400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC2 PC3 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC8 PC9 PC10 PC11 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
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
