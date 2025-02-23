#include "main.h"
#include <stdarg.h>
#include "oneWire.h"
#include <stdio.h>

#define USART1_EN       (1U << 13)
#define USART1_CLOCK_EN             (1U << 4)
#define TX_EN           (1U << 3)
#define RX_EN           (1U << 2)
#define TRANSMISSION_COMPLETE       (1U << 7)
void SystemClock_Config(void);
float convertBinaryToTemperature(uint16_t data)
{
	float result;
	// if flagSign = 1 is nagative, flagSign = 0 positive
	int flagSign = 0; 
	int temp = data >> 11;
	if(temp & 0x01)
	{
		flagSign = 1;
	}
	temp = (data >> 4) & 0x7F; 
	if(!flagSign)
	{
		if(data & 0xF)
		{
			switch(data & 0xF)
			{
				case 1:
				{
					result = (temp + 0.0625); 
					break;
				}
				case 2:
				{
					result = (temp + 0.125);
					break;
				}
				case 4:
				{
					result = (temp + 0.25);
					break;
				}
				case 8:
				{
					result = (temp + 0.5);
					break;
				}
			}
		}
		else 
		{
			return temp;
		}
	}
	if(flagSign)
	{
		result = 127 - temp;
		if(temp < 100)
		{
			result +=1;
		}
		if(data & 0xF)
		{
			switch(data & 0xF)
			{
				case 8:
				{
					result = - (result + 0.5);
					break;
				}
				case 0b1110:
				{
					result = - (result + 0.125);
					break;
				}
				case 0b1111:
				{
					result = - (result + 0.0625);
					break;
				}
				case 0b1100:
				{
					result = - (result + 0.25);
					break;
				}
			}
		}
		else 
		{
			return -result;
		}
	}
	return result;
}

float ds18b20_temp(void)
{
	uint8_t t1,t2;
  	uint16_t twoByteTemp;

  	onewire_reset();
  	// cách này dành cho bus chỉ có 1 ds18b20
  	// skip rom command
  	onewire_write(0xCC);
  	// convert T [44h]
  	onewire_write(0x44);
  	// xem xét lại hàm delay này có thể giảm delay
	HAL_Delay(800); 

  	// reset bus
	onewire_reset();
	// skip rom command
	onewire_write(0xCC);
  	// read data from scratchpad
  	onewire_write(0xBE);

  	t1 = onewire_read();
  	t2 = onewire_read();

  	twoByteTemp = (t2 << 8) | t1;
  	float result = convertBinaryToTemperature(twoByteTemp);
  	return result;

}

void uart1_rxtx(void)
{
	// 1. enable clock gpioa
	RCC->AHB1ENR |= GPIOA_CLOCK_EN;
	// 2. set mode alternate for PA9 TX
	GPIOA->MODER &= ~(3U << 18);
	GPIOA->MODER |= (2U << 18);
	GPIOA->AFR[1] |= (7U << 4);

	// 3.set mode alternate for P10 RX
	GPIOA->MODER |= (2U << 20);
	GPIOA->AFR[1] |= (7U << 8);

	// enable clock USART1
	RCC->APB2ENR |= USART1_CLOCK_EN;

	// config baudrate 115200
	// chose oversampling 16
	USART1->CR1 &= ~ (1U << 15);
	USART1->BRR |= (8U << 4) | (11U << 0);

	//configure 1 bit start, 8 bit data, 1 stop bit
	USART1->CR1 &= ~(1U << 12);
	USART1->CR2 &= ~(3U << 12);

	// enable transmit data
	USART1->CR1 |= TX_EN;
	// enable receive data
	USART1->CR1 |= RX_EN;

	#ifdef USE_INTERRUPT_RECEIVER
	USART1->USART_CR1 |= (1U << 5);
	__NVIC_EnableIRQ(USART1_position);
	initStructData(&_data);
	#endif

	// enable USART1
	USART1->CR1 |= USART1_EN;

}
static void tranmissionEachByte(char *str)
{
    while(*str)
    {
        USART1->DR = (*str++) & 0xFF;
        while(!(USART1->SR & TRANSMISSION_COMPLETE));
    }
}
void myPrintf(const char *fmt,...)
{
  	va_list arg;
  	va_start(arg,fmt);
  	char buffer[256];
  	vsnprintf(buffer,sizeof(buffer),fmt,arg);
  	tranmissionEachByte(buffer);
  	va_end(arg);
}

int main(void)
{

  	HAL_Init();
  	SystemClock_Config();
  	float temperature;
  	uart1_rxtx();
  	delay_us_dwt_init();
	onewire_reset();

  	while (1)
  	{
    	temperature = ds18b20_temp();
		myPrintf("temperature: %f\n",temperature);
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
