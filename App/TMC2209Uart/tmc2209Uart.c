//
// Created by mores on 2022/11/7.
//
#include "tmc2209Uart.h"
#include "stdio.h"
#include "stdarg.h"
#include "tools.h"
#include "usart.h"

TMC2209 customTMC;


// 顺时针旋转 开启外部细分模式 关闭PDN UART
//                       同步     寄存器地址   写模式                                0000001000 CRC8输入反转校验
char dir_right[8] = {0x05, 0x00, 0x80, 0x00, 0x00, 0x00, 0x89};
// 逆时针旋转 外部细分模式                                                                    0000000000
char dir_left[8] = {0x05, 0x00, 0x80, 0x00, 0x00, 0x00, 0x81};

char div[8] = {0x05, 0x00, 0x80 | 0x6c, 0x38, 0x00, 0x00, 0x00};

#define usart2RXBUFFERSIZE   1 //缓存大小
extern uint8_t usart2aRxBuffer[usart2RXBUFFERSIZE];
extern uint8_t USART_RX_BUF[];

uint32_t steps_per_revolution;

uint8_t stop = 0;

void swuart_calcCRC(uint8_t *datagram, uint8_t datagramLength) {
	int i, j;
	uint8_t *crc = datagram + (datagramLength - 1);
	uint8_t currentByte;
	*crc = 0;
	for (i = 0; i < (datagramLength - 1); i++) {
		currentByte = datagram[i];
		for (j = 0; j < 8; j++) {
			if ((*crc >> 7) ^ (currentByte & 0x01)) { *crc = (*crc << 1) ^ 0x07; }
			else { *crc = (*crc << 1); }
			currentByte = currentByte >> 1;
		}
	}
}

void UART_writeInt(unsigned char address, unsigned int value) {
	uint8_t writeData[8];

	writeData[0] = 0x05;                         // Sync byte
	writeData[1] = 0x00;                        // Slave address
	writeData[2] = address | 0x80;      // Register address with write bit set
	writeData[3] = value >> 24;                  // Register Data
	writeData[4] = value >> 16;                  // Register Data
	writeData[5] = value >> 8;                   // Register Data
	writeData[6] = value & 0xFF;                 // Register Data
	swuart_calcCRC(writeData, 8);    // Cyclic redundancy check

	//printf("CRC=%x\n",writeData[7]);
	//serialFlush (fd);                  //清空缓冲区
	HAL_UART_Transmit(customTMC.uart, (uint8_t *) writeData, 8, 0xff);
}

/*!
 * 软PWM
 * @param DIR_Flag 转动方向
 * @param Step 转动步数 一圈360° 旋转角度=步距角/细分数
 */
void MoveStepUart(uint64_t Step) {
	uint64_t i;
	for (i = 0; i < Step; i++) {
		//发送脉冲
		HAL_GPIO_WritePin(customTMC.STEP_Port, customTMC.STEP_GPIO_Pin, GPIO_PIN_RESET);
		delay_us(10);
		HAL_GPIO_WritePin(customTMC.STEP_Port, customTMC.STEP_GPIO_Pin, GPIO_PIN_SET);
		delay_us(10);
		if (stop)break;
	}
}

/*!
 * 移动到指定位置
 * @param DIR_Flag 移动方向 1正转 0反转
 * @param moveDistance 移动距离 mm
 */
void moveToUART(uint8_t DIR_Flag, uint32_t moveDistance) {
	// 电机转动一圈需要的步数
	double steps = ((360 * (double) customTMC.div) / customTMC.stepAngle);
	// 计算需要多少圈才能转到指定位置
	double nums = ((double) moveDistance / (double) customTMC.minMoveDistance);

	//电机使能
	HAL_GPIO_WritePin(customTMC.EN_Port, customTMC.EN_GPIO_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	if (DIR_Flag) {
		//正转
		swuart_calcCRC(dir_right, 8);
		HAL_UART_Transmit(customTMC.uart, (uint8_t *) dir_right, 8, 0xff);
//		HAL_GPIO_WritePin(customTMC.DIR_Port, customTMC.DIR_GPIO_Pin, GPIO_PIN_SET);
	} else {
		//反转
		swuart_calcCRC(dir_left, 8);
		HAL_UART_Transmit(customTMC.uart, (uint8_t *) dir_left, 8, 0xff);
//		HAL_GPIO_WritePin(customTMC.DIR_Port, customTMC.DIR_GPIO_Pin, GPIO_PIN_RESET);
	}
	MoveStepUart((uint64_t) (steps * nums));

	HAL_Delay(10);
	//电机非使能
	HAL_GPIO_WritePin(customTMC.EN_Port, customTMC.EN_GPIO_Pin, GPIO_PIN_SET);
}


uint32_t readReg(uint8_t regAddr) {
	uint8_t send[4] = {0x55, 0x00, regAddr};
	uint32_t cache;
	swuart_calcCRC(send, 4);

	HAL_UART_Transmit(customTMC.uart, send, 4, 0xff);
	delay_us(5);
	HAL_UART_Transmit(&huart6, USART_RX_BUF, 15, 0xff);
	cache = (USART_RX_BUF[7] << 24) + (USART_RX_BUF[8] << 16) + (USART_RX_BUF[9] << 8) + USART_RX_BUF[10];
	return cache;
}

void clearGSTAT() {
	uint32_t data = readReg(0x01);
	data = data | 0x03;
	UART_writeInt(0x01, data);
}

int myround(float num) {
	int b = (int) ((num * 10) + 5) / 10;
	return b;
}


int FastLog2(int x) {
	float fx;
	unsigned long ix, exp;

	fx = (float) x;
	ix = *(unsigned long *) &fx;
	exp = (ix >> 23) & 0xFF;

	return exp - 127;
}

//
void set_spreadcycle(uint8_t en_spread) {
	uint32_t data = readReg(0x00);
	if (en_spread) {
		data = data | (1 << 2);
	} else {
		data = data & ~(1 << 2);
	}
	UART_writeInt(0x00, data);
}

//
void set_mstep_resolution_reg_select(uint8_t EN) {
	uint32_t data = readReg(0x00);
	if (EN) {
		data = data | (1 << 7);
	} else {
		data = data & ~(1 << 7);
	}
	UART_writeInt(0x00, data);
}

//
void set_microstepping_resolution(uint8_t msres) {
	uint32_t msres0 = 1 << 24;
	uint32_t msres1 = 1 << 25;
	uint32_t msres2 = 1 << 26;
	uint32_t msres3 = 1 << 27;
	uint32_t intpol = 1 << 28;
	uint32_t chopconf = readReg(0x6c);
	chopconf = chopconf & (~msres0 | ~msres1 | ~msres2 | ~msres3);
	uint32_t msresdezimal = FastLog2(msres);
	msresdezimal = 8 - msresdezimal;
	chopconf = (int) chopconf & (int) 4043309055;
	chopconf = chopconf | msresdezimal << 24;
	UART_writeInt(0x6c, chopconf);
	set_mstep_resolution_reg_select(1);
	steps_per_revolution = 200 * msres;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == DIAG_Pin) {
		stop = 1;
		HAL_GPIO_WritePin(customTMC.EN_Port, customTMC.EN_GPIO_Pin, GPIO_PIN_SET);
	}
}

uint8_t set_vactual(uint32_t vactual, int acceleration) {
	stop = 0;

	if (acceleration == 0) {
		UART_writeInt(0x22, vactual);
	}
}

//
uint8_t set_vactual_rps(float rps) {
	float vactual = rps / 0.715 * steps_per_revolution;
	return set_vactual((uint32_t) myround(vactual), 0);
}

uint8_t set_vactual_rpm(uint32_t rpm) {
	return set_vactual_rps(rpm / 60);
}


void do_homing(uint32_t threshold) {

	set_spreadcycle(0);
	UART_writeInt(0x40, threshold);
	UART_writeInt(0x14, 2000);
	set_vactual_rpm(30);
}

/*!
 * 初始化TMC2209
 */
void initTMC2209(UART_HandleTypeDef *uart, uint8_t msres,
                 uint16_t maxDistance, uint8_t minMoveDistance,
                 GPIO_TypeDef *STEP_Port, uint16_t STEP_GPIO_Pin,
                 GPIO_TypeDef *EN_Port, uint16_t EN_GPIO_Pin) {

	customTMC.uart = uart;
	customTMC.STEP_Port = STEP_Port;
	customTMC.STEP_GPIO_Pin = STEP_GPIO_Pin;
	customTMC.EN_Port = EN_Port;
	customTMC.EN_GPIO_Pin = EN_GPIO_Pin;
	customTMC.div = msres;
	customTMC.stepAngle = 1.8;
	customTMC.maxDistance = maxDistance;
	customTMC.minMoveDistance = minMoveDistance;

	clearGSTAT();
	// 细分
	set_microstepping_resolution(msres);
	// 调整此处 阈值 碰撞
	do_homing(80);
	moveToUART(0, 203);
	HAL_Delay(500);
	stop = 0;
	do_homing(60);
}

//
//void set_internal_rsense(uint8_t EN) {
//	uint32_t data = readReg(0x00);
//	if (EN) {
//		data = data | (1 << 1);
//	} else {
//		data = data & ~(1 << 1);
//	}
//	UART_writeInt(0x00, data);
//}
//void setDirection(int DIR) {
//	uint32_t data = readReg(0x00);
//	if (DIR) {
//		data = data | (1 << 3);
//	} else {
//		data = data & ~(1 << 3);
//	}
//	UART_writeInt(0x00, data);
//}
//
//
//void setIScaleAnalog(uint8_t EN) {
//	uint32_t data = readReg(0x00);
//	if (EN) {
//		data = data | (1 << 0);
//	} else {
//		data = data & ~(1 << 0);
//	}
//	UART_writeInt(0x00, data);
//}
//
//void setvsense(uint8_t EN) {
//	uint32_t data = readReg(0x6c);
//	if (EN) {
//		data = data | (1 << 17);
//	} else {
//		data = data & ~(1 << 17);
//	}
//	UART_writeInt(0x6c, data);
//}
//
//
//void set_pdn_disable(uint8_t pdn_disable) {
//	uint32_t data = readReg(0x00);
//	if (pdn_disable) {
//		data = data | (1 << 6);
//	} else {
//		data = data & ~(1 << 6);
//	}
//	UART_writeInt(0x00, data);
//}
//
//void set_irun_ihold(int IHold, int IRun, int ihold_delay) {
//	uint32_t ihold_irun = 0;
//	ihold_irun = ihold_irun | IHold << 0;
//	ihold_irun = ihold_irun | IRun << 8;
//	ihold_irun = ihold_irun | ihold_delay << 16;
//	UART_writeInt(0x10, ihold_irun);
//}
//
//void setCurrent(float run_current) {
//	float cs_irun = 0;
//	float rsense = 0.11;
//	float Vref = 2.5;
//	float vfs = 0;
//	float hold_current_multiplier = 0.5;
//	float hold_current_delay = 10;
//
//
//	setIScaleAnalog(0);
//	vfs = 0.325 * Vref / 2.5;
//	cs_irun = 32.0 * 1.41421 * run_current / 1000.0 * (rsense + 0.02) / vfs - 1;
//	if (cs_irun < 16) {
//		vfs = 0.180 * Vref / 2.5;
//		cs_irun = 32.0 * 1.41421 * run_current / 1000.0 * (rsense + 0.02) / vfs - 1;
//		setvsense(1);
//	} else {
//		setvsense(0);
//	}
//
//	cs_irun = cs_irun < 31 ? cs_irun : 31;
//	cs_irun = cs_irun > 0 ? cs_irun : 31;
//
//	float CS_IHold = hold_current_multiplier * cs_irun;
//
//	cs_irun = myround(cs_irun);
//	CS_IHold = myround(CS_IHold);
//	hold_current_delay = myround(hold_current_delay);
//	set_irun_ihold(CS_IHold, cs_irun, hold_current_delay);
//	set_pdn_disable(1);
//}
//
//
//void set_interpolation(uint8_t EN) {
//	uint32_t data = readReg(0x6c);
//	if (EN) {
//		data = data | (1 << 28);
//	} else {
//		data = data & ~(1 << 28);
//	}
//	UART_writeInt(0x6c, data);
//}
//