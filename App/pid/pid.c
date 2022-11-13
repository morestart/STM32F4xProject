//
// Created by mores on 2022/10/21.
//
#include "PID.h"
#include "stm32f4xx_hal.h"
PID pid;

void PID_Init(uint8_t setNum) {
	pid.Sv = setNum;//预期温度
	pid.Kp = 30;
	pid.Ki = 0.003;
	pid.Kd = 75;
	pid.pwmcycle = 500;//pwm波周期500us
}

void PID_Calc()  //pid
{
	float DelEk;
	float out;
	pid.Ek = pid.Sv - pid.Pv;
	pid.SEk += pid.Ek;
	DelEk = pid.Ek - pid.Ek_1;

	pid.Pout = pid.Kp * pid.Ek;     //????
	pid.Iout = pid.Ki * pid.SEk;    //????
	pid.Dout = pid.Kd * DelEk;      //????

	out = pid.Pout + pid.Iout + pid.Dout;

	if (out > pid.pwmcycle) {
		pid.OUT = pid.pwmcycle;
	} else if (out <= 0) {
		pid.OUT = pid.OUT0;
	} else {
		pid.OUT = out;
	}
	pid.Ek_1 = pid.Ek;
}