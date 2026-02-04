/*
 * display.c
 *
 *  Created on: 2018. 9. 13.
 *      Author: ShinSeungCheol
 */

#include <string.h>
#include "stm32l0xx_hal.h"
#include "cmsis_os.h"
#include "display.h"
#include "logic.h"
#include "main.h"

extern TIM_HandleTypeDef htim3;

#define GRAYSCALE_12BIT         12
#define GRAYSCALE_16BIT         16
#define GRAYSCALE_MODE          GRAYSCALE_16BIT // select

#define GRAYSCALE_START         257
#define GRAYSCALE_END           512
//#define DRIVER_CONFIG           0b1110111111111111
#define DRIVER_CONFIG           0b1110111100000000

#define CMD_LATCH_DATA          0
#define CMD_DOT_CORRECTION      1
#define CMD_ENABLE_OUTPUT       3
#define CMD_ERROR_DETECTION     4
#define CMD_WRITE_CONFIG        5
#define CMD_READ_CONFIG         6
#define CMD_DISABLE_OUTPUT      7
#define CMD_12BIT_GRAYSCALE     192
#define CMD_16BIT_GRAYSCALE     256

typedef struct tagDispStatus
{
	LEDDATA_T ledData;

	uint16_t bright;
	uint8_t	isReflash;
	uint8_t isConfig;
    uint8_t current;
} DispStatus;

static DispStatus s;

static const uint8_t BITMASK_ICR[2] =
{ 0x01, 0x02 };


static void DisplayPlainMapModule(const LEDDATA_T* const data)
{
    register uint8_t color, bit;
    uint16_t offsetData;
    uint8_t bitmask;

    uint8_t value = pdFALSE;

    if (data->red)
    	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, s.bright);
	else
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);

    if (data->grn)
    	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, s.bright);
	else
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);

    if (data->blu)
    	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, s.bright);
	else
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);

    if (data->ir)
    	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, s.bright);
	else
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
}

void dispInit()
{
	printf("Disp Init.....\r\n");

    memset(&s, 0x00, sizeof(DispStatus));

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);

    s.ledData.red = 0xFF;
    s.current = 0x7F;
    s.bright = 0xFFFF;
    s.isConfig = pdTRUE;
    s.isReflash = pdTRUE;

    dispRefresh();
}

void dispSetData(LEDDATA_T* data)
{
    if (data == NULL)
    {
        memset(&s.ledData, 0x00, sizeof(LEDDATA_T));
    }
    else
    {
        memcpy(&s.ledData, data, sizeof(LEDDATA_T));
    }

    s.isReflash = pdTRUE;
}


void dispRefresh()
{
	if(s.isReflash)
	{
		s.isReflash = pdFALSE;

		DisplayPlainMapModule(&s.ledData);
	}
}

void dispSetBright(uint32_t brt)
{
	s.bright = 255-brt;
}
