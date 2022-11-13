# OLED

需要使用cubemx初始化i2c

```c
int main(void) {
	......
	
	SSD1306_Init();
	SSD1306_GotoXY(0, 0);
	SSD1306_Puts("OZGE", &Font_11x18, 1);
	SSD1306_GotoXY(0, 30);
	SSD1306_Puts("DURGUT", &Font_11x18, 1);
	SSD1306_UpdateScreen();
	HAL_Delay(500);

	SSD1306_ScrollRight(0, 7);
	HAL_Delay(3000);
	SSD1306_ScrollLeft(0, 7);
	HAL_Delay(3000);
	SSD1306_Stopscroll();
	SSD1306_Clear();

	SSD1306_GotoXY(35, 25);
	SSD1306_Puts("Temp", &Font_11x18, 1);
	SSD1306_UpdateScreen();
	HAL_Delay(1000);
	SSD1306_Clear();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		//	PT100_Temperature = MAX31865_Get_Temperature();
		if (HAL_GetTick() - Time >= 1000) {
			SSD1306_GotoXY(0, 30);
			SSD1306_Puts("xxxx", &Font_16x26, 1);
			SSD1306_UpdateScreen();
			SSD1306_GotoXY(0, 25);
			SSD1306_Puts(temperature, &Font_11x18, 1);
			SSD1306_UpdateScreen();
			HAL_Delay(500);
		}
	}
	/* USER CODE END 3 */
}
```