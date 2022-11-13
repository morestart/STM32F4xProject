//
// Created by mores on 2022/10/21.
//

#ifndef PID_H
#define PID_H
#include "stm32f4xx_hal.h"

typedef struct Pid {
	float Sv;
	float Pv;

	float Kp;
	float Ki;
	float Kd;
// 	int T;
// 	float Ti;
// 	float Td;


	float Ek;
	float Ek_1;
	float SEk;

	float Iout;
	float Pout;
	float Dout;

	float OUT0;

	float OUT;

	int C1ms;

	int pwmcycle;

	int times;
} PID;

extern PID pid;

void PID_Init(uint8_t setNum);

void PID_Calc(void);

#endif //PID_H
