//
// Created by mores on 2022/11/7.
//
#include <string.h>
#include "tmc220xUart.h"
#include "stdio.h"
#include "tools.h"
#include "usart.h"
#include "math.h"

TMC2209 tmc2209[TMC220X_NUM];

// 顺时针旋转
uint8_t dir_right[8] = {0x05, 0x00, 0x80, 0x00, 0x00, 0x00, 0x89};
// 逆时针旋转
uint8_t dir_left[8] = {0x05, 0x00, 0x80, 0x00, 0x00, 0x00, 0x81};

#define usart2RXBUFFERSIZE   1 //缓存大小
extern uint8_t usart2aRxBuffer[usart2RXBUFFERSIZE];
extern uint8_t USART_RX_BUF[];

uint32_t stepsPerRevolution;

extern void UART_clear();

/*!
 * CRC校验
 * @param datagram 数组
 * @param datagramLength 数组长度
 */
void calcCrc(uint8_t *datagram, uint8_t datagramLength) {
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

/*!
 * 取消电机使能
 */
void stopMotor(uint8_t num) {
	HAL_GPIO_WritePin(tmc2209[num].EN_Port, tmc2209[num].EN_GPIO_Pin, GPIO_PIN_SET);
}

/*!
 * 电机使能
 */
void startMotor(uint8_t num) {
	HAL_GPIO_WritePin(tmc2209[num].EN_Port, tmc2209[num].EN_GPIO_Pin, GPIO_PIN_RESET);
}

/*!
 * 串口写寄存器
 * @param address 寄存器地址
 * @param value 写入数据
 */
void uartWriteInt(uint8_t num,unsigned char address, unsigned int value) {
	uint8_t writeData[8];

	writeData[0] = 0x05;                              // Sync byte
	writeData[1] = 0x00;                              // Slave address
	writeData[2] = address | 0x80;                    // Register address with write bit set
	writeData[3] = value >> 24;                       // Register Data
	writeData[4] = value >> 16;                       // Register Data
	writeData[5] = value >> 8;                        // Register Data
	writeData[6] = value & 0xFF;                      // Register Data
	calcCrc(writeData, 8);    // Cyclic redundancy check
	HAL_Delay(100);
	HAL_UART_Transmit(tmc2209[num].uart, (uint8_t *) writeData, 8, 0xff);
	delay_us(5);
	HAL_UART_Transmit(&huart6, USART_RX_BUF, 15, 0xff);
	UART_clear();
	HAL_Delay(100);
}

/*!
 * 软PWM
 * @param DIR_Flag 转动方向
 * @param Step 转动步数 一圈360° 旋转角度=步距角/细分数
 */
void moveStepUart(uint8_t num,uint64_t Step) {
	uint64_t i;
	if(tmc2209[num].ZERO_flag){
		uint16_t  add= TMC_len;
		for (i = 0; i < Step; i++) {
			static uint8_t  addc=0;
			//发送脉冲
			HAL_GPIO_WritePin(tmc2209[num].STEP_Port, tmc2209[num].STEP_GPIO_Pin, GPIO_PIN_RESET);
			delay_us(10+add);
			HAL_GPIO_WritePin(tmc2209[num].STEP_Port, tmc2209[num].STEP_GPIO_Pin, GPIO_PIN_SET);
			delay_us(10+add);
			addc++;  //用定时器加效果会更好
			if((Step-TMC_len*TMC_acc<=i)&&(addc==TMC_acc)){
				addc=0;
				add++;
			}
			else if((addc==TMC_acc)&&(add>0)){
				addc=0;
				add--;
			}

			if (tmc2209[num].stop)break;
		}
	} else{
		for (i = 0; i < Step; i++) {
			HAL_GPIO_WritePin(tmc2209[num].STEP_Port, tmc2209[num].STEP_GPIO_Pin, GPIO_PIN_RESET);
			delay_us(10);
			HAL_GPIO_WritePin(tmc2209[num].STEP_Port, tmc2209[num].STEP_GPIO_Pin, GPIO_PIN_SET);
			delay_us(10);
			if (tmc2209[num].stop)break;
		}
	}

}

/*!
 * 移动到指定位置
 * @param DIR_Flag 移动方向 1正转 0反转
 * @param moveDistance 移动距离 mm
 */
void moveToUART(uint8_t num,uint8_t DIR_Flag, uint32_t moveDistance) {
	// 电机转动一圈需要的步数
	double steps = ((360 * (double) tmc2209[num].div) / tmc2209[num].stepAngle);
	// 计算需要多少圈才能转到指定位置
	double nums = ((double) moveDistance / (double) tmc2209[num].minMoveDistance);
	//电机使能
	startMotor(num);
	HAL_Delay(10);
	if (DIR_Flag) {
		//正转
		calcCrc(dir_right, 8);
		HAL_UART_Transmit(tmc2209[num].uart, (uint8_t *) dir_right, 8, 0xff);
//		HAL_GPIO_WritePin(tmc2209[num].DIR_Port, tmc2209[num].DIR_GPIO_Pin, GPIO_PIN_SET);
	} else {
		//反转
		calcCrc(dir_left, 8);
		HAL_UART_Transmit(tmc2209[num].uart, (uint8_t *) dir_left, 8, 0xff);
//		HAL_GPIO_WritePin(tmc2209[num].DIR_Port, tmc2209[num].DIR_GPIO_Pin, GPIO_PIN_RESET);
	}
	moveStepUart(num,(uint64_t) (steps * nums));

	HAL_Delay(10);
	//电机非使能
	stopMotor(num);
}

//55 00 01 46
uint32_t readReg(uint8_t num,uint8_t regAddr) {  //05 FF 01    00 00 00 00   13       00 00 00
	uint8_t send[4] = {0x55, 0x00, regAddr};
	uint32_t cache;
	calcCrc(send, 4);

	HAL_UART_Transmit(tmc2209[num].uart, send, 4, 0xff);
	delay_us(5);
	HAL_UART_Transmit(&huart6, USART_RX_BUF, 15, 0xff);
	cache = (USART_RX_BUF[7] << 24) + (USART_RX_BUF[8] << 16) + (USART_RX_BUF[9] << 8) + USART_RX_BUF[10];
	UART_clear();
	return cache;
}

/*！
 * 重设全局状态
 */
void clearGSTAT(uint8_t num) {
	uint32_t data = readReg(num,0x01);
	data = data | 0x03;
	uartWriteInt(num,0x01, data);
}

/*!
 * 计算以2为底的对数
 * @param x 计算数据
 * @return
 */
int fastLog2(int x) {
	float fx;
	unsigned long ix, exp;

	fx = (float) x;
	ix = *(unsigned long *) &fx;
	exp = (ix >> 23) & 0xFF;

	return exp - 127;
}

/*!
 * 设置斩波模式 0静音斩波 1高速斩波
 * @param en_spread
 */
void setSpreadCycle(uint8_t num,uint8_t en_spread) {
	uint32_t data = readReg(num,0x00);
	if (en_spread) {
		data = data | (1 << 2);
	} else {
		data = data & ~(1 << 2);
	}
	uartWriteInt(num,0x00, data);
}

/*!
 * 设置细分模式
 * @param EN 0外部细分 1内部细分
 */
void setStepResolutionRegSelect(uint8_t num,uint8_t EN) {
	uint32_t data = readReg(num,0x00);
	if (EN) {
		data = data | (1 << 7);
	} else {
		data = data & ~(1 << 7);
	}
	uartWriteInt(num,0x00, data);
}

/*!
 * 设置内部细分数
 * @param mres 细分数 256, 128, 64, 32, 16, 8, 4, 2, FULLSTEP
 */
void setMicrosteppingResolution(uint8_t num,uint8_t mres) {
//	uint32_t msres0 = 1 << 24;
//	uint32_t msres1 = 1 << 25;
//	uint32_t msres2 = 1 << 26;
//	uint32_t msres3 = 1 << 27;
//	uint32_t intpol = 1 << 28;
	uint32_t chopconf = readReg(num,0x6c);
//	chopconf = chopconf & (~msres0 | ~msres1 | ~msres2 | ~msres3);

	uint32_t msresdezimal = fastLog2(mres);
	msresdezimal = 8 - msresdezimal;
	chopconf = chopconf & 0xF0FFFFFF;
	chopconf = chopconf | msresdezimal << 24;
	uartWriteInt(num,0x6c, chopconf);
	setStepResolutionRegSelect(num,0);
	stepsPerRevolution = 200 * mres;
	tmc2209[num].div = mres;
}

/*!
 * 设置加速度 TODO: 只写了加速度为0的情况
 * @param vactual 
 * @param acceleration 
 * @return 
 */
void setVactual(uint8_t num,uint32_t vactual, int acceleration) {
	tmc2209[num].stop = 0;

	if (acceleration == 0) {
		uartWriteInt(num,0x22, vactual);
	}
}

//
void setVactualRps(uint8_t num,float rps) {
	float vactual = rps / 0.715 * stepsPerRevolution;
	setVactual(num,(uint32_t) roundf(vactual), 0);
}

void setVactualRpm(uint8_t num,uint32_t rpm) {
	setVactualRps(num,rpm / 60);
}

/*!
 * 设置无限位归零
 * @param threshold 碰撞检测阈值
 */
void stallGuard(uint8_t num,uint32_t threshold) {
	setSpreadCycle(num,0);
	uartWriteInt(num,0x40, threshold);
	uartWriteInt(num,0x14, 2000);
	setVactualRpm(num,30);
}

/*!
 * 初始化2209
 * @param uart 串口地址
 * @param mres 细分数
 * @param maxDistance 丝杆最大行程
 * @param minMoveDistance 电机一圈最小行程
 * @param STEP_Port 脉冲引脚Port
 * @param STEP_GPIO_Pin 脉冲引脚号
 * @param EN_Port 使能Port
 * @param EN_GPIO_Pin 使能引脚号
 * @param DIAG_Port DIAG
 * @param DIAG_GPIO_Pin DIAG
 * @param stallGuardThreshold 碰撞灵敏度
 * @param stallGuardDirection 归零方向
 */
void initTMC2209(uint8_t num,UART_HandleTypeDef *uart, uint8_t mres,
                 uint16_t maxDistance, uint8_t minMoveDistance,
                 GPIO_TypeDef *STEP_Port, uint16_t STEP_GPIO_Pin,
                 GPIO_TypeDef *EN_Port, uint16_t EN_GPIO_Pin, GPIO_TypeDef *DIAG_Port,
                 uint16_t DIAG_GPIO_Pin, uint8_t stallGuardThreshold, uint8_t stallGuardDirection) {

	tmc2209[num].uart = uart;
	tmc2209[num].STEP_Port = STEP_Port;
	tmc2209[num].STEP_GPIO_Pin = STEP_GPIO_Pin;
	tmc2209[num].EN_Port = EN_Port;
	tmc2209[num].EN_GPIO_Pin = EN_GPIO_Pin;
	tmc2209[num].div = mres;
	tmc2209[num].stepAngle = 1.8;
	tmc2209[num].maxDistance = maxDistance;
	tmc2209[num].minMoveDistance = minMoveDistance;
	tmc2209[num].DIAG_Port = DIAG_Port;
	tmc2209[num].DIAG_GPIO_Pin = DIAG_GPIO_Pin;
	tmc2209[num].ZERO_flag=0;

	clearGSTAT(num);
	// 细分
	setMicrosteppingResolution(num,mres);
//	while (1);
	// 调整此处 阈值 碰撞
	stallGuard(num,stallGuardThreshold);
	tmc2209[num].stop = 0;
	moveToUART(num,stallGuardDirection, maxDistance);
	HAL_Delay(500);
	tmc2209[num].ZERO_flag=1;
	tmc2209[num].stop = 0;
	clearGSTAT(num);
	setMicrosteppingResolution(0,32);
	stallGuard(num,stallGuardThreshold);
//	stallGuard(60);
}

void set_stop_flag(uint8_t num,uint8_t flag){
	tmc2209[num].stop = flag?1:0;
}

uint8_t get_stop_flag(uint8_t num){
	return tmc2209[num].stop;
}

void TMC220X_Callback(uint16_t GPIO_Pin)
{
	for (uint8_t ii=0;ii<TMC220X_NUM;ii++)
	{
		if(GPIO_Pin== tmc2209[ii].DIAG_GPIO_Pin){
			tmc2209[ii].stop = 1;
			stopMotor(ii);
//			print("P:%x\r\n",GPIO_Pin);
//			tmc2209[ii].stop = 0;
		}
	}
}



//
//void set_internal_rsense(uint8_t EN) {
//	uint32_t data = readReg(0x00);
//	if (EN) {
//		data = data | (1 << 1);
//	} else {
//		data = data & ~(1 << 1);
//	}
//	uartWriteInt(0x00, data);
//}
//void setDirection(int DIR) {
//	uint32_t data = readReg(0x00);
//	if (DIR) {
//		data = data | (1 << 3);
//	} else {
//		data = data & ~(1 << 3);
//	}
//	uartWriteInt(0x00, data);
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
//	uartWriteInt(0x00, data);
//}
//
//void setvsense(uint8_t EN) {
//	uint32_t data = readReg(0x6c);
//	if (EN) {
//		data = data | (1 << 17);
//	} else {
//		data = data & ~(1 << 17);
//	}
//	uartWriteInt(0x6c, data);
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
//	uartWriteInt(0x00, data);
//}
//
//void set_irun_ihold(int IHold, int IRun, int ihold_delay) {
//	uint32_t ihold_irun = 0;
//	ihold_irun = ihold_irun | IHold << 0;
//	ihold_irun = ihold_irun | IRun << 8;
//	ihold_irun = ihold_irun | ihold_delay << 16;
//	uartWriteInt(0x10, ihold_irun);
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
//	uartWriteInt(0x6c, data);
//}
//