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

	GPIO_TypeDef *DIAG_Port;
	uint16_t DIAG_GPIO_Pin;

	uint8_t stop;
} TMC2209;


//uint8_t GCONF = 0x00;
//uint8_t GSTAT = 0x01;
//uint8_t IFCNT = 0x02;
//uint8_t IOIN = 0x06;
//uint8_t IHOLD_IRUN = 0x10;
//uint8_t TSTEP = 0x12;
//uint8_t VACTUAL = 0x22;
//uint8_t TCOOLTHRS = 0x14;
//uint8_t SGTHRS = 0x40;
//uint8_t SG_RESULT = 0x41;
//uint8_t MSCNT = 0x6A;
//uint8_t CHOPCONF = 0x6C;
//uint8_t DRVSTATUS = 0x6F;

////GCONF
//uint8_t i_scale_analog = 1 << 0;
//uint8_t internal_rsense = 1 << 1;
//uint8_t en_spreadcycle = 1 << 2;
//uint8_t shaft = 1 << 3;
//uint8_t index_otpw = 1 << 4;
//uint8_t index_step = 1 << 5;
//uint8_t pdn_disable = 1 << 6;
//uint8_t mstep_reg_select = 1 << 7;
//
////GSTAT
//uint8_t reset = 1 << 0;
//uint8_t drv_err = 1 << 1;
//uint8_t uv_cp = 1 << 2;
//
////CHOPCONF
//uint32_t vsense = 1 << 17;
//uint32_t msres0 = 1 << 24;
//uint32_t msres1 = 1 << 25;
//uint32_t msres2 = 1 << 26;
//uint32_t msres3 = 1 << 27;
//uint32_t intpol = 1 << 28;
//
////IOIN
//uint8_t io_enn = 1 << 0;
//uint8_t io_step = 1 << 7;
//uint16_t io_spread = 1 << 8;
//uint16_t io_dir = 1 << 9;
//
////DRVSTATUS
//uint32_t stst = 1 << 31;
//uint32_t stealth = 1 << 30;
//uint32_t cs_actual = 31 << 16;
//uint16_t t157 = 1 << 11;
//uint16_t t150 = 1 << 10;
//uint16_t t143 = 1 << 9;
//uint16_t t120 = 1 << 8;
//uint8_t olb = 1 << 7;
//uint8_t ola = 1 << 6;
//uint8_t s2vsb = 1 << 5;
//uint8_t s2vsa = 1 << 4;
//uint8_t s2gb = 1 << 3;
//uint8_t s2ga = 1 << 2;
//uint8_t ot = 1 << 1;
//uint8_t otpw = 1 << 0;
//
////IHOLD_IRUN
//uint8_t ihold = 31 << 0;
//uint32_t irun = 31 << 8;
//uint32_t iholddelay = 15 << 16;
//
////SGTHRS
//uint8_t sgthrs = 255 << 0;

#define TMC220X_NUM 2

void moveStepUart(uint8_t num,uint64_t Step);

void moveToUART(uint8_t num,uint8_t DIR_Flag, uint32_t moveDistance);

void initTMC2209(uint8_t num,UART_HandleTypeDef *uart, uint8_t mres,
                 uint16_t maxDistance, uint8_t minMoveDistance,
                 GPIO_TypeDef *STEP_Port, uint16_t STEP_GPIO_Pin,
                 GPIO_TypeDef *EN_Port, uint16_t EN_GPIO_Pin, GPIO_TypeDef *DIAG_Port,
                 uint16_t DIAG_GPIO_Pin, uint8_t stallGuardThreshold, uint8_t stallGuardDirection);

void stopMotor(uint8_t num);

void TMC220X_Callback(uint16_t GPIO_Pin);

//extern uint8_t stop;
//extern TMC2209 tmc2209[];
#endif //TMC2209_H
