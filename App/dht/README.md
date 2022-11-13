# DHT

```C
#include "DHT.h"
#include <stdio.h>
#include <string.h>
...
int main(void) {
  ...
  static DHT_sensor bedRoom = {GPIOC, GPIO_PIN_1, DHT22, GPIO_PULLUP};
  ...
  while(1) {

    char msg[40]; 

    DHT_data d = DHT_getData(&bedRoom); 

    sprintf(msg, "\fBed room: Temp %2.1f°„C, Hum %2.1f%%", d.temp, d.hum);

    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 0xFF);
  }
}
```