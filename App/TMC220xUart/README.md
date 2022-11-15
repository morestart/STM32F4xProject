# TMC2209 ����ģʽ

## ��������

TMC2209 ��Ҫ���������ж��Լ�GPIO�жϷ���

### �����жϵ���

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
		HAL_UART_Receive_IT(&huart2, (uint8_t *) &usart2aRxBuffer, 1);   //�ٿ��������ж�
	}
}
```

���ڽ����жϰ�bufferΪ����Ϊ1�����顣Ȼ������ת�浽���õ�buffer�С�����>���鳤��ʱ���й��������

### GPIO�жϵ���

![image-20221113095049300](C:\Users\mores\CLionProjects\STM32F4xProject\assets\image-20221113095049300.png)

����Ϊ�����ش�������������gpio.c�м���GPIO�жϻص���

```c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == tmc2209.DIAG_GPIO_Pin) {
		stop = 1;
		stopMotor();
		stop = 0;
	}
}
```

��main�����г�ʼ��tmc2209�������ƶ�����

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

���ڽ��ߣ����ߣ����1k���ϵ��裬����ģʽ�첽

![img](https://img2022.cnblogs.com/blog/1044695/202208/1044695-20220827173750848-973805823.png)

![image-20221113103840579](C:\Users\mores\CLionProjects\STM32F4xProject\assets\image-20221113103840579.png)