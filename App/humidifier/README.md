# ��ʪ��

```c
#include "humidifier.h"

...
int main(void) {
  ...
  // ��ʼ����ʪ��
  humidifier myhum = {Humidifier_GPIO_Port, Humidifier_Pin};
  initHumidifier(&myhum);
  ...
  while(1) {

    openHumidifier();
    HAL_Delay(1000);
    closeHumidifier();
  }
}
```