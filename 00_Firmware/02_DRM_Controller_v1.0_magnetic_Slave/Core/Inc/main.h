/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32l0xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RF_WUT_Pin GPIO_PIN_13
#define RF_WUT_GPIO_Port GPIOC
#define ADC_SPARE_Pin GPIO_PIN_2
#define ADC_SPARE_GPIO_Port GPIOA
#define ADC_TEMP_Pin GPIO_PIN_3
#define ADC_TEMP_GPIO_Port GPIOA
#define ADC_BAT_Pin GPIO_PIN_4
#define ADC_BAT_GPIO_Port GPIOA
#define ADC_SOL_Pin GPIO_PIN_5
#define ADC_SOL_GPIO_Port GPIOA
#define PWM1_Pin GPIO_PIN_6
#define PWM1_GPIO_Port GPIOA
#define PWM2_Pin GPIO_PIN_7
#define PWM2_GPIO_Port GPIOA
#define PWM3_Pin GPIO_PIN_0
#define PWM3_GPIO_Port GPIOB
#define PWM4_Pin GPIO_PIN_1
#define PWM4_GPIO_Port GPIOB
#define HIGH_LOW_Pin GPIO_PIN_2
#define HIGH_LOW_GPIO_Port GPIOB
#define RF_B1_B2_Pin GPIO_PIN_12
#define RF_B1_B2_GPIO_Port GPIOB
#define BLE_SLEEP_Pin GPIO_PIN_13
#define BLE_SLEEP_GPIO_Port GPIOB
#define BLE_UART_SW_Pin GPIO_PIN_14
#define BLE_UART_SW_GPIO_Port GPIOB
#define RF_OPT_Pin GPIO_PIN_15
#define RF_OPT_GPIO_Port GPIOB
#define RF_SW_Pin GPIO_PIN_8
#define RF_SW_GPIO_Port GPIOA
#define RF_CH1_Pin GPIO_PIN_11
#define RF_CH1_GPIO_Port GPIOA
#define RF_CH2_Pin GPIO_PIN_12
#define RF_CH2_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_15
#define LED_GPIO_Port GPIOA
#define BLE_GPI_Pin GPIO_PIN_5
#define BLE_GPI_GPIO_Port GPIOB
#define BLE_GPO_Pin GPIO_PIN_6
#define BLE_GPO_GPIO_Port GPIOB
#define I2C1_WP_Pin GPIO_PIN_7
#define I2C1_WP_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
