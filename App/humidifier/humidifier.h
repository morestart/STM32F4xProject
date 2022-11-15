//
// Created by mores on 2022/10/21.
//

#ifndef HUMIDIFIER_H
#define HUMIDIFIER_H

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <string.h>
#include <math.h>

typedef struct {
	GPIO_TypeDef *humidifier_Port;
	uint16_t humidifier_GPIO_PIN;
} humidifier;

void initHumidifier(GPIO_TypeDef *humidifier_Port,
                    uint16_t humidifier_GPIO_PIN);

void openHumidifier();

void closeHumidifier();

#endif //HUMIDIFIER_H
