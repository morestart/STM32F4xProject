//
// Created by mores on 2022/10/21.
//
#include "humidifier.h"
#include "stm32f4xx_hal.h"

unsigned char Humflag = 0;

humidifier *my_Humidifier;

// 初始化 给一个高电平
void initHumidifier(humidifier *customHumidifier) {
	my_Humidifier = customHumidifier;
	HAL_GPIO_WritePin(customHumidifier->humidifier_Port, customHumidifier->humidifier_GPIO_PIN, GPIO_PIN_SET);
}


void openHumidifier() {
	if (!Humflag) {
		Humflag = 1;
		HAL_GPIO_WritePin(my_Humidifier->humidifier_Port, my_Humidifier->humidifier_GPIO_PIN, GPIO_PIN_RESET);
		HAL_Delay(500);
		HAL_GPIO_WritePin(my_Humidifier->humidifier_Port, my_Humidifier->humidifier_GPIO_PIN, GPIO_PIN_SET);
	}
}

void closeHumidifier() {
	if (Humflag) {
		Humflag = 0;
		HAL_GPIO_WritePin(my_Humidifier->humidifier_Port, my_Humidifier->humidifier_GPIO_PIN, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(my_Humidifier->humidifier_Port, my_Humidifier->humidifier_GPIO_PIN, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(my_Humidifier->humidifier_Port, my_Humidifier->humidifier_GPIO_PIN, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(my_Humidifier->humidifier_Port, my_Humidifier->humidifier_GPIO_PIN, GPIO_PIN_SET);
	}
}