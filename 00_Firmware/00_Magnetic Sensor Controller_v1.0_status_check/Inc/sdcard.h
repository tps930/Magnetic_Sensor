/*
 * sdcard.h
 *
 *  Created on: 2018. 8. 2.
 *      Author: S.S.C
 */

#ifndef SDCARD_H_
#define SDCARD_H_

#include <string.h>
#include "sdcard.h"
#include "fatfs.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "uart.h"
#include "logic.h"
#include "ff.h"


#define FILE_COUNT 10
#define FILE_NAME_LENGTH 256

#define STATE_SD_DET_ACTIVE     0
#if (STATE_SD_DET_ACTIVE == 0)
#define STATE_SD_DET_INACTIVE   1
#else
#define STATE_SD_DET_INACTIVE   0
#endif


#define GET_SD_DET()        	HAL_GPIO_ReadPin(SDIO_DETECT_GPIO_Port, SDIO_DETECT_Pin)

#define GET_SD_POWER()        	HAL_GPIO_ReadPin(SDIO_POWER_GPIO_Port, SDIO_POWER_Pin)
#define SET_SD_POWER(state)     HAL_GPIO_WritePin(SDIO_POWER_GPIO_Port, SDIO_POWER_Pin, state)

typedef struct
{
	uint8_t sd_det;
	uint8_t sd_power;

	uint8_t status;

	FRESULT mount;
	FRESULT getfree;
	uint8_t isInit;

	int32_t fre_clust;
	int32_t total_space;
	int32_t available_space;
} SDCardStatus_T;

extern LogicStatus_T ls;

SDCardStatus_T sds;

#define MAX_EVENT 		64

typedef struct {
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	uint8_t id;
	uint8_t opcode;
	uint8_t subcode;

	char msg[512];
}	LOG_T;


typedef struct {
	uint8_t wbp;
	uint8_t rbp;

	LOG_T	buff[MAX_EVENT];

}	LOG_EVENT_T;


void loginit(void);
void logicLogTask();
void report_append_str(RTC_DateTypeDef sDate, RTC_TimeTypeDef sTime, char *str);
void SensorLogSave(void);

void sdcardInit();
void logicSDCardTask();

#endif /* SDCARD_H_ */
