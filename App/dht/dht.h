//
// Created by mores on 2022/10/21.
//

#ifndef DHT_H
#define DHT_H
#define DHT_TIMEOUT        10000
#define DHT_POLLING_CONTROL      1
#define DHT_POLLING_INTERVAL_DHT11  2000
#define DHT_POLLING_INTERVAL_DHT22  1000
#define DHT_IRQ_CONTROL

#include "stm32f4xx_hal.h"

typedef struct {
	float hum;
	float temp;
} DHT_data;


typedef enum {
	DHT11,
	DHT22
} DHT_type;


typedef struct {
	GPIO_TypeDef *DHT_Port;
	uint16_t DHT_Pin;
	DHT_type type;
	uint8_t pullUp;


#if DHT_POLLING_CONTROL == 1
	uint32_t lastPollingTime;
	float lastTemp;
	float lastHum;
#endif
} DHT_sensor;


DHT_data DHT_getData(DHT_sensor *sensor);

#endif //DHT_H
