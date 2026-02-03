/*
 * peripherals.c
 *
 *  Created on: 2018. 7. 6.
 *      Author: S.S.C
 */
#include <string.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "peripherals.h"
#include "main.h"

#define STATE_LAMP_ACTIVE       1
#if (STATE_LAMP_ACTIVE == 0)
#define STATE_LAMP_INACTIVE     1
#else
#define STATE_LAMP_INACTIVE     0
#endif

#define STATE_SWITCH_ACTIVE     0
#if (STATE_SWITCH_ACTIVE == 0)
#define STATE_SWITCH_INACTIVE   1
#else
#define STATE_SWITCH_INACTIVE   0
#endif

#define STATE_OUTPUT_ACTIVE     1
#if (STATE_OUTPUT_ACTIVE == 0)
#define STATE_OUTPUT_INACTIVE   1
#else
#define STATE_OUTPUT_INACTIVE   0
#endif

#define STATE_INPUT_ACTIVE     1
#if (STATE_INPUT_ACTIVE == 0)
#define STATE_INPUT_INACTIVE   1
#else
#define STATE_INPUT_INACTIVE   0
#endif



typedef struct {
    uint8_t lampStatus;
} PeriStatus_t;

static PeriStatus_t s;

void periInit()
{
    memset(&s, 0x00, sizeof(PeriStatus_t));


    for (uint8_t i=0; i<10; i++)
    {
		SetLamp1(STATE_LAMP_ACTIVE);
		SetLamp2(STATE_LAMP_ACTIVE);
		SetLamp3(STATE_LAMP_ACTIVE);
		SetLamp4(STATE_LAMP_ACTIVE);
		SetLamp5(STATE_LAMP_ACTIVE);
		SetLamp6(STATE_LAMP_ACTIVE);
		SetLamp7(STATE_LAMP_ACTIVE);
		SetLamp8(STATE_LAMP_ACTIVE);
		HAL_Delay(25);

		SetLamp1(STATE_LAMP_INACTIVE);
		SetLamp2(STATE_LAMP_INACTIVE);
		SetLamp3(STATE_LAMP_INACTIVE);
		SetLamp4(STATE_LAMP_INACTIVE);
		SetLamp5(STATE_LAMP_INACTIVE);
		SetLamp6(STATE_LAMP_INACTIVE);
		SetLamp7(STATE_LAMP_INACTIVE);
		SetLamp8(STATE_LAMP_INACTIVE);
		HAL_Delay(25);
	}

}

void periSetLamp(uint8_t lamp)
{
    if (lamp != s.lampStatus)
    {
        s.lampStatus = lamp;
        SetLamp1(lamp & PERI_LAMP1 ? STATE_LAMP_ACTIVE : STATE_LAMP_INACTIVE);
        SetLamp2(lamp & PERI_LAMP2 ? STATE_LAMP_ACTIVE : STATE_LAMP_INACTIVE);
        SetLamp3(lamp & PERI_LAMP3 ? STATE_LAMP_ACTIVE : STATE_LAMP_INACTIVE);
        SetLamp4(lamp & PERI_LAMP4 ? STATE_LAMP_ACTIVE : STATE_LAMP_INACTIVE);
        SetLamp5(lamp & PERI_LAMP5 ? STATE_LAMP_ACTIVE : STATE_LAMP_INACTIVE);
        //SetLamp6(lamp & PERI_LAMP6 ? STATE_LAMP_ACTIVE : STATE_LAMP_INACTIVE);
        //SetLamp7(lamp & PERI_LAMP7 ? STATE_LAMP_ACTIVE : STATE_LAMP_INACTIVE);
        //SetLamp8(lamp & PERI_LAMP8 ? STATE_LAMP_ACTIVE : STATE_LAMP_INACTIVE);
    }
}

uint8_t periGetLamp()
{
    return s.lampStatus;
}

uint8_t periGetSwitch()
{

    uint8_t ret = 0;

	if (GetSwitch1() == STATE_SWITCH_ACTIVE)
		ret |= PERI_SWITCH1;

	if (GetSwitch2() == STATE_SWITCH_ACTIVE)
		ret |= PERI_SWITCH2;

	if (GetSwitch3() == STATE_SWITCH_ACTIVE)
		ret |= PERI_SWITCH3;

	if (GetSwitch4() == STATE_SWITCH_ACTIVE)
		ret |= PERI_SWITCH4;

	if (GetSwitch5() == STATE_SWITCH_ACTIVE)
		ret |= PERI_SWITCH5;

	if (GetSwitch6() == STATE_SWITCH_ACTIVE)
		ret |= PERI_SWITCH6;

	if (GetSwitch7() == STATE_SWITCH_ACTIVE)
		ret |= PERI_SWITCH7;

	if (GetSwitch8() == STATE_SWITCH_ACTIVE)
		ret |= PERI_SWITCH8;

    return ret;
}

uint8_t periGetInput()
{

    uint8_t ret = 0;

	if (GetInput1() == STATE_SWITCH_ACTIVE)
		ret |= PERI_INPUT1;

	if (GetInput2() == STATE_SWITCH_ACTIVE)
		ret |= PERI_INPUT2;

	if (GetInput3() == STATE_SWITCH_ACTIVE)
		ret |= PERI_INPUT3;

	if (GetInput4() == STATE_SWITCH_ACTIVE)
		ret |= PERI_INPUT4;
	if (GetDRDYInput() == STATE_SWITCH_ACTIVE)
		ret |= PERI_DRDY;

    return ret;
}


