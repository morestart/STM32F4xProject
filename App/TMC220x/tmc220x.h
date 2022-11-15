//
// Created by mores on 2022/10/25.
//

#ifndef TMC220X_H
#define TMC220X_H

#include "stm32f407xx.h"

typedef struct {
	// 电机一圈最小移动距离
	uint8_t minMoveDistance;
	// 细分数
	uint8_t div;
	// 丝杆最大行程
	uint16_t maxDistance;
	// 步距角
	double stepAngle;
	// step IO
	GPIO_TypeDef *STEP_Port;
	uint16_t STEP_GPIO_Pin;
	// dir io
	GPIO_TypeDef *DIR_Port;
	uint16_t DIR_GPIO_Pin;

	// EN IO
	GPIO_TypeDef *EN_Port;
	uint16_t EN_GPIO_Pin;
	//MS1
	GPIO_TypeDef *MS1_Port;
	uint16_t MS1_GPIO_Pin;
	// MS2
	GPIO_TypeDef *MS2_Port;
	uint16_t MS2_GPIO_Pin;
} MOTOR;

void MoveStep(uint64_t Step);

void SubdivisionSet(uint8_t i);

void initMotorSubDivision(MOTOR *customMotor);

void moveTo(uint8_t DIR_Flag, uint32_t moveDistance);

void disableMotor();

#endif //TMC220X_H
