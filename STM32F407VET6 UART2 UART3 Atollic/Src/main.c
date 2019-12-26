/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint8_t tx[100];
volatile uint8_t rx[100];
uint8_t rx1[100];


uint8_t tx1[100];

volatile uint8_t txCmpl=0;
volatile uint8_t rxCmpl=0;
volatile uint8_t toTransfer=0;
volatile uint8_t wasTransferred=0;


//volatile uint16_t rxCount=0;
volatile uint16_t toRead=0;
volatile uint16_t rxPos2=0;
volatile uint16_t rxPos1=0;
volatile uint16_t wasReadden=0;
uint8_t* stateUART(HAL_UART_StateTypeDef State);

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
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	HAL_StatusTypeDef res;
	uint8_t jj=0;

	HAL_Delay(10);

	rxCmpl=0;
	toRead=1;
	wasReadden=0;
	rxPos1=0;
	rxPos2=0;

	// всегда сначала надо подготовить чтение
	if(res=HAL_UART_Receive_IT(&huart3, (uint8_t *)rx, toRead) == HAL_OK)
		; //printf("HAL_OK\n");
	else if(res = HAL_ERROR)
		printf("< HAL_ERROR\n");
	else  if(res = HAL_BUSY)
		printf("< HAL_BUSY\n");

	for(int ii=0; ii< sizeof(tx); ii++)
		tx[ii]=ii;

	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */


		if(txCmpl == 1)
		{
			txCmpl = 0;
			printf("=> toTransfer=%d wasTransferred=%d \n",toTransfer,wasTransferred);

			printf("-->  ");
			for(uint8_t ii=0; ii < wasTransferred; ii++)
				printf("0x%.2X ",tx[ii]);
			printf("\n");
			wasTransferred=0;

		}
		else if(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_SET)
		{
			HAL_Delay(1000); // это чтобы дребезга не было

			printf("\n ---- %d -----\n",jj++);

			/*if(huart3.RxState == HAL_UART_STATE_BUSY_RX) // это нормально , была запущена HAL_UART_Receive_IT
			{
				if(huart3.RxXferSize >1 & (huart3.RxXferCount < huart3.RxXferSize))
				// часть предыдущего принята ,а часть еще нет  : еще не все байты приняты
				// это тоже нормально , т.к. это нас не волнует, все-равно все байты будут приняты по одному
				{
					printf("UART3 get read RxXferCount=%d RxXferSize=%d wasReadden=%d\n",huart3.RxXferCount,huart3.RxXferSize,wasReadden);
					printf("Lock=%d\n",huart3.Lock);
					printf("RxState=%.2X\n",huart3.RxState);

					rxCmpl = 0;
					toRead=1;//huart3.RxXferCount;
					//wasReadden=0;
					if(res=HAL_UART_Receive_IT(&huart3, (uint8_t *)(rx+(uint8_t)wasReadden), (uint16_t)toRead) == HAL_OK)
						;//printf("HAL_OK\n");
					else if(res = HAL_ERROR)
					{
						printf("< 2 HAL_ERROR %.2X state=%s\n",huart3.ErrorCode,stateUART(huart3.RxState));
					}
					else  if(res = HAL_BUSY)
						printf("< 2 HAL_BUSY\n");
					//HAL_Delay(1);

					continue; // давайте-ка дождемся приехода всех байт сначала
				}
			}*/

			// посылаем данные
			txCmpl = 0;
			toTransfer=33;
			wasTransferred=0;
			wasReadden=0;

			if(res=HAL_UART_Transmit_IT(&huart2, (uint8_t *)tx, toTransfer) == HAL_OK)
				;//printf("HAL_OK\n");
			else if(res = HAL_ERROR)
				printf("> HAL_ERROR\n");
			else  if(res = HAL_BUSY)
				printf("> HAL_BUSY\n");

			// Enable the USART Receive Not Empty interrupt

		}
		else if(rxPos2 != rxPos1) // прием всегда в приоритете
		// значит есть необработанные принятые байты
		{
			printf("<= toRead=%d rxPos1=%d rxPos2=%d\n",toRead,rxPos1,rxPos2);

			printf("<-- ");

			if(rxPos1 > rxPos2) // читаем хвост
			{
				for(uint8_t ii=rxPos1; ii < sizeof(rx1); ii++)
				{
					printf(" 0x%.2X ",rx1[rxPos2+ii]);
					wasReadden++;
				}
				rxPos1=0;
			}

			while(rxPos1 < rxPos2) // догоняем маркер rxPos2
			{
				printf(" 0x%.2X ",rx1[rxPos1++]);
				wasReadden++;
			}

			printf("\n");

		}
		else
		{
			//HAL_Delay(1);//printf("");
		}

		HAL_Delay(1);
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
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}
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
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void)
{

	/* USER CODE BEGIN USART3_Init 0 */

	/* USER CODE END USART3_Init 0 */

	/* USER CODE BEGIN USART3_Init 1 */

	/* USER CODE END USART3_Init 1 */
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart3) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART3_Init 2 */

	/* USER CODE END USART3_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : KEY2_Pin */
	GPIO_InitStruct.Pin = KEY2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(KEY2_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : KEY1_Pin */
	GPIO_InitStruct.Pin = KEY1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LED2_Pin */
	GPIO_InitStruct.Pin = LED2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart-> Instance == USART2)
	{
		uint16_t TxXferCount=huart->TxXferCount;
		uint16_t TxXferSize=huart->TxXferSize;

		if(TxXferSize == 0 ) // какая-то ошибка
		{
			//printf(". HAL_UART_TxCpltCallback count=%d  size=%d \n",count,size);
			HAL_UART_Transmit_IT(huart, (uint8_t *)(tx),toTransfer);

		}
		else if (TxXferSize > 0 & TxXferCount < TxXferSize)
		{
			for(int ii=0; ii<  (TxXferSize-TxXferCount); ii++)
				tx1[wasTransferred++]=tx[ii];

			if(TxXferCount>0)
				HAL_UART_Transmit_IT(huart, (uint8_t *)(tx+TxXferSize-TxXferCount),TxXferCount);
			else
				txCmpl=1;

			//printf("USART2 HAL_UART_TxCpltCallback TxXferCount=%d  TxXferSize=%d \n",count,size);
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// ВАЖНО , ЧТОБЫ HAL_UART_RxCpltCallback завершилась до прихода следующего байта !!!
	// точнее пока следующий байт приходит , у нас еще есть время, а вот следующий за следующим уже начнет затирать перед ним
	// По-этому все здесь делаем быстро !
	// Иначе ...

	if (huart-> Instance == USART3)
	{
		uint16_t RxXferCount=huart->RxXferCount;
		uint16_t RxXferSize=huart->RxXferSize;

		//printf("USART2 HAL_UART_RxCpltCallback count=%d  size=%d \n",count,size);

		if(RxXferSize == 0) // похоже на сбой
		{
			HAL_UART_Receive_IT(huart, (uint8_t *)(rx),toRead);
		}
		else if(RxXferSize > 0 && RxXferCount < RxXferSize)
		{
			for(int ii=0; ii<  (RxXferSize-RxXferCount); ii++)
			{
				if(rxPos2 == sizeof(rx1))
					rxPos2=0;

				rx1[rxPos2++]=rx[ii];
			}

			if(RxXferCount>0)
				HAL_UART_Receive_IT(huart, (uint8_t *)(rx+RxXferSize-RxXferCount),RxXferCount);
			else
			{
				rxCmpl=1;
				HAL_UART_Receive_IT(huart, (uint8_t *)(rx),toRead);
			}
		}
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart-> Instance == USART2)
	{
		uint16_t TxXferCount=huart->TxXferCount;
		uint16_t TxXferSize=huart->TxXferSize;
		printf("> HAL_UART_ErrorCallback USART2\n");
		printf("> TxXferCount=%d TxXferSize=%d\n",TxXferCount,TxXferSize);
	}
	else if (huart-> Instance == USART3)
	{
		uint16_t RxXferCount=huart->RxXferCount;
		uint16_t RxXferSize=huart->RxXferSize;
		printf("< HAL_UART_ErrorCallback USART3\n");
		printf("< RxXferCount=%d RxXferSize=%d\n",RxXferCount,RxXferSize);
		HAL_UART_Receive_IT(huart, (uint8_t *)(rx+RxXferSize-RxXferCount),RxXferCount);
		// проблема в приемном буфере, ПЕРЕПОЛНЕНИЕ, не все вытащили ДО прихода следующей пачки данных
	}

}
void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart)
{
	/*if (huart-> Instance == USART2)
	{*/
	printf("HAL_UART_AbortReceiveCpltCallback \n");
	//}
}
void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef *huart)
{
	/*if (huart-> Instance == USART2)
	{*/
	printf("HAL_UART_AbortTransmitCpltCallback \n");
	//}
}
void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart)
{
	/*if (huart-> Instance == USART2)
	{*/
	printf("HAL_UART_AbortCpltCallback \n");
	//}
}

uint8_t* stateUART(HAL_UART_StateTypeDef State)
{
	switch(State)
	{
	case HAL_UART_STATE_RESET: 		return "HAL_UART_STATE_RESET";
	case HAL_UART_STATE_READY: 		return "HAL_UART_STATE_READY";
	case HAL_UART_STATE_BUSY: 		return "HAL_UART_STATE_BUSY";
	case HAL_UART_STATE_BUSY_TX: 	return "HAL_UART_STATE_BUSY_TX";
	case HAL_UART_STATE_BUSY_RX: 	return "HAL_UART_STATE_BUSY_RX";
	case HAL_UART_STATE_BUSY_TX_RX: return "HAL_UART_STATE_BUSY_TX_RX";
	case HAL_UART_STATE_TIMEOUT: 	return "HAL_UART_STATE_TIMEOUT";
	case HAL_UART_STATE_ERROR: 		return "HAL_UART_STATE_ERROR";
	default : 						return "?????";
	}
	return "????";
}

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM1) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

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
void assert_failed(uint8_t *file, uint32_t line)
{ 
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
