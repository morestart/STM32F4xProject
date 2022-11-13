//
// Created by mores on 2022/10/21.
//

#ifndef TOOLS_H
#define TOOLS_H

#include "stm32f4xx_hal.h"

#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t
static u8  fac_us=0;       //us延时倍乘数
static u16 fac_ms=0;       //ms延时倍乘数,在os下,代表每个节拍的ms数
void RetargetInit(UART_HandleTypeDef *huart);
void print(const char *buf, ...);
void delay_init(u8 SYSCLK);
void delay_us(u32 nus);
void delay_ms(u16 nms);
#endif //TOOLS_H

