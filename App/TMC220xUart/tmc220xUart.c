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

// ˳ʱ����ת
uint8_t dir_right[8] = {0x05, 0x00, 0x80, 0x00, 0x00, 0x00, 0x89};
// ��ʱ����ת
uint8_t dir_left[8] = {0x05, 0x00, 0x80, 0x00, 0x00, 0x00, 0x81};

#define usart2RXBUFFERSIZE   1 //�����С
extern uint8_t usart2aRxBuffer[usart2RXBUFFERSIZE];
extern uint8_t USART_RX_BUF[];

uint32_t stepsPerRevolution;

extern void UART_clear();

/*!
 * CRCУ��
 * @param datagram ����
 * @param datagramLength ���鳤��
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
 * ȡ�����ʹ��
 */
void stopMotor(uint8_t num) {
	HAL_GPIO_WritePin(tmc2209[num].EN_Port, tmc2209[num].EN_GPIO_Pin, GPIO_PIN_SET);
}

/*!
 * ���ʹ��
 */
void startMotor(uint8_t num) {
	HAL_GPIO_WritePin(tmc2209[num].EN_Port, tmc2209[num].EN_GPIO_Pin, GPIO_PIN_RESET);
}

/*!
 * ����д�Ĵ���
 * @param address �Ĵ�����ַ
 * @param value д������
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
 * ��PWM
 * @param DIR_Flag ת������
 * @param Step ת������ һȦ360�� ��ת�Ƕ�=�����/ϸ����
 */
void moveStepUart(uint8_t num,uint64_t Step) {
	uint64_t i;
	if(tmc2209[num].ZERO_flag){
		uint16_t  add= TMC_len;
		for (i = 0; i < Step; i++) {
			static uint8_t  addc=0;
			//��������
			HAL_GPIO_WritePin(tmc2209[num].STEP_Port, tmc2209[num].STEP_GPIO_Pin, GPIO_PIN_RESET);
			delay_us(10+add);
			HAL_GPIO_WritePin(tmc2209[num].STEP_Port, tmc2209[num].STEP_GPIO_Pin, GPIO_PIN_SET);
			delay_us(10+add);
			addc++;  //�ö�ʱ����Ч�������
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
 * �ƶ���ָ��λ��
 * @param DIR_Flag �ƶ����� 1��ת 0��ת
 * @param moveDistance �ƶ����� mm
 */
void moveToUART(uint8_t num,uint8_t DIR_Flag, uint32_t moveDistance) {
	// ���ת��һȦ��Ҫ�Ĳ���
	double steps = ((360 * (double) tmc2209[num].div) / tmc2209[num].stepAngle);
	// ������Ҫ����Ȧ����ת��ָ��λ��
	double nums = ((double) moveDistance / (double) tmc2209[num].minMoveDistance);
	//���ʹ��
	startMotor(num);
	HAL_Delay(10);
	if (DIR_Flag) {
		//��ת
		calcCrc(dir_right, 8);
		HAL_UART_Transmit(tmc2209[num].uart, (uint8_t *) dir_right, 8, 0xff);
//		HAL_GPIO_WritePin(tmc2209[num].DIR_Port, tmc2209[num].DIR_GPIO_Pin, GPIO_PIN_SET);
	} else {
		//��ת
		calcCrc(dir_left, 8);
		HAL_UART_Transmit(tmc2209[num].uart, (uint8_t *) dir_left, 8, 0xff);
//		HAL_GPIO_WritePin(tmc2209[num].DIR_Port, tmc2209[num].DIR_GPIO_Pin, GPIO_PIN_RESET);
	}
	moveStepUart(num,(uint64_t) (steps * nums));

	HAL_Delay(10);
	//�����ʹ��
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

/*��
 * ����ȫ��״̬
 */
void clearGSTAT(uint8_t num) {
	uint32_t data = readReg(num,0x01);
	data = data | 0x03;
	uartWriteInt(num,0x01, data);
}

/*!
 * ������2Ϊ�׵Ķ���
 * @param x ��������
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
 * ����ն��ģʽ 0����ն�� 1����ն��
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
 * ����ϸ��ģʽ
 * @param EN 0�ⲿϸ�� 1�ڲ�ϸ��
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
 * �����ڲ�ϸ����
 * @param mres ϸ���� 256, 128, 64, 32, 16, 8, 4, 2, FULLSTEP
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
 * ���ü��ٶ� TODO: ֻд�˼��ٶ�Ϊ0�����
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
 * ��������λ����
 * @param threshold ��ײ�����ֵ
 */
void stallGuard(uint8_t num,uint32_t threshold) {
	setSpreadCycle(num,0);
	uartWriteInt(num,0x40, threshold);
	uartWriteInt(num,0x14, 2000);
	setVactualRpm(num,30);
}

/*!
 * ��ʼ��2209
 * @param uart ���ڵ�ַ
 * @param mres ϸ����
 * @param maxDistance ˿������г�
 * @param minMoveDistance ���һȦ��С�г�
 * @param STEP_Port ��������Port
 * @param STEP_GPIO_Pin �������ź�
 * @param EN_Port ʹ��Port
 * @param EN_GPIO_Pin ʹ�����ź�
 * @param DIAG_Port DIAG
 * @param DIAG_GPIO_Pin DIAG
 * @param stallGuardThreshold ��ײ������
 * @param stallGuardDirection ���㷽��
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
	// ϸ��
	setMicrosteppingResolution(num,mres);
//	while (1);
	// �����˴� ��ֵ ��ײ
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