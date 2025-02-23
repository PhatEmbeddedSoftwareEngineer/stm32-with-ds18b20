/*
 * oneWire.h
 *
 *  Created on: Feb 16, 2025
 *      Author: Phat
 */

#ifndef INC_ONEWIRE_H_
#define INC_ONEWIRE_H_

#include "main.h"
#include <stdint.h>
#define ONEWIRE_PIN		GPIO_PIN_1
#define ONEWIRE_GPIO_PORT	GPIOA
#define GPIOA_CLOCK_EN      (1U << 0)

// nghĩa là mình sẽ sử dụng chân PA1

// hàm này không có parameter
// tên hàm là delay_us_dwt_init
void delay_us_dwt_init(void);

// hàm này có một parameter là us kiểu dữ liệu uint32_t
// tên hàm là delay_us
void delay_us(uint32_t us);

// hàm này có một parameter là ms kiểu dữ liệu là uint32_t
// tên hàm là delay_ms
void delay_ms(uint32_t ms);

// hàm này là hàm uint8_t kiểu trả về là return uint8_t
// parameter là uint8_t data
// tên hàm là onewire_reset
uint8_t onewire_reset(void);

// hàm này dùng để ghi data vào ds18b20
void onewire_write(uint8_t data);

// hàm này trả về kiểu uint8_t
// không có parameter
// tên hàm là onewire_read_bit
uint8_t onewire_read_bit(void);

// hàm này có kiểu trả về là uint8_t
// hàm ko có parameter
// tên hàm là onewire_read
uint8_t onewire_read(void);

// hàm này là hàm void
// có 2 tham số là con trỏ GPIO_InitTypeDef, và uint16_t
// tên hàm là Output_Pin
void outputPin(GPIO_TypeDef *GPIOx,uint16_t pin_number);

// hàm này là hàm void
// có 2 tham số là con trỏ GPIO_InitTypeDef* và uint16_t
// tên hàm là Intput_Pin
void inputPin(GPIO_TypeDef *GPIOx, uint16_t pin_number);




#endif /* INC_ONEWIRE_H_ */
