//
// Created by mores on 2022/11/6.
//

#ifndef TMC2209_H
#define TMC2209_H

#include "stm32f4xx_hal.h"

typedef struct {
	// 设置串口
	UART_HandleTypeDef *uart;
	// 电机一圈最小移动距离
	uint8_t minMoveDistance;
	// 细分数
	uint16_t div;
	// 丝杆最大行程
	uint16_t maxDistance;
	// 步距角
	double stepAngle;
	// step IO
	GPIO_TypeDef *STEP_Port;
	uint16_t STEP_GPIO_Pin;

	// EN IO
	GPIO_TypeDef *EN_Port;
	uint16_t EN_GPIO_Pin;
} TMC2209;

void MoveStepUart(uint64_t Step);

void moveToUART(uint8_t DIR_Flag, uint32_t moveDistance);

void initTMC2209(UART_HandleTypeDef *uart,uint8_t msres,
                 uint16_t maxDistance,uint8_t  minMoveDistance,
                 GPIO_TypeDef *STEP_Port,uint16_t STEP_GPIO_Pin,
                 GPIO_TypeDef *EN_Port,uint16_t EN_GPIO_Pin);

uint32_t readReg(uint8_t regAddr);
extern uint8_t stop;
#endif //TMC2209_H
