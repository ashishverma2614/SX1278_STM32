/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2020 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "SX1278.h"
/*The module has to be connected with the MCU board with following pins:
*
*	MCU pin		SX1278 pin	Description
*	--------------------------------------
*	PA4		NSS		SPI chip--select
*	PA5		SCK		SPI SCK
*	PA6		SO 		SPI MISO
*	PA7		SI		SPI MOSI
*	PB0		DIO0		LoRa interrupt indicator pin (for receiver mode)
*	PB1		RST		LoRa reset
*	VDD		VCC		+3V3
*	VSS		GND		GND
*	--------------------------------------
*/
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
/* Private variables ---------------------------------------------------------*/

//struct __FILE {int handle;/* Add whatever you need here */};

//FILE __stdout;
//FILE __stdin;

//int fputc(int ch, FILE *f)
//{
  //ITM_SendChar(ch);
  //return(ch);
//}

 int fputc(int c, FILE *stream)
{
  return(ITM_SendChar(c));
}
 
//int _write(int file, char *ptr, int len) 
	//{
		//int i;
		//for (i = 0; i < len; i++) 
		//{
			//ITM_SendChar(*ptr++);
		//}
	//return len;
//}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

SX1278_hw_t SX1278_hw;
SX1278_t SX1278;

int master;
int ret;

char buffer[64];

int message;
int message_length;
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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
//checking PB2 pin is HIGH or LOW 
printf("starting Programm \r\n"); // for testing debugging 
master = HAL_GPIO_ReadPin(MODE_GPIO_Port, MODE_Pin);    
	if (master == 1)
		{
			printf("Mode: Master\r\n");
			HAL_GPIO_WritePin(MODE_GPIO_Port, MODE_Pin, GPIO_PIN_RESET);
		}
	else 
		{
			printf("Mode: Slave\r\n");
			HAL_GPIO_WritePin(MODE_GPIO_Port, MODE_Pin, GPIO_PIN_SET);
		}

	//initialize LoRa module
	SX1278_hw.dio0.port = DIO0_GPIO_Port;
	SX1278_hw.dio0.pin = DIO0_Pin;
	SX1278_hw.nss.port = NSS_GPIO_Port;
	SX1278_hw.nss.pin = NSS_Pin;
	SX1278_hw.reset.port = RESET_GPIO_Port;
	SX1278_hw.reset.pin = RESET_Pin;
	SX1278_hw.spi = &hspi1;

	SX1278.hw = &SX1278_hw;

	printf("Configuring LoRa module\r\n");
	SX1278_begin(&SX1278, SX1278_433MHZ, SX1278_POWER_17DBM, SX1278_LORA_SF_8,    //SX1278_begin(SX1278_t * module, uint8_t frequency, uint8_t power,
			SX1278_LORA_BW_20_8KHZ, 10);																																					//uint8_t LoRa_Rate, uint8_t LoRa_BW, uint8_t packetLength)
	
	
	printf("Done configuring LoRaModule\r\n");

	if (master == 1) 
	{
		ret = SX1278_LoRaEntryTx(&SX1278, 16, 2000);
	} else 
	
	{
		ret = SX1278_LoRaEntryRx(&SX1278, 16, 2000);
	}

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		 
		if (master == 1) 
			{
			printf("Master ...\r\n");
			HAL_Delay(2500);
			printf("Sending package...\r\n");

			message_length = sprintf(buffer, "Hello %d", message);
			ret = SX1278_LoRaEntryTx(&SX1278, message_length, 2000);
			printf("Entry: %d\r\n", ret);

			printf("Sending %s\r\n", buffer);
			ret = SX1278_LoRaTxPacket(&SX1278, (uint8_t *) buffer, message_length,
					2000);
			message += 1;

			printf("Transmission: %d\r\n", ret);
			printf("Package sent...\r\n");

		}
		else {
			printf("Slave ...\r\n");
			HAL_Delay(1000);
			printf("Receiving package...\r\n");

			ret = SX1278_LoRaRxPacket(&SX1278);
			printf("Received: %d\r\n", ret);
			if (ret > 0) {
				SX1278_read(&SX1278, (uint8_t *) buffer, ret);
				printf("Content (%d): %s\r\n", ret, buffer);
			}
			printf("Package received ...\r\n");

		}
	
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

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
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
  while(1) 
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
