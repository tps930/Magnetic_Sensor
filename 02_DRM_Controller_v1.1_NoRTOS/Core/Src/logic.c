/*
 * logic.c
 *
 *  Created on: 2019. 9. 3.
 *      Author: S.S.C
 */

#define TITLE  	"LEDO Co., Ltd "
#define VERSION  "HW:DRM Ver2.1 SW : v2.1.0 "

char swTITLE[32] = TITLE;
char swVER[32] = VERSION;

#include <string.h>
#include <stdio.h>
#include "stm32l0xx_hal.h"
#include "cmsis_os.h"
#include "logic.h"
#include "uart.h"

extern IWDG_HandleTypeDef hiwdg;

extern ADC_HandleTypeDef hadc;
extern RTC_HandleTypeDef hrtc;

extern I2C_HandleTypeDef hi2c1;

extern osMessageQId uartQueueHandle;
extern osMessageQId bleQueueHandle;

extern UART_HandleTypeDef huart1;		//Debug Download
extern UART_HandleTypeDef huart4;		//RF Modem
extern UART_HandleTypeDef huart5;		//BLE

static const uint32_t Baud[] =
{
    9600, 38400, 115200
};

const unsigned int temp_table[] = {
		3867, 3854, 3841, 3827, 3812, 3796, 3780, 3763, 3745, 3727,	//-30 ~ -21�꼦
		3708, 3688, 3667, 3645, 3623, 3600, 3575, 3550, 3524, 3497,	//-20 ~ -11�꼦
		3470, 3441, 3411, 3381, 3349, 3317, 3284, 3250, 3215, 3179,	//-10 ~ -1�꼦
		3142, 3105, 3066, 3027, 2987, 3947, 3906, 3864, 3821, 2778,	//0 ~ 9�꼦
		3735, 2690, 2646, 2601, 2556, 2510, 2464, 2418, 2372, 2325,	//10 ~ 19�꼦
		2279, 2232, 2186, 2140, 2094, 2048, 2002, 1956, 1911, 1866,	//20 ~ 29�꼦
		1822, 1778, 1734, 1691, 1649, 1607, 1565, 1525, 1484, 1445,	//30 ~ 39�꼦
		1406, 1368, 1331, 1294, 1258, 1223, 1188, 1155, 1122, 1089,	//40 ~ 49�꼦
		1058, 1027, 997, 968, 940, 912, 885, 858, 833, 808,	//50 ~ 59�꼦
		783, 860, 737, 715, 693, 672, 652, 632, 613, 594,	//60 ~ 69�꼦
		576, 558, 541, 525, 509, 493, 478, 464, 450, 436,	//70 ~ 79�꼦
		423, 410, 397, 385, 374, 363, 352, 341, 331, 321,			//80 ~ 89�꼦
		311, 302, 293, 284, 276, 268, 260, 252, 245, 238 };			//90 ~ 99�꼦
/*
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOC)) {
		ls.cds.Stat = pdTRUE;
		printf("[LOGIC] HAL_ADC_ConvCpltCallback\r\n");
	}
}*/

uint32_t CalcTickDiff(uint32_t oldTicks, uint32_t newTicks)
{
	uint32_t ret;
    if (oldTicks <= newTicks)
    {
        ret = newTicks - oldTicks;
    }
    else
    {
        ret = 0xFFFFFFFF - newTicks + oldTicks;
    }

    return ret;
}

void SleepMode(void)
{
	//printf("SleepMode[%d]\r\n", s.night_count);
	/* Suspend SysTick */
	printf("SleepMode\r\n");
	periRFWakeup(pdFALSE);
	periRFSet(pdFALSE);
	HAL_SuspendTick();

	//HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0x1E000, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

	 if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0xFFFF, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
	  {
	    Error_Handler();
	  }

	/* Enable Power Peripheral */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* Sleep Mode */
	HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);

	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	/* Resume SysTick When System Wake-up */
	HAL_ResumeTick();
}

void StopMode(void)
{
	//printf("StopMode[%d]\r\n", s.night_count);
	/* Suspend SysTick */
	printf("StopMode\r\n");
	periRFWakeup(pdFALSE);
	periRFSet(pdFALSE);
	HAL_SuspendTick();

	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0xFFFF, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

	/* Enable Power Peripheral */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* STOP Mode */
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

	/* PLL Clock Recovery */
	SystemClock_Config();

	/* Resume SysTick When System Wake-up */
	HAL_ResumeTick();

}


void StandByMode(void)
{
	/* Suspend SysTick */
	//HAL_SuspendTick();
	//printf("StandByMode[%d]\r\n", s.night_count);
	//HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0xFFFF, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

	printf("StandByMode\r\n");

	HAL_RTCEx_BKUPWrite(&hrtc, 1, ls.stat.RFWakeupCount);
	periRFWakeup(pdFALSE);
	periRFSet(pdFALSE);
	/* Enable Power Peripheral */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* STOP Mode */
	HAL_PWR_EnterSTANDBYMode();

	//HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

	/* PLL Clock Recovery */
	//SystemClock_Config();

	/* Resume SysTick When System Wake-up */
	//HAL_ResumeTick();

}

void logicInit()
{
	printf("======================================\r\n");
	printf("  Smart DRM Controller v1.0\r\n");
	printf("  LEDO Co,. Ltd. \r\n");
	printf("======================================\r\n");

	memset(&ls, 0x00, sizeof(LogicStatus_t));

	//periRFWakeup(pdFALSE);

	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB)) {
		ls.stat.RFWakeupCount = HAL_RTCEx_BKUPRead(&hrtc, 1);
		printf("StandbyMode Check[%d]\r\n", ls.stat.RFWakeupCount);

		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU | PWR_FLAG_SB);
	}

	periInit();
	dispInit();


    /*dispSetBright(TEST_BRIGHT_W3);
	ls.tdis.Disp_Led.red = 0xFF;
	ls.tdis.Disp_Led.grn = 0xFF;
	ls.tdis.Disp_Led.blu = 0xFF;
    dispSetData(&ls.tdis.Disp_Led);
    dispRefresh();
    HAL_Delay(25);

    ls.tdis.Disp_Led.red = 0x00;
    ls.tdis.Disp_Led.grn = 0x00;
    ls.tdis.Disp_Led.blu = 0x00;
    ls.tdis.Delay = TEST_DELAY_SHORT;
    dispSetBright(0);
    dispSetData(&ls.tdis.Disp_Led);
    dispRefresh();
    HAL_Delay(25);*/

	Read_Params();
	ls.ble.isInit = pdFALSE;
	ls.ble.isBaudInit = ls.modem.isBLEBaudInit;

	ls.tick.RF = ls.tick.Tick;
}

void logicRFModemInit()
{
	uint8_t rfset = 0x00;

	rfset = 0x00;
	rfset |= 0x04;
	rfset |= 0x03;

	periRFModule(rfset);
	periRFWakeup(pdTRUE);

	/*if(ls.stat.RFWakeupCount > 10)
	{
		ls.stat.RFWakeupCount = 0;
		periRFWakeup(pdTRUE);
	}
	else
	{
		ls.stat.RFWakeupCount++;
		periRFWakeup(pdFALSE);
	}*/

	periRFReset();
	ls.tick.RF = ls.tick.Tick;
}


void logicPeriTask()
{
	if(ls.stat.isCommInit == pdFALSE)
	{
		ls.stat.isCommInit = pdTRUE;
		commInit();
		ls.stat.isRegister = pdTRUE;
	}

	if (CalcTickDiff(ls.tick.Indi, ls.tick.Tick) <= 200) {
		return;
	}

	if (ls.stat.isRegister == pdTRUE) {
		ls.stat.isRegister = pdFALSE;
		logicRFModemInit();
	}

	ls.tick.Indi = ls.tick.Tick;

	if (ls.indi.Program) {
		if(ls.indi.OnCount > 1)
		{
			ls.indi.Program = pdFALSE;
			ls.indi.OnCount = 0;
		}
	} else {
		if(ls.indi.OnCount > 16)
		{
			ls.indi.Program = pdTRUE;
			ls.indi.OnCount = 0;
		}
	}

	ls.indi.OnCount++;
	periSetLamp(ls.indi.Program);

/*	if (CalcTickDiff(ls.tick.Cds, ls.tick.Tick) <= 60000) {
		return;
	}

	if (HAL_ADC_Init(&hadc) != HAL_OK) {
		printf("[LOGIC] ADC Init Error\r\n");
	}

	ls.tick.Cds = ls.tick.Tick;

	HAL_ADC_Start_DMA(&hadc, (uint32_t*)ls.cds.Value, 4);

	while(ls.cds.Stat == pdFALSE){
		if (__HAL_ADC_GET_FLAG(&hadc, ADC_FLAG_EOC)) {
			ls.cds.Stat = pdTRUE;
		}

		if (CalcTickDiff(ls.tick.Cds, ls.tick.Tick) >= 300) {
			break;
		}
	}

	if(ls.cds.Stat != pdTRUE)
		return;

	for(int i = 0; i < 10; i++)
	{
		ls.cds.Count = i;
		ls.cds.Cds_Array[0][ls.cds.Count] = ls.cds.Value[0];
		ls.cds.Cds_Array[1][ls.cds.Count] = ls.cds.Value[1];
		ls.cds.Cds_Array[2][ls.cds.Count] = ls.cds.Value[2];
		ls.cds.Cds_Array[3][ls.cds.Count] = ls.cds.Value[3];
		HAL_Delay(10);
	}
	HAL_ADC_Stop_DMA(&hadc);

	if (HAL_ADC_DeInit(&hadc) != HAL_OK) {
		printf("[LOGIC] ADC DeInit Error\r\n");
	}

	ls.cds.Cds_Sum[0] = 0;
	ls.cds.Cds_Sum[1] = 0;
	ls.cds.Cds_Sum[2] = 0;
	//ls.cds.Cds_Sum[3] = 0;

	for(int i = 0; i < 10; i++)
	{
		ls.cds.Cds_Sum[0] += ls.cds.Cds_Array[0][i];
		ls.cds.Cds_Sum[1] += ls.cds.Cds_Array[1][i];
		ls.cds.Cds_Sum[2] += ls.cds.Cds_Array[2][i];
		ls.cds.Cds_Sum[3] += ls.cds.Cds_Array[3][i];
	}

	uint32_t v = 0;
	for(int i = 0; i < 4; i++)
	{
		if(ls.cds.Cds_Sum[i] > 0)
			ls.cds.Cds_Avg[i] = ls.cds.Cds_Sum[i] / 10;
		else
			ls.cds.Cds_Avg[i] = 0;
	}

	printf("[LOGIC] ADC Value [CH1 : %d][CH2 : %d][CH3 : %d][CH4 : %d]\r\n", ls.cds.Cds_Avg[0], ls.cds.Cds_Avg[1], ls.cds.Cds_Avg[2], ls.cds.Cds_Avg[3]);

	double dv = ls.cds.Cds_Avg[2] * 1.0;
	double fVoltage = 0.0;
	fVoltage = ((3.3 * dv)/4096.0) * ((1.0+1.0)/1.0);

	ls.cds.fBattery = fVoltage;
	ls.cds.Battery = ls.cds.fBattery * 100;

	dv = ls.cds.Cds_Avg[3] * 1.0;
	fVoltage = ((3.3 * dv)/4096.0) * ((1.0+1.0)/1.0);
	ls.cds.fSolar = fVoltage;
	ls.cds.Solar = ls.cds.fSolar * 100;

	CalcTemp();
	printf("[Logic] Battery Voltage [BAT : %d v][SOL : %d v][TEMP : %d ℃]\r\n", ls.cds.Battery, ls.cds.Solar, ls.cds.Temp);*/
}

void CalcTemp()
{
	uint8_t tmp = 0;
	uint16_t decimal_max;
	uint16_t decimal_min;
	uint16_t decimal_gap;
	uint16_t decimal_value;

	if(ls.cds.Cds_Avg[1] > TEMP_MINUS_30)
	{
		ls.cds.Temp = -300;		//temp * 10(-30.0)
	}
	else if(ls.cds.Cds_Avg[1] < TEMP_99)
	{
		ls.cds.Temp = 990;		//temp * 10(99.0 )
	}
	else
	{
		for (int i = 1; i < 130; i++) {
			if (ls.cds.Cds_Avg[1] >= temp_table[i]) {
				ls.cds.Temp = (i - 30) * 10;
				decimal_max = temp_table[i - 1];
				decimal_min = temp_table[i];
				decimal_gap = decimal_max - decimal_min;
				decimal_value = (((decimal_max * 10) - (ls.cds.Cds_Avg[1] * 10)) / decimal_gap);

				ls.cds.Temp -= decimal_value;
				break;
			}
			tmp++;
		}
	}
}
void logicIdleTask()
{
	ls.tick.Tick = HAL_GetTick();
    HAL_IWDG_Refresh(&hiwdg);
}

void logicProcTask()
{
	if(ls.stat.isTestMode)
		DisplayTestMode();
	else
		DisplayWorkingMode();
}

char command_str[128];
void logicUartTask()
{
	if (CalcTickDiff(ls.tick.RF, ls.tick.Tick) > 2000 && ls.stat.isStandByMode == pdFALSE
			&& ls.ble.isConnected == pdFALSE) {
		//StandByMode();
	}

	if (CalcTickDiff(ls.tick.RF, ls.tick.Tick) > (180 * 1000) && ls.stat.isStandByMode
			&& ls.ble.isConnected == pdFALSE) {
		//StandByMode();
	}

	Modem_Receive_Check();

	uint8_t device_id = 0;

	if(ls.modem.recvFlag == pdTRUE)
	{
		ls.tick.RF = ls.tick.Tick;

		ls.modem.recvFlag = pdFALSE;
		switch (ls.modem.recvData.opcode)		//Opcode
		{
		   	case RF_CMD_SEND:
	    	case RF_CMD_RECV:			//Recv Data Packet
	    		ls.modem.recvData.section_id = ls.modem.recvData.data[1];
	    		ls.modem.recvData.device_id = ls.modem.recvData.data[2];
	    		ls.modem.recvData.subcode = ls.modem.recvData.data[3];
	    		Modem_Packet_Parse(ls.modem.recvData.subcode, ls.modem.recvData.section_id, ls.modem.recvData.device_id, &ls.modem.recvData.data[4]);
	    		break;

	    	default:
	    		break;
		}
	}

	if (ls.stat.isRegisterCall && ls.ble.isInit == pdTRUE) {

		if (CalcTickDiff(ls.tick.Send, ls.tick.Tick) > (ls.modem.DeviceID != 0x00 ? ls.modem.DeviceID * 100 : ls.ble.mac_address[5] * 100)) {
			ls.stat.isRegisterCall = pdFALSE;

			Send_Register();
		}
	}

	if (ls.stat.isStatusCall) {
		if (CalcTickDiff(ls.tick.Send, ls.tick.Tick) > 1000) {
			ls.stat.isStatusCall = pdFALSE;

			Send_Status();
		}
	}
}

void logicBLETask()
{
	if (CalcTickDiff(ls.tick.Ble, ls.tick.Tick) <= 100) {
		return;
	}

	ls.tick.Ble = ls.tick.Tick;

	//ls.ble.isBaudInit = pdTRUE;

	periSetBLESleep(pdFALSE);

		uint8_t ble = 0x00;

		ble = periGetBLE();

		if (ls.ble.isConnected != ble) {
			ls.ble.isConnected = ble;
			if (ls.ble.isConnected) {
				printf("BLE [ CONNECTED    ]\r\n");
				ls.ble.isMode = pdTRUE;
				periSetBLE(GPIO_PIN_RESET);		//Bypass Mode
			} else {
				printf("BLE [ DISCONNECTED ]\r\n");
				ls.ble.isMode = pdFALSE;
				periSetBLE(GPIO_PIN_SET);		//Command Mode
			}
		}

	if (ls.ble.isInit == pdFALSE && ls.ble.isConnected == pdFALSE) {
		periSetBLE(GPIO_PIN_SET);		//Command Mode
		ls.ble.isMode = pdFALSE;

		if(ls.ble.isBaudInit == pdFALSE)
		{
			if(ls.ble.isStatus == pdTRUE)
			{
				if(ls.ble.BaudCheckStep > 3)
				{
					if(ls.ble.BaudSetCall == pdFALSE)
					{
						ls.ble.BaudSetCall = pdTRUE;
						ls.ble.isStatus = pdFALSE;
						Send_BLE_UART_SET(Baud[0]);
						UART_Baud(&huart5, Baud[0]);
					}
					else
					{
						printf("BLE Baud Check OK....\r\n");
						ls.ble.isStatus = pdFALSE;
						ls.ble.isBaudInit = pdTRUE;
						ls.modem.isBLEBaudInit = pdTRUE;
						Save_Params(&ls.modem, sizeof(MODEM_T));
					}
				}
				else
				{
					printf("BLE Baud Check OK....\r\n");
					ls.ble.isStatus = pdFALSE;
					ls.ble.isBaudInit = pdTRUE;
					ls.modem.isBLEBaudInit = pdTRUE;
				}
			}
			else{
				if(ls.ble.BaudSetCall != pdTRUE)
				{
					switch(ls.ble.BaudCheckStep)
					{
						case 0:
							Send_BLE_check();
							ls.ble.BaudCheckStep++;
							break;
						case 1:
							UART_Baud(&huart5, Baud[0]);
							ls.ble.BaudCheckStep++;
							break;
						case 2:
						case 3:
							ls.ble.isStatus = pdFALSE;
							Send_BLE_check();
							ls.ble.BaudCheckStep++;
							break;
						case 4:
							Send_BLE_check();
							ls.ble.BaudCheckStep++;
							break;
						case 5:
							UART_Baud(&huart5, Baud[1]);
							ls.ble.BaudCheckStep++;
							break;
						case 6:
						case 7:
							ls.ble.isStatus = pdFALSE;
							Send_BLE_check();
							ls.ble.BaudCheckStep++;
							break;
						case 8:
							Send_BLE_check();
							ls.ble.BaudCheckStep++;
							break;
						case 9:
							UART_Baud(&huart5, Baud[2]);
							ls.ble.BaudCheckStep++;
							break;
						case 10:
						case 11:
							ls.ble.isStatus = pdFALSE;
							Send_BLE_check();
							ls.ble.BaudCheckStep++;
							break;
						case 12:
							//ls.ble.BaudCheckStep = 0;
							break;
					}
				}
				else
				{
					Send_BLE_check();
				}
			}
		}
		else  //Baud Init Next Step
		{
			if(ls.ble.isStatus == pdTRUE)
			{
				switch (ls.ble.InitStep) {
				case 0:
					ls.ble.isStatus = pdFALSE;
					Send_BLE_Info_REQ();
					ls.ble.InitStep++;
					break;
				case 1:
					ls.ble.InitStep++;
					break;
				case 2:
					ls.ble.isStatus = pdFALSE;
					Send_BLE_MANUF_SET();
					ls.ble.InitStep++;
					break;
				case 3:
					ls.ble.InitStep++;
					break;
				case 4:
					ls.ble.InitStep = 0;
					ls.ble.isInit = pdTRUE;

					printf("BLE Init OK....\r\n");

					if (ls.ble.isConnected) {
						ls.ble.isMode = pdTRUE;
						periSetBLE(GPIO_PIN_RESET);		//Bypass Mode
					}
					break;

				}
			}
			else
			{
					Send_BLE_check();
			}
		}
	}

	BLE_Receive_Check(ls.ble.isMode);

	if (ls.ble.recvFlag == pdTRUE)
	{
		ls.ble.recvFlag = pdFALSE;

		printf("\r\n[Logic] Firmware OP_CODE [%2X]\r\n", ls.ble.recvData.opcode);

		switch (ls.ble.recvData.opcode)		//Opcode
		{
		case FIRMWARE_CMD_REG_REQ:		//모뎀정보 요청
			Send_Modem_Status();
			break;

		case FIRMWARE_CMD_REG_SET:		//C-Resigter 설정
			Save_Params(&ls.ble.recvData.data[0], 9);
			ls.stat.isRegister = pdTRUE;
			break;

		case FIRMWARE_BLE_NAME_REQ:
			Send_BLE_MANUF_Status_REQ();
			break;

		case FIRMWARE_BLE_NAME_SET:
			memcpy(&ls.ble.name, &ls.ble.recvData.data[0], 8);
			printf("[LOGIC] ============= BLE SETTING ============\r\n");
			printf("[LOGIC]  BLE NAME        : [ %s ]\r\n", ls.ble.name);
			printf("[LOGIC] =======================================\r\n");
			Send_BLE_MANUF_SET(ls.ble.name);
			break;

		case FIRMWARE_BLE_ADVDATA_REQ:
			Send_BLE_ADVDATA_Status_REQ();
			break;

		case FIRMWARE_BLE_ADVDATA_SET:
			memcpy(&ls.ble.data, &ls.ble.recvData.data[0], 13);
			printf("[LOGIC] ============= BLE SETTING ============\r\n");
			printf("[LOGIC] BLE DATA        : [ %s ]\r\n", ls.ble.data);
			printf("[LOGIC] =======================================\r\n");
			Send_BLE_ADVDATA_SET();
			break;

		default:
			break;
		}
	}
}

void Modem_Packet_Parse(uint8_t opcode, uint8_t section_id, uint8_t device_id, uint8_t* buff)
{
	printf("[LOGIC] Modem Packet_Parse[%02x][%02x][%02x][%02x]\r\n", opcode, section_id, device_id, buff[0]);
	uint8_t temp_id = 0;

	if (section_id != 0xFF) {
		if (section_id != ls.modem.SectionID) {
			return;
		}
	}
	if (device_id != 0xFF) {
		if (device_id != ls.modem.DeviceID) {
			return;
		}
	}

	switch (opcode)		//Opcode
	{
		case DRM_BLE_SLEEP:
			ls.stat.isStandByMode = pdFALSE;
			HAL_RTCEx_BKUPWrite(&hrtc, 0, 0);
			break;
		case DRM_BLE_STANDBY:
			ls.stat.isStandByMode = pdTRUE;
			HAL_RTCEx_BKUPWrite(&hrtc, 0, 0);
			break;

		case DRM_CMD_REG_REQ:			//DRM Register Requent
			ls.tick.Send = ls.tick.Tick;
			ls.stat.isRegisterCall = pdTRUE;
			printf("[LOGIC] RegisterCall\r\n");
			break;

		case DRM_CMD_GRP_CTL:			//Group Control
			Set_DRM(&buff[0]);
			ls.wdis.Brt = buff[0];
			ls.wdis.Disp_Led.red = (buff[1] & 0xC0) >> 6;
			ls.wdis.Disp_Led.grn = (buff[1] & 0x30) >> 4;
			ls.wdis.Disp_Led.blu = (buff[1] & 0x0C) >> 2;
			ls.wdis.isIR =  (buff[1] & 0x02) >> 1;

			ls.wdis.isIRFlash = buff[2] & 0x80 ? pdTRUE : pdFALSE;
			ls.wdis.isFlash = buff[2] & 0x40 ? pdTRUE : pdFALSE;
			ls.wdis.FlashMode = buff[2] & 0x20 ? pdTRUE : pdFALSE;
			ls.wdis.Flashing_InterVal = buff[2] & 0x1F;

			ls.tick.Send  = ls.tick.Tick;

			ls.wdis.Pos = 1;
			ls.wdis.FlashCount = 0;
			ls.stat.isGroupControl = pdTRUE;
			//ls.stat.isBLEADVCall = pdTRUE;

			ls.tick.Send = ls.tick.Tick;
			ls.stat.isStatusCall = pdTRUE;

			printf("[LOGIC] DRM Group Control[BRT : %02x][RGB : %02x][Opt : %02x]\r\n",
					buff[0], buff[1], buff[2]);

			break;

		case DRM_CMD_ONE_CTL:			//One Control
			Set_DRM(&buff[0]);

			ls.wdis.Brt = buff[0];
			ls.wdis.Disp_Led.red = (buff[1] & 0xC0) >> 6;
			ls.wdis.Disp_Led.grn = (buff[1] & 0x30) >> 4;
			ls.wdis.Disp_Led.blu = (buff[1] & 0x0C) >> 2;
			ls.wdis.isIR = (buff[1] & 0x02) >> 1;
			ls.wdis.isBLEOnOff = (buff[1] & 0x01);

			ls.wdis.isIRFlash = buff[2] & 0x80 ? pdTRUE : pdFALSE;
			ls.wdis.isFlash = buff[2] & 0x40 ? pdTRUE : pdFALSE;
			ls.wdis.FlashMode = buff[2] & 0x20 ? pdTRUE : pdFALSE;
			ls.wdis.Flashing_InterVal = buff[2] & 0x1F;

			ls.wdis.Pos = 1;
			ls.wdis.FlashCount = 0;

			ls.stat.isGroupControl = pdFALSE;
			printf("[LOGIC ] OneControl [BRT : %02x][RGB : %02x][Opt : %02x]\r\n", buff[0], buff[1], buff[2]);

			//Send_Status();

			break;

		case DRM_CMD_ALL_CTL_V3:			//All Control(Max 8EA) - 2021 Year
			for (int i = 0; i < 7; i++) {
				temp_id = buff[(i * 4)];
				if (temp_id == ls.modem.DeviceID) {

					Set_DRM(&buff[(i * 4) + 1]);

					ls.stat.isGroupControl = pdFALSE;
					//ls.stat.isBLEADVCall = pdTRUE;
					printf("[LOGIC ] AllControl_V3 [%d][BRT:%02x][RGB:%02x][O:%02x]\r\n",
							temp_id, buff[(i * 4) + 1], buff[(i * 4) + 2], buff[(i * 4) + 3]);
					break;
				}
			}
			break;

		case DRM_CMD_EVENT_SET_V2:
					ls.stat.EventPosition = (buff[0] << 8) | buff[1];
					if(ls.stat.initPosition < ls.stat.EventPosition)
					{
						printf("[LOGIC ] This Event be not included - [E : %d m][P : %4d m]\r\n", ls.stat.EventPosition, ls.stat.initPosition);

						/*Send_BLE_check();
						Send_BLE_ADVDATA_Situation(ls.modem.SectionID, 0, 0);
						Send_BLE_check();*/

						ls.stat.isEventControl = pdFALSE;
						ls.wdis.isBLEOnOff = pdFALSE;
						ls.stat.isBLEADVCall = pdTRUE;
					}
					else
					{
						ls.stat.NewEventDistanceGap = ls.stat.initPosition - ls.stat.EventPosition;
						for(int i = 0; i < buff[2]; i++)
						{
							uint16_t offset = (i*8);
							int16_t start = (buff[4 + offset] << 8) | buff[5 + offset];
							int16_t end =  (buff[6 + offset] << 8) | buff[7 + offset];

							printf("[LOGIC ] [%4d ~ %4d m]\r\n", start, end);

							if(ls.stat.NewEventDistanceGap >= start && ls.stat.NewEventDistanceGap < end)
							{
								Set_DRM(&buff[8 + offset]);

								ls.stat.EventGroupID = i;
								if(ls.wdis.isBLEOnOff == pdTRUE)
								{
									if(ls.wdis.situation != device_id || ls.stat.EventDistanceGap != ls.stat.NewEventDistanceGap)
									{
										printf("[LOGIC ] BLE AdvData Set\r\n");
										ls.stat.EventDistanceGap = ls.stat.NewEventDistanceGap;
										ls.wdis.situation = device_id;
										ls.stat.isBLEADVUpdate = pdTRUE;

										//Send_BLE_check();
										//Send_BLE_ADVDATA_Situation(ls.modem.SectionID, ls.wdis.situation, ls.stat.EventDistanceGap);
										//Send_BLE_check();
										ls.stat.isEventControl = pdTRUE;
									}
								}
								break;
							}
						}
					}
					break;

		case DRM_CMD_INIT_SET_V2:
			ls.stat.istallDistance = buff[0];
			for (int i = 0; i < 25; i++) {
				temp_id = buff[i + 1];
				if (temp_id == ls.modem.DeviceID) {
					//ls.stat.initID = buff[(i * 2) + 2];
					ls.stat.initPosition = i * ls.stat.istallDistance;
					printf("[LOGIC ] [%d]Init Position - [%d][%4d m]\r\n", ls.modem.DeviceID, i, ls.stat.initPosition);
					break;
				}
			}
			break;

		case DRM_CMD_EVENT_GRP_CTL:
			if(ls.stat.isEventControl == pdFALSE)
				break;

			for (int i = 0; i < buff[0]; i++) {
				if(ls.stat.EventGroupID == buff[(i * 2) + 1])
				{
					ls.wdis.OnOff = buff[(i * 2) + 2];

					printf("[LOGIC ] [%d] Flashing Control - [%d]\r\n",
												ls.stat.EventGroupID, ls.wdis.OnOff);
					break;
				}
			}
			break;

		case DRM_CMD_ST_REQ:			//Status Request
			ls.tick.Send = ls.tick.Tick;
			ls.stat.isStatusCall = pdTRUE;
			break;

		case DRM_CMD_RESET:				//Reset
			StandByMode();
			NVIC_SystemReset();
			break;

		case DRM_CMD_MODE_SET:				//Display Mode
			if(buff[0])
				ls.stat.isTestMode = pdFALSE;
			else
			{
				ls.tdis.Mode = 0;
				ls.tdis.Pos = 0;
				ls.stat.isTestMode = pdTRUE;
			}
			break;

		case DRM_CMD_DISP_SET:
			ls.wdis.OnOff = buff[0];

			break;
		case DRM_CMD_BLE_CTL:
				ls.wdis.isBLEOnOff = buff[0];
				ls.stat.isBLEADVCall = pdTRUE;

			break;
		case DRM_CMD_BLE_ADV:
			ls.ble.isADVInterval = (buff[0] << 8) | buff[1];

			Send_BLE_check();

			Send_BLE_INTERVAL_SET(ls.ble.isADVInterval);

			Send_BLE_check();
			break;
		default:
			break;
	}
}

void Set_DRM(uint8_t* ptr)
 {
	ls.wdis.Brt = *ptr;
	printf("[LOGIC ] Set DRM [BRT:%d]", *ptr);

	ptr++;
	printf("[RGB:%02x]", *ptr);

	ls.wdis.Disp_Led.red = (*ptr & 0xC0) >> 6;
	ls.wdis.Disp_Led.grn =  (*ptr & 0x30) >> 4;
	ls.wdis.Disp_Led.blu = (*ptr & 0x0C) >> 2;
	ls.wdis.isIR = (*ptr & 0x02) >> 1;
	//ls.wdis.isBLEOnOff = *ptr & 0x01;

	ptr++;
	printf("[OPT:%02x]\r\n", *ptr);

	ls.wdis.DispLens = *ptr & 0x80 ? pdTRUE : pdFALSE;
	ls.wdis.isFlash = *ptr & 0x40 ? pdTRUE : pdFALSE;
	ls.wdis.FlashMode = *ptr & 0x20 ? pdTRUE : pdFALSE;
	ls.wdis.Flashing_InterVal = *ptr & 0x1F;

	ls.tick.Send  = ls.tick.Tick;
	ls.wdis.Pos = 1;
	ls.wdis.FlashCount = 0;
}


uint8_t send_buf[128];

void Send_Standby()
{
	printf("[LOGIC] Modem Send_Standby\r\n");
	uint8_t bp = 0;
	uint16_t calc_lrc = 0;

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;			//STX
	send_buf[bp++] = 0x04;			//Length~LRC Size
	send_buf[bp++] = RF_CMD_REG_WRITE;			//OP-Code(Display Data)

	send_buf[bp++] = 0x11;

	send_buf[bp++] = 0xFF; 	//CRC;
	send_buf[bp++] = 0x7F;

	commSendData(MODEM_UART_PORT, send_buf, bp);
}

void Send_Modem_Status()
{
	uint8_t bp = 0;
	uint8_t temp_data = 0;
	uint16_t calc_lrc = 0;

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));
	//RAM_Read();
	send_buf[bp++] = FIRMWARE_CMD_REG_RSP;			//OP-Code(Display Data)
	send_buf[bp++] = 1;		//frame
	send_buf[bp++] = 1;		//totalframe
	send_buf[bp++] = 0;
	send_buf[bp++] = 10;		//Length~LRC Size

	send_buf[bp++] = ls.modem.Status_Control;
	send_buf[bp++] = ls.modem.SectionID;
	send_buf[bp++] = ls.modem.DeviceID;
	send_buf[bp++] = ls.modem.Rf_ch;
	send_buf[bp++] = ls.ble.mac_address[0];
	send_buf[bp++] = ls.ble.mac_address[1];
	send_buf[bp++] = ls.ble.mac_address[2];
	send_buf[bp++] = ls.ble.mac_address[3];
	send_buf[bp++] = ls.ble.mac_address[4];
	send_buf[bp++] = ls.ble.mac_address[5];

	send_buf[bp++] = 0; 	//CRC;
	send_buf[bp++] = 0; 	//CRC;

	periSetBLE(GPIO_PIN_RESET);

	printf("[LOGIC] Send_Modem_Status[%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x]\r\n",
			send_buf[5],send_buf[6],send_buf[7],send_buf[8],send_buf[9],send_buf[10],send_buf[11],send_buf[12],send_buf[13]);
	commSendData(BLE_UART_PORT, send_buf, bp);
}

void Send_Status()
{
	uint8_t bp = 0;
	uint8_t temp_data = 0;
	uint16_t calc_lrc = 0;
	uint32_t sum = 0;

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;			//STX
	send_buf[bp++] = 19;			//Length~LRC Size
	send_buf[bp++] = RF_CMD_RECV;			//OP-Code(Display Data)
	send_buf[bp++] = ls.modem.SectionID;
	send_buf[bp++] = 0;
	send_buf[bp++] = 14;
	send_buf[bp++] = ls.modem.DeviceID;
	send_buf[bp++] = DRM_CMD_ST_RSP;
	send_buf[bp++] = ls.wdis.Brt;
	send_buf[bp++] = ls.wdis.Disp_Led.red;
	send_buf[bp++] = ls.wdis.Disp_Led.grn;
	send_buf[bp++] = ls.wdis.Disp_Led.blu;

	temp_data |= ls.wdis.isIR ? 0x80 : 0x00;
	temp_data |= ls.wdis.isFlash ? 0x40 : 0x00;
	temp_data |= ls.wdis.isBLEOnOff ? 0x20 : 0x00;
	temp_data |= (ls.wdis.Flashing_InterVal & 0x1F);

	send_buf[bp++] = temp_data;
	send_buf[bp++] = (ls.cds.Battery >> 8) & 0xFF;
	send_buf[bp++] = ls.cds.Battery & 0xFF;
	send_buf[bp++] = (ls.cds.Solar >> 8) & 0xFF;
	send_buf[bp++] = ls.cds.Solar & 0xFF;
	send_buf[bp++] = (ls.cds.Temp >> 8) & 0xFF;
	send_buf[bp++] = ls.cds.Temp & 0xFF;

	send_buf[bp++] = CalcLRC(&send_buf, 1, bp - 1); 	//CRC;
	send_buf[bp++] = 0x7F;
	send_buf[bp++] = 0x0D;
	send_buf[bp++] = 0x0A;

	commSendData(MODEM_UART_PORT, send_buf, bp);

	printf("[LOGIC] Modem Send_Status\r\n");
}

void Send_Register()
{
	uint8_t bp = 0;
	uint8_t temp_data = 0;
	uint16_t calc_lrc = 0;

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;			//STX
	send_buf[bp++] = 14;			//Length~LRC Size
	send_buf[bp++] = RF_CMD_RECV;			//OP-Code(Display Data)
	send_buf[bp++] = ls.modem.SectionID;
	send_buf[bp++] = 0;
	send_buf[bp++] = 9;
	send_buf[bp++] = ls.modem.DeviceID;
	send_buf[bp++] = DRM_CMD_REG_RSP;
	send_buf[bp++] = ls.ble.mac_address[0];
	send_buf[bp++] = ls.ble.mac_address[1];
	send_buf[bp++] = ls.ble.mac_address[2];
	send_buf[bp++] = ls.ble.mac_address[3];
	send_buf[bp++] = ls.ble.mac_address[4];
	send_buf[bp++] = ls.ble.mac_address[5];

	printf("[LOGIC] Modem Send_Register[%d][%02x:%02x:%02x:%02x:%02x:%02x]\r\n", ls.modem.DeviceID,
			ls.ble.mac_address[0], ls.ble.mac_address[1], ls.ble.mac_address[2],
			ls.ble.mac_address[3], ls.ble.mac_address[4], ls.ble.mac_address[5]);

	send_buf[bp++] = CalcLRC(&send_buf, 1, bp - 1); 	//CRC;
	send_buf[bp++] = 0x7F;

	commSendData(MODEM_UART_PORT, send_buf, bp);
}

static void PatternVertical(LEDDATA_T* image, uint8_t pattern)
{
    //const uint8_t patt = pattern % 5;
    const uint8_t value = 0x01 << pattern;

    image->red = value;
    image->grn = value;
    image->blu = value;
}

static void DisplayTestMode()
{

	if (CalcTickDiff(ls.tdis.Tick, ls.tick.Tick) <= ls.tdis.Delay) {
		return;
	}

	ls.tdis.Tick = ls.tick.Tick;

    uint32_t delay = 0;
    uint32_t bright = 0;
    memset(&ls.tdis.Disp_Led, 0x00, sizeof(LEDDATA_T));


    switch (ls.tdis.Mode)
    {
		case 0:
			ls.tdis.Disp_Led.red = 0xFF;
			ls.tdis.Mode += 1;
			bright = TEST_BRIGHT_ALL;
			delay = TEST_DELAY_LONG;
			break;

		case 1:
			ls.tdis.Disp_Led.grn = 0xFF;
			ls.tdis.Mode += 1;
			bright = TEST_BRIGHT_ALL;
			delay = TEST_DELAY_LONG;
			break;

		case 2:
			ls.tdis.Disp_Led.blu = 0xFF;
			ls.tdis.Mode += 1;
			bright = TEST_BRIGHT_ALL;
			delay = TEST_DELAY_LONG;
			break;

		case 3:
			ls.tdis.Disp_Led.red = 0xFF;
			ls.tdis.Disp_Led.grn = 0xFF;
			ls.tdis.Mode += 1;
			bright = TEST_BRIGHT_W1;
			delay = TEST_DELAY_LONG;
			break;

		case 4:
			ls.tdis.Disp_Led.red = 0xFF;
			ls.tdis.Disp_Led.grn = 0xFF;
			ls.tdis.Disp_Led.blu = 0xFF;
			ls.tdis.Mode += 1;
			bright = TEST_BRIGHT_W1;
			delay = TEST_DELAY_LONG;
			break;

		case 5:
			ls.tdis.Disp_Led.red = 0xFF;
			ls.tdis.Disp_Led.grn = 0xFF;
			ls.tdis.Disp_Led.blu = 0xFF;
			ls.tdis.Mode += 1;
			bright = TEST_BRIGHT_W2;
			delay = TEST_DELAY_LONG;
			break;

		case 6:
			ls.tdis.Disp_Led.ir = 0xFF;
			ls.tdis.Mode += 1;
			bright = TEST_BRIGHT_ALL;
			delay = TEST_DELAY_LONG;
			break;

		case 7:
			PatternVertical(&ls.tdis.Disp_Led, ls.tdis.Pos);
			if (++ls.tdis.Pos == 3)
			{
				ls.tdis.Pos = 0;
				ls.tdis.Mode += 1;
			}
			bright = TEST_BRIGHT_ALL;
			delay = TEST_DELAY_SHORT;
			break;

		default:
			break;
    }

    dispSetBright(bright);

    dispSetData(&ls.tdis.Disp_Led);
    dispRefresh();

    printf("[LOGIC] DisplayTestMode() Mode[%d]Pos[%d]\r\n", ls.tdis.Mode, ls.tdis.Pos);

    const uint8_t MAX_DISP_MODE = 7;
    if (ls.tdis.Mode == MAX_DISP_MODE)
    {
        ls.tdis.Mode = 0;
    }

    ls.tdis.Delay = delay;

}

static void DisplayTestWhiteMode()
{
	if (CalcTickDiff(ls.tdis.Tick, ls.tick.Tick) <= TEST_DELAY_LONG) {
		return;
	}

	ls.tdis.Tick = ls.tick.Tick;

    memset(&ls.tdis.Disp_Led, 0x00, sizeof(LEDDATA_T));

	ls.tdis.Disp_Led.red = 0xFF;
	ls.tdis.Disp_Led.grn = 0xFF;
	ls.tdis.Disp_Led.blu = 0xFF;
	ls.tdis.Disp_Led.ir = 0xFF;

    dispSetBright(TEST_BRIGHT_MAX);

    dispSetData(&ls.tdis.Disp_Led);
    dispRefresh();
}

static void DisplayTestOffMode()
{
	if (CalcTickDiff(ls.tdis.Tick, ls.tick.Tick) <= TEST_DELAY_LONG) {
		return;
	}

	ls.tdis.Tick = ls.tick.Tick;

    memset(&ls.tdis.Disp_Led, 0x00, sizeof(LEDDATA_T));

	ls.tdis.Disp_Led.red = 0x00;
	ls.tdis.Disp_Led.grn = 0x00;
	ls.tdis.Disp_Led.blu = 0x00;
	ls.tdis.Disp_Led.ir = 0x00;

    dispSetBright(TEST_BRIGHT_MAX);

    dispSetData(&ls.tdis.Disp_Led);
    dispRefresh();
}

static void DisplayWorkingMode()
{
	if (CalcTickDiff(ls.wdis.Tick, ls.tick.Tick) <= 10) {
		return;
	}

	ls.wdis.Tick = ls.tick.Tick;

	if(ls.stat.isGroupControl == pdFALSE && ls.stat.isEventControl != pdTRUE)
	{
		if (ls.wdis.isFlash) {

			if (ls.wdis.FlashCount++ >= (ls.wdis.Flashing_InterVal * 10)) {
				ls.wdis.FlashCount = 0;
				if (ls.wdis.Pos++ >= 1) {
					ls.wdis.Pos = 0;
				}
			}
		} else {
			ls.wdis.Pos = 1;
			ls.wdis.FlashCount = 0;
		}

		if (ls.wdis.isIRFlash) {

			if (ls.wdis.IRFlashCount++ >= (ls.wdis.Flashing_InterVal * 10)) {
				ls.wdis.IRFlashCount = 0;
				if (ls.wdis.IRPos++ >= 1) {
					ls.wdis.IRPos = 0;
				}
			}
		} else {
			ls.wdis.IRPos = 1;
			ls.wdis.IRFlashCount = 0;
		}
	}else if(ls.stat.isEventControl)
	{
		if(ls.wdis.isFlash)
			ls.wdis.Pos = ls.wdis.OnOff;
		else
			ls.wdis.Pos = pdTRUE;

		if(ls.wdis.isIRFlash)
			ls.wdis.IRPos = ls.wdis.OnOff;
		else
			ls.wdis.IRPos = pdTRUE;
	}
	else
	{
		if (ls.wdis.isFlash) {
			if (ls.wdis.Flashing_InterVal < 5) {
				if (ls.wdis.FlashCount++ >= (ls.wdis.Flashing_InterVal * 10)) {
					ls.wdis.FlashCount = 0;
					if (ls.wdis.Pos++ >= 1) {
						ls.wdis.Pos = 0;
					}
				}
			} else {
				ls.wdis.Pos = ls.wdis.OnOff;
			}
		}

		if (ls.wdis.isIRFlash) {
			if (ls.wdis.Flashing_InterVal < 5) {
				if (ls.wdis.IRFlashCount++ >= (ls.wdis.Flashing_InterVal * 10)) {
					ls.wdis.IRFlashCount = 0;
					if (ls.wdis.IRPos++ >= 1) {
						ls.wdis.IRPos = 0;
					}
				}
			} else {
				ls.wdis.IRPos = ls.wdis.OnOff;
			}
		} else {
			ls.wdis.IRPos = 1;
			ls.wdis.IRFlashCount = 0;
		}
	}



    if(ls.wdis.Pos)
    {
    	if(ls.wdis.Disp_Led.red)
    		ls.wdis.Dummy_Led.red = 0xFF;
    	else
    		ls.wdis.Dummy_Led.red = 0x00;

    	if(ls.wdis.Disp_Led.grn)
    		ls.wdis.Dummy_Led.grn = 0xFF;
    	else
    		ls.wdis.Dummy_Led.grn = 0x00;

    	if(ls.wdis.Disp_Led.blu)
    	    ls.wdis.Dummy_Led.blu = 0xFF;
    	else
    	    ls.wdis.Dummy_Led.blu = 0x00;
    }
    else
    {
    	ls.wdis.Dummy_Led.red = 0x00;
    	ls.wdis.Dummy_Led.grn = 0x00;
    	ls.wdis.Dummy_Led.blu = 0x00;
    }

    if(ls.wdis.IRPos)
    {
		if(ls.wdis.isIR)
			ls.wdis.Dummy_Led.ir = 0xFF;
		else
			ls.wdis.Dummy_Led.ir = 0x00;
    }
    else
	{
    	ls.wdis.Dummy_Led.ir = 0x00;
	}

	uint32_t bright = ls.wdis.Brt;

	dispSetBright(bright);

    dispSetData(&ls.wdis.Dummy_Led);
    dispRefresh();

    //printf("[LOGIC] DisplayWorkingMode() Pos[%d][%02X/%02X/%02X]\r\n", ls.wdis.Pos, ls.wdis.Dummy_Led.red, ls.wdis.Dummy_Led.grn, ls.wdis.Dummy_Led.blu);
}

void Send_BLE_check()
{
	//printf("[LOGIC] Send_BLE_check \r\n");
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT\r");

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
}


void Send_BLE_Info_REQ()
{
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));
   ls.ble.isInfoCall = pdTRUE;

   sprintf(ls.ble.command_str, "AT+INFO?\r");

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
   //printf("[LOGIC] Send_BLE_cmd : %s \n", ls.ble.command_str);
}

void Send_BLE_PacketMode_REQ()
{
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT+VER?\r");

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
   //printf("[LOGIC] Send_BLE_cmd : %s \n", ls.ble.command_str);
}

void Send_BLE_ADVDATA_Status_REQ()
{
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT+ADVDATA?\r");

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
   //printf("[LOGIC] Send_BLE_cmd : %s", ls.ble.command_str);

   //Send_BLE_ADVDATA_Status_RSP();
}

void Send_BLE_ADV_CTL(uint8_t onoff)
{
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   if(onoff)
	   sprintf(ls.ble.command_str, "AT+ADVON\r");
   else
	   sprintf(ls.ble.command_str, "AT+ADVOFF\r");

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
}

void Send_BLE_ADVDATA_Status_RSP()
{
	uint8_t bp = 0;
	//printf("[LOGIC] command_str : %s", ls.ble.command_str);

	osEvent uartevent = osMessageGet(bleQueueHandle, 0);
		while (uartevent.status == osEventMessage) {

			COMMAND_T Packet;

			memcpy(&Packet, (COMMAND_T*)uartevent.value.v, sizeof(COMMAND_T));

			//printf("\r\n[Logic] Firmware OP_CODE [%2X]\r\n", Packet.opcode);

					memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

					send_buf[bp++] = FIRMWARE_CMD_REG_RSP;			//OP-Code(Display Data)
					send_buf[bp++] = 1;			//frame
					send_buf[bp++] = 1;			//totalframe
					send_buf[bp++] = 0;			//STX
					send_buf[bp++] = 13;		//Length~LRC Size

					send_buf[bp++] = Packet.data[0];
					send_buf[bp++] = Packet.data[1];
					send_buf[bp++] = Packet.data[2];
					send_buf[bp++] = Packet.data[3];
					send_buf[bp++] = Packet.data[4];
					send_buf[bp++] = Packet.data[5];
					send_buf[bp++] = Packet.data[6];
					send_buf[bp++] = Packet.data[7];
					send_buf[bp++] = Packet.data[8];
					send_buf[bp++] = Packet.data[9];
					send_buf[bp++] = Packet.data[10];
					send_buf[bp++] = Packet.data[11];
					send_buf[bp++] = Packet.data[12];


					send_buf[bp++] = 0; 	//CRC;
					send_buf[bp++] = 0; 	//CRC;

					printf("[LOGIC] Send_BLE_ADVDATA_Status[%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c][%c]\r\n",
							send_buf[5],send_buf[6],send_buf[7],send_buf[8],send_buf[9],send_buf[10],send_buf[11],send_buf[12],send_buf[13],send_buf[14],send_buf[15],send_buf[16],send_buf[17]);
			uartevent = osMessageGet(bleQueueHandle, 0);
		}
}

void Send_BLE_ADVDATA_Situation(uint8_t section, uint8_t situ, uint16_t loca)
{
	uint8_t bp = 0;
	//printf("[LOGIC] Send_BLE_ADVDATA_Situation\r\n");
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   send_buf[bp++] = section;
   send_buf[bp++] = situ;
   send_buf[bp++] = loca/1000;
   send_buf[bp++] = (loca/100)%10;
   send_buf[bp++] = (loca/10)%10;
   send_buf[bp++] = loca%10;

   sprintf(ls.ble.command_str, "AT+ADVDATA=%d%d%04d\r", section, situ, loca);

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
}

void Send_BLE_ADVDATA_SET()
{
	//printf("[LOGIC] BLE_ADVDATA_SET\r\n");
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT+ADVDATA=%s\r", ls.ble.data);

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
   printf("[LOGIC] Send_BLE_cmd : %s", ls.ble.command_str);
}

void Send_BLE_MANUF_Status_REQ()
{
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT+MANUF?\r");

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
   //printf("[LOGIC] Send_BLE_cmd : %s", ls.ble.command_str);
}

void Send_BLE_MANUF_SET()
{
	//printf("[LOGIC] BLE_MANUF_SET\r\n");
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT+MANUF=DRM\r");

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
   printf("[LOGIC] Send_BLE_cmd : %s", ls.ble.command_str);
}

void Send_BLE_INTERVAL_SET(uint16_t interval)
{
	//printf("[LOGIC] Send_BLE_INTERVAL_SET %d\r\n", interval);
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT+ADVINTERVAL=%d\r", interval);

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
   //printf("[LOGIC] Send_BLE_cmd : %s", ls.ble.command_str);
}

void Send_BLE_UART_SET(uint32_t baud)
{
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT+UART=%d\r", baud);
   //printf("[LOGIC] Send_BLE_cmd : %s \n", ls.ble.command_str);

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
}

void Read_Params()
{
	memset(&ls.modem, 0x00, sizeof(MODEM_T));
	ReadFRAMMemory(0x00, &ls.modem, sizeof(MODEM_T));
	Print_Params();
}

void Save_Params(uint8_t* ptr, uint16_t size)
{
	SaveFRAMMemory(0x00, ptr, size);
	Read_Params();
}

void Print_Params()
{
	printf("[LOGIC] =============Modem Register============\r\n");
	printf("[LOGIC] 1. Section ID  : [ %02x ]\r\n", ls.modem.SectionID);
	printf("[LOGIC] 2. Device ID   : [ %02x ]\r\n", ls.modem.DeviceID);
	printf("[LOGIC] 3. RF Channel  : [ %02x ]\r\n", ls.modem.Rf_ch);
	printf("[LOGIC] =======================================\r\n");
}

void SaveFRAMMemory(uint32_t address, uint8_t *pData, uint16_t Size) {
	//printf("[logic.c] SaveFRAMMemory - Size [%d/%d]\r\n", address, Size);
	/// I2C F-RAM Write
	memset(&ls.FRAM_SaveBuff, 0x00, BUFF_LEN);

	uint8_t DevAddress = 0x00;
	uint16_t SplitSize1 = 0;
	uint16_t SplitSize2 = 0;
	uint16_t SubAddress1 = 0;
	uint16_t SubAddress2 = 0;
	HAL_StatusTypeDef stat = HAL_ERROR;

	if (address > 0xFFFF)	//Page 1
	{
		DevAddress = 0xA2;
		SubAddress2 = address - 0xFFFF;
		ls.FRAM_SaveBuff[0] = (uint8_t) (SubAddress2 >> 8) & 0x00FF;
		ls.FRAM_SaveBuff[1] = (uint8_t) (SubAddress2 >> 0) & 0x00FF;
		memcpy(&ls.FRAM_SaveBuff[2], pData, Size);

		stat = HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &ls.FRAM_SaveBuff, 2+Size, 0xFFFF);
		HAL_Delay(20);

		if(stat != HAL_OK)
			printf("[logic.c] Save Page1 Transmit Result [%d][%2x]\r\n", stat, hi2c1.ErrorCode);
	} else {
		if ((address + Size) > 0xFFFF)	//Page 0 + 1
		{
			SplitSize1 = 0xFFFF - address;
			SubAddress1 = address;
			DevAddress = 0xA0;
			ls.FRAM_SaveBuff[0] = (uint8_t) (SubAddress1 >> 8) & 0x00FF;
			ls.FRAM_SaveBuff[1] = (uint8_t) (SubAddress1 >> 0) & 0x00FF;
			memcpy(&ls.FRAM_SaveBuff[2], pData, SplitSize1);
			stat = HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &ls.FRAM_SaveBuff[0], SplitSize1 + 2, 0xFFFF);
			HAL_Delay(20);

			if(stat != HAL_OK)
				printf("[logic.c] Save Page0+1 [0] Transmit Result [%d][%2x]\r\n", stat, hi2c1.ErrorCode);
			stat = HAL_ERROR;

			pData += SplitSize1;
			DevAddress = 0xA2;
			SplitSize2 = Size - SplitSize1;
			SubAddress2 = 0;
			ls.FRAM_SaveBuff[0] = (uint8_t) (SubAddress2 >> 8) & 0x00FF;
			ls.FRAM_SaveBuff[1] = (uint8_t) (SubAddress2 >> 0) & 0x00FF;
			memcpy(&ls.FRAM_SaveBuff[2], pData, SplitSize2);
			stat = HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &ls.FRAM_SaveBuff[0], SplitSize2 + 2, 1000);
			HAL_Delay(20);



			if(stat != HAL_OK)
				printf("[logic.c] Save Page0+1 [1] Transmit Result [%d][%2x]\r\n", stat, hi2c1.ErrorCode);
		} else		//Page 0
		{
			DevAddress = 0xA0;
			SubAddress1 = address;
			ls.FRAM_SaveBuff[0] = (uint8_t) (SubAddress1 >> 8) & 0x00FF;
			ls.FRAM_SaveBuff[1] = (uint8_t) (SubAddress1 >> 0) & 0x00FF;
			memcpy(&ls.FRAM_SaveBuff[2], pData, Size);

			stat = HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &ls.FRAM_SaveBuff[0], Size + 2, 0xFFFF);
			HAL_Delay(20);

			if(stat != HAL_OK)
				printf("[logic.c] Save Page0 Transmit Result [%d][%2x]\r\n", stat, hi2c1.ErrorCode);
		}
	}
}

void ReadFRAMMemory(uint32_t address, uint8_t *pData, uint16_t Size) {
	uint8_t fram_buf[2];
	uint8_t DevAddress = 0x00;
	uint16_t SplitSize1 = 0;
	uint16_t SplitSize2 = 0;
	uint16_t SubAddress1 = 0;
	uint16_t SubAddress2 = 0;
	HAL_StatusTypeDef stat = HAL_ERROR;

	if (address > 0xFFFF)	//Page 1
	{
		DevAddress = 0xA2;
		SubAddress2 = address - 0xFFFF;
		fram_buf[0] = (uint8_t) (SubAddress2 >> 8) & 0x00FF;
		fram_buf[1] = (uint8_t) (SubAddress2 >> 0) & 0x00FF;

		stat = HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &fram_buf[0], 2, 1000);
		HAL_Delay(20);
		if(stat != HAL_OK)
			printf("[logic.c] Page1 Transmit Result [%d][%2x]\r\n", stat, hi2c1.ErrorCode);

		stat = HAL_ERROR;
		stat = HAL_I2C_Master_Receive(&hi2c1, DevAddress, pData, Size, 1000);
		if(stat != HAL_OK)
			printf("[logic.c] Page1 Receive Result [%d][%2x]\r\n", stat, hi2c1.ErrorCode);

		HAL_Delay(20);
	} else {
		if ((address + Size) > 0xFFFF)	//Page 0 + 1
		{
			SplitSize1 = 0xFFFF - address;
			SubAddress1 = address;
			DevAddress = 0xA0;
			stat = HAL_ERROR;

			fram_buf[0] = (uint8_t) (SubAddress1 >> 8) & 0x00FF;
			fram_buf[1] = (uint8_t) (SubAddress1 >> 0) & 0x00FF;
			stat = HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &fram_buf[0], 2, 1000);
			HAL_Delay(20);
			if (stat != HAL_OK)
				printf("[logic.c] Page 0 - 1[1] Transmit Result [%d][%2x]\r\n", stat, hi2c1.ErrorCode);

			stat = HAL_ERROR;

			stat = HAL_I2C_Master_Receive(&hi2c1, DevAddress, pData, SplitSize1, 1000);
			HAL_Delay(20);
			if (stat != HAL_OK)
				printf("[logic.c] Page 0 - 1[1] Receive Result [%d][%2x]\r\n", stat, hi2c1.ErrorCode);

			stat = HAL_ERROR;

			pData += SplitSize1;
			DevAddress = 0xA2;
			SubAddress2 = 0;
			SplitSize2 = Size - SplitSize1;
			stat = HAL_ERROR;

			fram_buf[0] = (uint8_t) (SubAddress2 >> 8) & 0x00FF;
			fram_buf[1] = (uint8_t) (SubAddress2 >> 0) & 0x00FF;
			stat = HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &fram_buf[0], 2, 1000);
			HAL_Delay(20);
			if (stat != HAL_OK)
				printf("[logic.c] Page 0 - 1[2] Transmit Result [%d][%2x]\r\n", stat, hi2c1.ErrorCode);

			stat = HAL_ERROR;

			stat = HAL_I2C_Master_Receive(&hi2c1, DevAddress, pData, SplitSize2, 1000);
			HAL_Delay(20);
			if (stat != HAL_OK)
				printf("[logic.c] Page 0 - 1[2] Receive Result [%d][%2x]\r\n", stat, hi2c1.ErrorCode);

			stat = HAL_ERROR;
		} else		//Page 0
		{
			DevAddress = 0xA0;
			SubAddress1 = address;
			stat = HAL_ERROR;
			fram_buf[0] = (uint8_t) (SubAddress1 >> 8) & 0x00FF;
			fram_buf[1] = (uint8_t) (SubAddress1 >> 0) & 0x00FF;
			stat = HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &fram_buf[0], 2, 1000);
			HAL_Delay(20);
			if (stat != HAL_OK)
				printf("[logic.c] Page 0 Transmit Result [%d][%2x]\r\n", stat, hi2c1.ErrorCode);

			stat = HAL_ERROR;
			stat = HAL_I2C_Master_Receive(&hi2c1, DevAddress, pData, Size, 1000);
			HAL_Delay(20);

			if (stat != HAL_OK)
				printf("[logic.c] Page 0 Receive Result [%d][%2x]\r\n", stat, hi2c1.ErrorCode);

			stat = HAL_ERROR;
		}
	}
}


