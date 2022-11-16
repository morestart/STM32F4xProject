/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define EN1_Pin GPIO_PIN_5
#define EN1_GPIO_Port GPIOA
#define STEP1_Pin GPIO_PIN_6
#define STEP1_GPIO_Port GPIOA
#define DIAG1_Pin GPIO_PIN_7
#define DIAG1_GPIO_Port GPIOA
#define DIAG1_EXTI_IRQn EXTI9_5_IRQn
#define STEP2_Pin GPIO_PIN_4
#define STEP2_GPIO_Port GPIOC
#define EN2_Pin GPIO_PIN_5
#define EN2_GPIO_Port GPIOC
#define DIAG2_Pin GPIO_PIN_0
#define DIAG2_GPIO_Port GPIOB
#define DIAG2_EXTI_IRQn EXTI0_IRQn
#define CS_Pin GPIO_PIN_9
#define CS_GPIO_Port GPIOE
#define SDI_Pin GPIO_PIN_11
#define SDI_GPIO_Port GPIOE
#define SDO_Pin GPIO_PIN_13
#define SDO_GPIO_Port GPIOE
#define CLK_Pin GPIO_PIN_15
#define CLK_GPIO_Port GPIOE
#define Humidifier_Pin GPIO_PIN_12
#define Humidifier_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
