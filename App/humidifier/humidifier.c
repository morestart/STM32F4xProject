//
// Created by mores on 2022/10/21.
//
#include "humidifier.h"
#include "stm32f4xx_hal.h"

unsigned char humflag = 0;

humidifier my_Humidifier;

// 初始化 给一个高电平
void initHumidifier(GPIO_TypeDef *humidifier_Port,
                    uint16_t humidifier_GPIO_PIN) {
	my_Humidifier.humidifier_Port = humidifier_Port;
	my_Humidifier.humidifier_GPIO_PIN = humidifier_GPIO_PIN;
	HAL_GPIO_WritePin(humidifier_Port, humidifier_GPIO_PIN, GPIO_PIN_SET);
}


void openHumidifier() {
	if (!humflag) {
		humflag = 1;
		HAL_GPIO_WritePin(my_Humidifier.humidifier_Port, my_Humidifier.humidifier_GPIO_PIN, GPIO_PIN_RESET);
		HAL_Delay(500);
		HAL_GPIO_WritePin(my_Humidifier.humidifier_Port, my_Humidifier.humidifier_GPIO_PIN, GPIO_PIN_SET);
	}
}

void closeHumidifier() {
	if (humflag) {
		humflag = 0;
		HAL_GPIO_WritePin(my_Humidifier.humidifier_Port, my_Humidifier.humidifier_GPIO_PIN, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(my_Humidifier.humidifier_Port, my_Humidifier.humidifier_GPIO_PIN, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(my_Humidifier.humidifier_Port, my_Humidifier.humidifier_GPIO_PIN, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(my_Humidifier.humidifier_Port, my_Humidifier.humidifier_GPIO_PIN, GPIO_PIN_SET);
	}
}