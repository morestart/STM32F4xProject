# Motor

## 步进电机

```c
int main(void)
{
  ......................
  MX_GPIO_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
	// 初始化延时
	delay_init(168);
	// 初始化步进电机
	MOTOR motor = {2, 64, 200, 1.8,
	               STEP_GPIO_Port, STEP_Pin,
	               DIR_GPIO_Port, DIR_Pin,
	               EN_GPIO_Port, EN_Pin,
	               MS1_GPIO_Port, MS1_Pin,
	               MS2_GPIO_Port, MS2_Pin
	};
	initMotorSubDivision(&motor);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */
    moveTo(1, 100);
    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}
```