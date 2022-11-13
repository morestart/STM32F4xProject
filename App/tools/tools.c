//
// Created by mores on 2022/10/20.
//
/*
 * tools.c
 *
 *  Created on: Oct 19, 2022
 *      Author: mores
 */
#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "tools.h"
#include "stdarg.h"

UART_HandleTypeDef *gHuart;

void RetargetInit(UART_HandleTypeDef *huart) {
	gHuart = huart;
	/* Disable I/O buffering for STDOUT stream, so that
	 * chars are sent out as soon as they are printed. */
	setvbuf(stdout, NULL, _IONBF, 0);
}

void print(const char *buf, ...) {
	//const char *p = buf;
	unsigned char ii = 0;
	char str[255] = {0};
	va_list v;
	va_start(v, buf);
	vsprintf(str, buf, v); //使用可变参数的字符串打印。类似sprintf
	while (1) {
		if (str[ii] == 0)
			break;
		ii++;
		if (ii == 255)break;
	}
	HAL_UART_Transmit(gHuart, (uint8_t *) str, ii, 0xff);
	va_end(v);
}

void delay_init(u8 SYSCLK) {
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTick频率为HCLK
	fac_us = SYSCLK;      //不论是否使用OS,fac_us都需要使用
}

void delay_us(u32 nus) {
	u32 ticks;
	u32 told, tnow, tcnt = 0;
	u32 reload = SysTick->LOAD;    //LOAD的值
	ticks = nus * fac_us;       //需要的节拍数
	told = SysTick->VAL;            //刚进入时的计数器值
	while (1) {
		tnow = SysTick->VAL;
		if (tnow != told) {
			if (tnow < told)tcnt += told - tnow; //这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt += reload - tnow + told;
			told = tnow;
			if (tcnt >= ticks)break;   //时间超过/等于要延迟的时间,则退出.
		}
	};
}

//延时nms
//nms:要延时的ms数
void delay_ms(u16 nms) {
	u32 i;
	for (i = 0; i < nms; i++) delay_us(1000);
}
