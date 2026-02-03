/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define GPIO_OUTPUT0_Pin 		GPIO_PIN_0
#define GPIO_OUTPUT0_GPIO_Port 	GPIOF
#define GPIO_OUTPUT1_Pin 		GPIO_PIN_1
#define GPIO_OUTPUT1_GPIO_Port 	GPIOF
#define GPIO_OUTPUT2_Pin 		GPIO_PIN_2
#define GPIO_OUTPUT2_GPIO_Port 	GPIOF
#define GPIO_OUTPUT3_Pin 		GPIO_PIN_3
#define GPIO_OUTPUT3_GPIO_Port 	GPIOF
#define GPIO_OUTPUT4_Pin 		GPIO_PIN_4
#define GPIO_OUTPUT4_GPIO_Port 	GPIOF
#define GPIO_OUTPUT5_Pin 		GPIO_PIN_5
#define GPIO_OUTPUT5_GPIO_Port 	GPIOF
#define GPIO_OUTPUT6_Pin 		GPIO_PIN_8
#define GPIO_OUTPUT6_GPIO_Port 	GPIOF
#define GPIO_OUTPUT7_Pin 		GPIO_PIN_9
#define GPIO_OUTPUT7_GPIO_Port 	GPIOF

#define GPIO_INPUT0_Pin 		GPIO_PIN_10
#define GPIO_INPUT0_GPIO_Port 	GPIOD
#define GPIO_INPUT1_Pin 		GPIO_PIN_11
#define GPIO_INPUT1_GPIO_Port 	GPIOD
#define GPIO_INPUT2_Pin 		GPIO_PIN_12
#define GPIO_INPUT2_GPIO_Port	GPIOD
#define GPIO_INPUT3_Pin 		GPIO_PIN_13
#define GPIO_INPUT3_GPIO_Port 	GPIOD

#define DRDY_INPUT_Pin 			GPIO_PIN_1
#define DRDY_INPUT_GPIO_Port 	GPIOB

#define DIP_SW0_Pin 			GPIO_PIN_11
#define DIP_SW0_GPIO_Port 		GPIOF
#define DIP_SW1_Pin 			GPIO_PIN_12
#define DIP_SW1_GPIO_Port 		GPIOF
#define DIP_SW2_Pin 			GPIO_PIN_13
#define DIP_SW2_GPIO_Port 		GPIOF
#define DIP_SW3_Pin 			GPIO_PIN_14
#define DIP_SW3_GPIO_Port 		GPIOF
#define DIP_SW4_Pin 			GPIO_PIN_15
#define DIP_SW4_GPIO_Port 		GPIOF
#define DIP_SW5_Pin 			GPIO_PIN_0
#define DIP_SW5_GPIO_Port 		GPIOG
#define DIP_SW6_Pin 			GPIO_PIN_1
#define DIP_SW6_GPIO_Port 		GPIOG
#define DIP_SW7_Pin 			GPIO_PIN_7
#define DIP_SW7_GPIO_Port 		GPIOE

#define DISP_SCLK_Pin 			GPIO_PIN_15		//CPLD CLOCK
#define DISP_SCLK_GPIO_Port 	GPIOB

#define IOT_WAKEUP_Pin 			GPIO_PIN_6		//IoT WakeUp Signal
#define IOT_WAKEUP_GPIO_Port 	GPIOB


#define DISP_BRT_R_Pin 			GPIO_PIN_9
#define DISP_BRT_R_GPIO_Port 	GPIOE
#define DISP_BRT_G_Pin 			GPIO_PIN_11
#define DISP_BRT_G_GPIO_Port 	GPIOE
#define DISP_BRT_B_Pin 			GPIO_PIN_13
#define DISP_BRT_B_GPIO_Port 	GPIOE

#define DISP_SCAN_A_Pin 		GPIO_PIN_4
#define DISP_SCAN_A_GPIO_Port 	GPIOG
#define DISP_SCAN_B_Pin 		GPIO_PIN_6
#define DISP_SCAN_B_GPIO_Port 	GPIOG
#define DISP_SCAN_C_Pin 		GPIO_PIN_8
#define DISP_SCAN_C_GPIO_Port 	GPIOG
#define DISP_SCAN_D_Pin 		GPIO_PIN_14
#define DISP_SCAN_D_GPIO_Port 	GPIOD

#define DISP_DLAT_Pin 			GPIO_PIN_15
#define DISP_DLAT_GPIO_Port 	GPIOD
#define DISP_DCLK_Pin			GPIO_PIN_2
#define DISP_DCLK_GPIO_Port 	GPIOG

#define DISP_DHS_Pin 			GPIO_PIN_14
#define DISP_DHS_GPIO_Port 		GPIOD
#define DISP_DVS_Pin 			GPIO_PIN_15
#define DISP_DVS_GPIO_Port 		GPIOD

#define DISP_DATA_R_Pin 		GPIO_PIN_3
#define DISP_DATA_R_GPIO_Port 	GPIOG
#define DISP_DATA_G_Pin 		GPIO_PIN_5
#define DISP_DATA_G_GPIO_Port 	GPIOG
#define DISP_DATA_B_Pin 		GPIO_PIN_7
#define DISP_DATA_B_GPIO_Port 	GPIOG

#define SDIO_POWER_Pin 			GPIO_PIN_3
#define SDIO_POWER_GPIO_Port 	GPIOD
#define SDIO_DETECT_Pin 		GPIO_PIN_4
#define SDIO_DETECT_GPIO_Port 	GPIOD


#define LAMP_LED0_Pin 			GPIO_PIN_6
#define LAMP_LED0_GPIO_Port 	GPIOD
#define LAMP_LED1_Pin 			GPIO_PIN_7
#define LAMP_LED1_GPIO_Port 	GPIOD
#define LAMP_LED2_Pin 			GPIO_PIN_9
#define LAMP_LED2_GPIO_Port 	GPIOG
#define LAMP_LED3_Pin 			GPIO_PIN_10
#define LAMP_LED3_GPIO_Port 	GPIOG
#define LAMP_LED4_Pin 			GPIO_PIN_11
#define LAMP_LED4_GPIO_Port 	GPIOG
#define LAMP_LED5_Pin 			GPIO_PIN_12
#define LAMP_LED5_GPIO_Port 	GPIOG
#define LAMP_LED6_Pin 			GPIO_PIN_13
#define LAMP_LED6_GPIO_Port 	GPIOG
#define LAMP_LED7_Pin 			GPIO_PIN_14
#define LAMP_LED7_GPIO_Port 	GPIOG

#define FRAM_WP_Pin 			GPIO_PIN_7
#define FRAM_WP_GPIO_Port 		GPIOB
/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
