/*
 * peripherals.c
 *
 *  Created on: 2018. 7. 6.
 *      Author: S.S.C
 */
#include <string.h>
#include "cmsis_os.h"
#include "peripherals.h"
#include "main.h"

#define STATE_LAMP_ACTIVE       0
#if (STATE_LAMP_ACTIVE == 0)
#define STATE_LAMP_INACTIVE     1
#else
#define STATE_LAMP_INACTIVE     0
#endif

#define STATE_OUTPUT_ACTIVE     1
#if (STATE_OUTPUT_ACTIVE == 0)
#define STATE_OUTPUT_INACTIVE   1
#else
#define STATE_OUTPUT_INACTIVE   0
#endif

#define STATE_RF_ACTIVE     0
#if (STATE_RF_ACTIVE == 0)
#define STATE_RF_INACTIVE   1
#else
#define STATE_RF_INACTIVE   0
#endif

typedef struct {
    uint8_t lampStatus;
    uint8_t outputStatus;
    uint8_t rfStatus;
} PeriStatus_t;

static PeriStatus_t s;

void periInit()
{
	printf("Peri Init.....\r\n");

    memset(&s, 0x00, sizeof(PeriStatus_t));
    s.rfStatus = 0xFF;
    for (uint8_t i=0; i<10; i++)
    {
    	SetLamp(STATE_LAMP_ACTIVE);
		HAL_Delay(25);

		SetLamp(STATE_LAMP_INACTIVE);
		HAL_Delay(25);
	}
}

void periSetLamp(uint8_t lamp)
{
    if (lamp != s.lampStatus)
    {
        s.lampStatus = lamp;
        SetLamp(lamp ? STATE_LAMP_ACTIVE : STATE_LAMP_INACTIVE);
    }
}

void periRFModule(uint8_t output)
{
	printf("periRFModule [%d].....\r\n", output);
    if (output != s.rfStatus)
    {
        s.rfStatus = output;
        SetCH1(output & PERI_OUTPUT1 ? STATE_RF_ACTIVE : STATE_RF_INACTIVE);
        SetCH2(output & PERI_OUTPUT2 ? STATE_RF_ACTIVE : STATE_RF_INACTIVE);
        SetBAND(output & PERI_OUTPUT3 ? STATE_RF_ACTIVE : STATE_RF_INACTIVE);
        SetOPT(output & PERI_OUTPUT4 ? STATE_RF_ACTIVE : STATE_RF_INACTIVE);
    }
}

void periRFWakeup(uint8_t mode)
{
	printf("periRFWakeup [%d].....\r\n", mode);
	SetWAKEUP(mode == pdTRUE ? STATE_OUTPUT_ACTIVE : STATE_OUTPUT_INACTIVE);
	//1 : high, 0:low
}

void periRFSet(uint8_t mode)
{
	//printf("dispPowerSet [%d].....\r\n", mode);
	SetRFRst(mode == pdTRUE ? STATE_OUTPUT_ACTIVE : STATE_OUTPUT_INACTIVE);
}


void periRFReset()
{
	SetRFRst(STATE_OUTPUT_INACTIVE);
	HAL_Delay(25);
	SetRFRst(STATE_OUTPUT_ACTIVE);
}

void periBLEWakeup(uint8_t mode)
{
	printf("periBLEWakeup [%d].....\r\n", mode);
	SetBLEWAKEUP(mode == pdTRUE ? STATE_OUTPUT_ACTIVE : STATE_OUTPUT_INACTIVE);
	//0 : high, 1:low
}

void periBLEUART(uint8_t mode)
{
	printf("periBLEUART [%d].....\r\n", mode);
	SetBLEUART(mode == pdTRUE ? STATE_OUTPUT_ACTIVE : STATE_OUTPUT_INACTIVE);
	//0 : high, 1:low
}

void periSetBLE(uint8_t mode)
{
	SetBLE_GPI(mode);
}

uint8_t periGetBLE()
{
    uint8_t ret = 0;
    if(GetBLE_GPO())
    	ret = pdTRUE;
    else
    	ret = pdFALSE;

	return ret;
}
