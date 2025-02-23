/*
 * oneWire.c
 *
 *  Created on: Feb 16, 2025
 *      Author: Phat
 */

#ifndef SRC_ONEWIRE_C_
#define SRC_ONEWIRE_C_

#include "oneWire.h"
#include <core_cm4.h>

static uint32_t cycle_per_microsecond = 0;

void delay_us_dwt_init(void)
{
    // set bit 1 << 24 trong thanh ghi DEMCR
	// enable trace and debug feature
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    // enable DWT cycle counter
	// DWT (Data Watch point and Trace)
    DWT->CTRL |= DWT_CTRL_CPIEVTENA_Msk;
    cycle_per_microsecond = HAL_RCC_GetHCLKFreq()/1000000;
    // enable clock gpioa
    RCC->AHB1ENR |= GPIOA_CLOCK_EN;
}

void delay_us(uint32_t us)
{
    DWT->CYCCNT = 0;
    while(DWT->CYCCNT <= cycle_per_microsecond*us);
}

void delay_ms(uint32_t ms)
{
    delay_us(1000*ms);
}

void outputPin(GPIO_TypeDef *GPIOx, uint16_t pin_number)
{
    GPIOx->MODER &= ~ (0xF << 2*pin_number);
    GPIOx->MODER |= (1U << 2*pin_number);
}

void inputPin(GPIO_TypeDef *GPIOx, uint16_t pin_number)
{
    GPIOx->MODER &= ~ (0xF << 2*pin_number);
    GPIOx->MODER |= (0b00 << 2*pin_number);
}



uint8_t onewire_reset(void)
{
    uint8_t sensorOk = 0;
    // kéo bus xuống mức thấp trong thời gian 480us để tạo reset trên bus
    outputPin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN);
    HAL_GPIO_WritePin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN,GPIO_PIN_RESET);
    delay_us(480);

    // sau khi kéo xuống thấp 480us ds18b20 sẽ đợi 15-60us 
	// chuyển sang input để ngưng trạng thái kéo xuông
    inputPin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN);
    delay_us(60);
    /*
	sau khoảng thời gian này ds18b20 sẽ phát 1 xung gọi là "presence" bằng  cách 	
	kéo bus xuống thấp trong khoảng từ 60us đến 240us. để báo với master biết là nó 
	đã sẵn sàng để nhận dữ liệu.
	*/
    if(HAL_GPIO_ReadPin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN) == GPIO_PIN_RESET)
    {
        sensorOk = 1;
    }
    delay_us(240);
    return sensorOk;
}

void onewire_write(uint8_t data)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        if(data & (1 << i))
        {
            /**
             * Để ghi 1 vào ds18b20 phải kéo xuống trong khoảng thời gian dưới 15us
             */
            outputPin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN);
            HAL_GPIO_WritePin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN,GPIO_PIN_RESET);
            delay_us(8);

            inputPin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN);
            delay_us(60);

        }
        else
        {
            /**
             * để ghi 0 vào ds18b20 phải kéo xuống và giữ ở mức low là 60us
             */
            outputPin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN);
            HAL_GPIO_WritePin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN,GPIO_PIN_RESET);
            delay_us(60);

            inputPin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN);
            delay_us(8);

        }
    }
}

uint8_t onewire_read_bit(void)
{
    uint8_t data = 0;
    // kéo xuống 1us để tạo thời gian read time slot
    outputPin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN);
    HAL_GPIO_WritePin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN,GPIO_PIN_RESET);
    delay_us(1);

    // thả ra sau khi kéo 1us 3us là thời gian phục hồi
    inputPin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN);
    delay_us(3);
    if(HAL_GPIO_ReadPin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN) == GPIO_PIN_SET)
    {
        data = 1;
    }
    delay_us(60);
    return data;
}


/*
	hàm này dùng để đọc các 8 bit data liên tục từ ds18b20 
	
*/

uint8_t onewire_read(void)
{
    uint8_t data = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        outputPin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN);
        HAL_GPIO_WritePin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN,GPIO_PIN_RESET);
        delay_us(1);

        inputPin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN);
        delay_us(3);
        if(HAL_GPIO_ReadPin(ONEWIRE_GPIO_PORT,ONEWIRE_PIN) == GPIO_PIN_SET)
        {
            data |= (1 << i);
        }
        delay_us(60);
    }
    return data;
}



#endif /* SRC_ONEWIRE_C_ */
