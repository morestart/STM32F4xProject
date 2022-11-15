//
// Created by mores on 2022/10/25.
//
#include "tmc220x.h"
#include "stm32f4xx_hal.h"
#include "tools.h"

MOTOR *motorInfo;


void disableMotor() {
	HAL_GPIO_WritePin(motorInfo->EN_Port, motorInfo->EN_GPIO_Pin, GPIO_PIN_SET);
}

/*!
 * 移动到指定位置
 * @param DIR_Flag 移动方向 1正转逆时针 0反转
 * @param moveDistance 移动距离 mm
 */
void moveTo(uint8_t DIR_Flag, uint32_t moveDistance) {
	// 电机转动一圈需要的步数
	double steps = ((360 * (double) motorInfo->div) / motorInfo->stepAngle);
	// 计算需要多少圈才能转到指定位置
	double nums = ((double) moveDistance / (double) motorInfo->minMoveDistance);

	//电机使能
	HAL_GPIO_WritePin(motorInfo->EN_Port, motorInfo->EN_GPIO_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	if (DIR_Flag) {
		//正转
		HAL_GPIO_WritePin(motorInfo->DIR_Port, motorInfo->DIR_GPIO_Pin, GPIO_PIN_SET);
	} else {
		//反转
		HAL_GPIO_WritePin(motorInfo->DIR_Port, motorInfo->DIR_GPIO_Pin, GPIO_PIN_RESET);
	}
	MoveStep((uint64_t) (steps * nums));

	HAL_Delay(10);
	//电机非使能
	HAL_GPIO_WritePin(motorInfo->EN_Port, motorInfo->EN_GPIO_Pin, GPIO_PIN_SET);
}

/*!
 * 软PWM
 * @param DIR_Flag 转动方向
 * @param Step 转动步数 一圈360° 旋转角度=步距角/细分数
 */
#define acc 40
#define len 200

void MoveStep(uint64_t Step) {
	uint64_t i;
	for (i = 0; i < Step; i++) {
		//发送脉冲
		HAL_GPIO_WritePin(motorInfo->STEP_Port, motorInfo->STEP_GPIO_Pin, GPIO_PIN_RESET);
		delay_us(10);
		HAL_GPIO_WritePin(motorInfo->STEP_Port, motorInfo->STEP_GPIO_Pin, GPIO_PIN_SET);
		delay_us(10);
	}
}

/*!
 * 设置步进电机的细分
 * @param i 细分数 仅支持8 16 32 64
 */
void SubdivisionSet(uint8_t i) {
	if (i == 8) {
		HAL_GPIO_WritePin(motorInfo->MS2_Port, motorInfo->MS2_GPIO_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(motorInfo->MS1_Port, motorInfo->MS1_GPIO_Pin, GPIO_PIN_RESET);
	} else if (i == 16) {
		// 16细分
		HAL_GPIO_WritePin(motorInfo->MS2_Port, motorInfo->MS2_GPIO_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(motorInfo->MS1_Port, motorInfo->MS1_GPIO_Pin, GPIO_PIN_SET);
	} else if (i == 32) {
		HAL_GPIO_WritePin(motorInfo->MS2_Port, motorInfo->MS2_GPIO_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(motorInfo->MS1_Port, motorInfo->MS1_GPIO_Pin, GPIO_PIN_SET);
	} else if (i == 64) {
		HAL_GPIO_WritePin(motorInfo->MS2_Port, motorInfo->MS2_GPIO_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(motorInfo->MS1_Port, motorInfo->MS1_GPIO_Pin, GPIO_PIN_RESET);
	} else {

	}
}

/*!
 * 初始化步进电机 并设置细分
 * @param customMotor 结构体
 */
void initMotorSubDivision(MOTOR *customMotor) {
	motorInfo = customMotor;
	// 细分设置
	SubdivisionSet(motorInfo->div);
	// step设置为高
	HAL_GPIO_WritePin(motorInfo->STEP_Port, motorInfo->STEP_GPIO_Pin, GPIO_PIN_SET);
}