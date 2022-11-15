# TMC2209 串口模式

## 开启服务

TMC2209 需要开启串口中断以及GPIO中断服务

### 串口中断调用

```c
uint8_t USART_RX_BUF[20];
uint8_t usart2aRxBuffer[1];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
		static uint8_t USART_RX_STA = 0;
		USART_RX_BUF[USART_RX_STA++]=usart2aRxBuffer[0] ;
		if (USART_RX_STA >= 20) {
			USART_RX_STA = 0;
		}
		HAL_UART_Receive_IT(&huart2, (uint8_t *) &usart2aRxBuffer, 1);   //再开启接收中断
	}
}
```

串口接受中断绑定buffer为长度为1的数组。然后将数据转存到设置的buffer中。计数>数组长度时进行归零操作。

### GPIO中断调用

![image-20221113095049300](C:\Users\mores\CLionProjects\STM32F4xProject\assets\image-20221113095049300.png)

设置为上升沿触发，上拉。在gpio.c中加入GPIO中断回调：

```c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == tmc2209.DIAG_GPIO_Pin) {
		stop = 1;
		stopMotor();
		stop = 0;
	}
}
```

在main函数中初始化tmc2209并调用移动函数

```c
int main(void) {
    
	................................................
        
	initTMC2209(
		&huart2, 64, 203, 2,
		STEP_GPIO_Port, STEP_Pin,
		EN_GPIO_Port, EN_Pin,
		DIAG_GPIO_Port, DIAG_Pin
	);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		moveToUART(1, 5);
		HAL_Delay(1000);
		moveToUART(0, 5);
		HAL_Delay(1000);
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}
```

串口接线，单线，需接1k以上电阻，串口模式异步

![img](https://img2022.cnblogs.com/blog/1044695/202208/1044695-20220827173750848-973805823.png)

![image-20221113103840579](C:\Users\mores\CLionProjects\STM32F4xProject\assets\image-20221113103840579.png)