# PT100温度传感器

```c
#include "max31865.h"

...
int main(void) {
  ...
  //	// 初始化Max31865 设置4线
    MAX31865_GPIO max_gpio;
    max_gpio.MISO_PIN = SDO_Pin;
    max_gpio.MISO_PORT = SDO_GPIO_Port;
    max_gpio.MOSI_PIN = SDI_Pin;
    max_gpio.MOSI_PORT = SDI_GPIO_Port;
    max_gpio.CLK_PIN = CLK_Pin;
    max_gpio.CLK_PORT = CLK_GPIO_Port;
    max_gpio.CE_PIN = CS_Pin;
    max_gpio.CE_PORT = CS_GPIO_Port;
    MAX31865_init(&max_gpio, 4);
  ...
  while(1) {

    if (sendFlag) {
        float temp = MAX31865_readTemp();
        print("%f\r\n", temp);
        sendFlag = 0;
    }
  }
}
```