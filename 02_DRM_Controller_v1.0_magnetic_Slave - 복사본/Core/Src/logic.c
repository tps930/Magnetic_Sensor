/*
 * logic.c
 *
 *  Created on: 2019. 9. 3.
 *      Author: S.S.C
 */

#define TITLE  	"LEDO Co., Ltd "
#define VERSION  "HW:DRM Ver1.1 SW : v1.1.0 "

char swTITLE[32] = TITLE;
char swVER[32] = VERSION;

#include <string.h>
#include <stdio.h>
#include "stm32l0xx_hal.h"
#include "cmsis_os.h"
#include "logic.h"
#include "uart.h"

#define CALIB_SAMPLES          100      // 10 Hz에서 5초 동안 수집 → 50개가 딱 맞습니다
#define K_SIGMA                4.5f    // 필드용 권장 (오검 줄임). 벤치에서 약하면 4.2까지
#define T_ABS_MIN              12      // ≈ 1.8 µT (LIS2MDL ~0.15 µT/LSB 가정)
#define EMA_BETA_PERMILLE      1       // idle일 때 기준선 아주 느리게 추적(0.1%/샘플 ≈ 100 s)
#define IDLE_BAND_DIV          3       // 기준선 추적은 ±T/3 안에서만 (이벤트에 끌려가지 않음)
#define DEBOUNCE_MS            150     // 10 Hz에서 최소 2샘플(≥200 ms) 권장, 시작값 150 ms
#define REFRACT_MS             300     // 0.3 s(=3샘플) 휴지구간 → 재중복 방지

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

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOC)) {
		ls.cds.Stat = pdTRUE;
		ls.cds.Cds_Array[0][ls.cds.Count] = ls.cds.Value[0];
		ls.cds.Cds_Array[1][ls.cds.Count] = ls.cds.Value[1];
		ls.cds.Cds_Array[2][ls.cds.Count] = ls.cds.Value[2];
		ls.cds.Cds_Array[3][ls.cds.Count] = ls.cds.Value[3];
		HAL_ADC_Stop_DMA(hadc);
		//ls.cds.Value = HAL_ADC_GetValue(hadc);
	}
}

TickType_t CalcTickDiff(TickType_t oldTicks, TickType_t newTicks)
{
    TickType_t ret;
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


void logicInit()
{
	printf("======================================\r\n");
	printf("  Magnetic Controller v1.0(Slave)\r\n");
	printf("  LEDO Co,. Ltd. \r\n");
	printf("======================================\r\n");

	periInit();
	//dispInit();

	memset(&ls, 0x00, sizeof(LogicStatus_t));

	ls.ble.isInit = pdFALSE;
	Magnetic_Sensor_Init();

/*    dispSetBright(TEST_BRIGHT_W1);
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
}

void logicRFModemInit()
{
	/*uint8_t rfset = 0x00;
	Read_Params();

	rfset = 0x00;

	if (ls.modem.SectionID)
		rfset |= 0x04;

	rfset |= ls.modem.Rf_ch;*/

	uint8_t rfset = 0x00;

	rfset = 0x00;
	rfset |= 0x04;
	rfset |= 0x03;

	periRFModule(rfset);
	periRFWakeup(pdTRUE);
	periRFReset();
}

void logicBLEInit()
{
	periSetBLE(GPIO_PIN_SET);		//Command Mode
	ls.ble.isMode = pdFALSE;

	Send_BLE_check();

	Send_BLE_MANUF_SET();

	Send_BLE_check();

	Send_BLE_PacketMode_REQ();
	for(int i = 0; i < 3; i++)
	{
		UART_Baud(&huart5, Baud[i]);
		Send_BLE_check();

		Send_BLE_UART_SET(115200);
		Send_BLE_check();
	}

	if(ls.ble.isInit)
	{
		Send_BLE_check();

		Send_BLE_MANUF_SET();

		Send_BLE_check();


	}

	if (ls.ble.isConnected) {
		ls.ble.isMode = pdTRUE;
		periSetBLE(GPIO_PIN_RESET);		//Bypass Mode
	}

	ls.wdis.isBLEOnOff = pdTRUE;
	ls.stat.isBLEADVCall = pdTRUE;



}

void logicPeriTask()
{
	if (CalcTickDiff(ls.tick.Indi, ls.tick.tickNow) <= 1000) {
		return;
	}

	if(ls.stat.isCommInit == pdFALSE)
	{
		ls.stat.isCommInit = pdTRUE;
		commInit();
		ls.stat.isRegister = pdTRUE;
	}


	if (ls.stat.isRegister == pdTRUE) {
		ls.stat.isRegister = pdFALSE;
		logicRFModemInit();
	}

	if (ls.ble.isInit == pdFALSE) {
		//logicBLEInit();
	}

	ls.tick.Indi = ls.tick.tickNow;

	if (ls.indi.Program) {
		ls.indi.Program = pdFALSE;
	} else
		ls.indi.Program = pdTRUE;

	periSetLamp(ls.indi.Program);

	//printf("[Logic] logicPeriTask\r\n");

/*	uint8_t ble = 0x00;

	ble = periGetBLE();

	if (ls.ble.isConnected != ble) {
		ls.ble.isConnected = ble;
		if (ls.ble.isConnected) {
			printf("BLE [ CONNECTED    ]\r\n");
			ls.ble.isMode = pdTRUE;
			//periSetBLE(GPIO_PIN_RESET);		//Bypass Mode
		} else {
			printf("BLE [ DISCONNECTED ]\r\n");
			ls.ble.isMode = pdFALSE;
			//periSetBLE(GPIO_PIN_SET);		//AT Command Mode
		}
	}
	if(ls.stat.preADVStat)
	{
		periBLEWakeup(pdFALSE);
		periBLEUART(pdFALSE);
		//Send_BLE_PacketMode_REQ();
		ls.stat.preADVStat = pdFALSE;
	}


	if (CalcTickDiff(ls.tick.Cds, ls.tick.tickNow) <= 5000) {
		return;
	}

	ls.tick.Cds = ls.tick.tickNow;


	for(int i = 0; i < 10; i++)
	{
		ls.cds.Stat = pdFALSE;
		ls.cds.Count = i;
		HAL_ADC_Start_DMA(&hadc, (uint32_t*)ls.cds.Value, 4);

		while(ls.cds.Stat == pdFALSE){}
	}
	ls.cds.Cds_Sum[0] = 0;
	ls.cds.Cds_Sum[1] = 0;
	ls.cds.Cds_Sum[2] = 0;
	ls.cds.Cds_Sum[3] = 0;

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
		ls.cds.Cds_Avg[i] = ls.cds.Cds_Sum[i] / 10;
	}

	printf("[LOGIC] ADC Value [CH1 : %d][CH2 : %d][CH3 : %d][CH4 : %d]\r\n", ls.cds.Cds_Avg[0], ls.cds.Cds_Avg[1], ls.cds.Cds_Avg[2], ls.cds.Cds_Avg[3]);

	double dv = ls.cds.Cds_Avg[2] * 1.0;
	double fVoltage = 0.0;
	fVoltage = ((3.3 * dv)/4096.0) * ((1.0+1.0)/1.0);

	ls.cds.fBattery = fVoltage;
	ls.cds.Battery = ls.cds.fBattery * 10;

	dv = ls.cds.Cds_Avg[3] * 1.0;
	fVoltage = ((3.3 * dv)/4096.0) * ((1.0+1.0)/1.0);
	ls.cds.fSolar = fVoltage;
	ls.cds.Solar = ls.cds.fSolar * 10;

	 CalcTemp();

	dv = ls.cds.Cds_Avg[1] * 1.0;
	fVoltage = ((3.3 * dv)/4096.0) * ((1.0+1.0)/1.0);
	ls.cds.fTemp = fVoltage;
	ls.cds.Temp = ls.cds.fTemp * 10;
	printf("[Logic] Battery Voltage [BAT : %d v][SOL : %d v][TEMP : %d ℃]\r\n", ls.cds.Battery, ls.cds.Solar, ls.cds.Temp);*/
}

/*void CalcTemp()
{
	uint8_t tmp = 0;
	uint8_t minus = pdFALSE;
	uint16_t decimal_max;
	uint16_t decimal_min;
	uint16_t decimal_gap;
	double d_decimal_value;
	uint16_t decimal_value;

	if(ls.cds.Cds_Avg[1] > TEMP_MINUS_30)
	{
		minus = pdTRUE;
		ls.cds.Temp = 300;		//temp * 10(-30.0)
	}
	else if(ls.cds.Cds_Avg[1] < TEMP_99)
	{
		minus = pdTRUE;
		ls.cds.Temp = 990;		//temp * 10(99.0 )
	}
	else
	{
		if (ls.cds.Cds_Avg[1] > TEMP_ZERO) {
				tmp = 0;
				minus = pdTRUE;
				for (int i = 0; i < 130; i++) {
					if (ls.cds.Cds_Avg[1] >= temp_table[tmp]) {
						ls.cds.Temp = (30 - tmp) * 10;
						decimal_max = temp_table[tmp - 1];
						decimal_min = temp_table[tmp];
						decimal_gap = decimal_max - decimal_min;
						d_decimal_value = (ls.cds.Cds_Avg[1] - decimal_min) / decimal_gap;
						decimal_value = d_decimal_value * 10;

						printf("[Logic] CalcTemp [%d ~ %d][gap : %d][deciaml : %d]\r\n", decimal_max, decimal_min, decimal_gap, decimal_value);

						ls.cds.Temp += decimal_value;
						break;
					}

					tmp++;
				}
			} else {
				for (int i = 1; i < 130; i++) {
					if (ls.cds.Cds_Avg[1] >= temp_table[i]) {
						ls.cds.Temp = (i - 30) * 10;
						decimal_max = temp_table[i - 1];
						decimal_min = temp_table[i];
						decimal_gap = decimal_max - decimal_min;
						decimal_value = (((decimal_max * 10) - (ls.cds.Cds_Avg[1] * 10)) / decimal_gap);

						printf("[Logic] CalcTemp [%d ~ %d][gap : %d][deciaml : %d]\r\n", decimal_max, decimal_min, decimal_gap, decimal_value);

						ls.cds.Temp -= decimal_value;
						break;
					}
					tmp++;
				}
			//}
	}
}*/
void logicIdleTask()
{
	ls.tick.tickNow = xTaskGetTickCount();
    HAL_IWDG_Refresh(&hiwdg);
}

void logicProcTask()
{
	if(ls.stat.isTestMode)
		DisplayTestMode();
	else
		//DisplayWorkingMode();

	DisplayTestWhiteMode();

	/*if (ls.stat.isRegisterCall) {
		//if (CalcTickDiff(ls.tick.Send, ls.tick.tickNow) > (ls.modem.DeviceID * 200)) {
			ls.stat.isRegisterCall = pdFALSE;

			Send_Register();
		//}
	}

	if (ls.stat.isStatusCall) {
		//if (CalcTickDiff(ls.tick.Send, ls.tick.tickNow) > (ls.modem.DeviceID * 200)) {
			ls.stat.isStatusCall = pdFALSE;

			Send_Status();
		//}
	}

	if (ls.stat.isBLEADVCall) {
		ls.stat.isBLEADVCall = pdFALSE;

		if(ls.stat.preADVStat != ls.wdis.isBLEOnOff)
		{
			ls.stat.preADVStat = ls.wdis.isBLEOnOff;
			Send_BLE_check();
			Send_BLE_ADV_CTL(ls.wdis.isBLEOnOff);
			Send_BLE_check();
		}
	}

	if(ls.stat.isBLEADVUpdate){
		ls.stat.isBLEADVUpdate = pdFALSE;

		//Send_BLE_check();
		Send_BLE_ADVDATA_Situation(ls.modem.SectionID, ls.wdis.situation, ls.stat.EventDistanceGap);
	}*/
}

char command_str[128];
TickType_t master_tick;
TickType_t slave_tick;
void logicUartTask()
{
	//if(ls.stat.isCommInit == pdFALSE)
	//	return;

	Modem_Receive_Check();
	BLE_Receive_Check(ls.ble.isMode);


	uint8_t device_id = 0;

	osEvent event = osMessageGet(uartQueueHandle, 0);
	while (event.status == osEventMessage) {

		COMMAND_T Packet;

		memcpy(&Packet, (COMMAND_T*)event.value.v, sizeof(COMMAND_T));
		switch (Packet.opcode)		//Opcode
		{
			case Senser_Connect_REQ:
				ls.Slave_sensor.Connect = pdTRUE;
				printf("[logic] Slave Connect...! \r\n");
				Send_Connect_Ack();
				break;
			case Senser_Connect_ACK:
				printf("[logic] Master Connect...! \r\n");
				ls.Master_sensor.Connect = pdTRUE;
				Send_Time_Sync_RQ();
				break;
			case Time_REQ:
				printf("[logic] Slave Tick Time Request...! \r\n");
				Send_Time_Sync_ACK();
				ls.Slave_sensor.Connect = pdTRUE;
				break;
			case Time_ACk:
				 master_tick = (Packet.data[0] << 24) | (Packet.data[1] << 16) | (Packet.data[2] << 8) | (Packet.data[3]);
				 slave_tick = xTaskGetTickCount();
				ls.magnetic.Recv_time_offset = (int32_t)master_tick - (int32_t)slave_tick;
				printf("[logic] Slave Tick Time Recv master[%d] slave[%d] tick_offset[%d] \r\n",master_tick, slave_tick,ls.magnetic.Recv_time_offset);
				ls.Master_sensor.Connect = pdTRUE;
				break;
			case Magnetic_Time_Send:
				ls.magnetic.Recv_traffic_time = (Packet.data[0] << 24) | (Packet.data[1] << 16) | (Packet.data[2] << 8) | (Packet.data[3]);
				printf("[logic] Slave Magnetic Time Recv [%d] \r\n",ls.magnetic.Recv_traffic_time);
				ls.Slave_sensor.Connect = pdTRUE;
				break;
			case Magnetic_Time_Recv:
				ls.Master_sensor.Connect = pdTRUE;
				break;

	    	default:
	    		break;
		}
/*
		printf("\r\n[Logic] Modem OP_CODE [%2X]\r\n", Packet.opcode);

		switch (Packet.opcode)		//Opcode
		{
		   	case RF_CMD_SEND:
	    	case RF_CMD_RECV:			//Recv Data Packet
	    		device_id = Packet.data[1];
	    		Packet.subcode = Packet.data[2];
	    		Modem_Packet_Parse(Packet.subcode, device_id, &Packet.data[3]);
	    		break;

	    	default:
	    		break;
		}*/
		event = osMessageGet(uartQueueHandle, 0);
	}

/*
	osEvent uartevent = osMessageGet(bleQueueHandle, 0);
		while (uartevent.status == osEventMessage) {

			COMMAND_T Packet;

			memcpy(&Packet, (COMMAND_T*)uartevent.value.v, sizeof(COMMAND_T));

			printf("\r\n[Logic] Firmware OP_CODE [%2X]\r\n", Packet.opcode);

			switch (Packet.opcode)		//Opcode
			{
			   	case FIRMWARE_CMD_REG_REQ:		//모뎀정보 요청
			   		Send_Modem_Status();
			    	break;

			   	case FIRMWARE_CMD_REG_SET:		//C-Resigter 설정
			   		Save_Params(&Packet.data[0], 9);
			   		ls.stat.isRegister = pdTRUE;
		    		break;

			   	case FIRMWARE_BLE_NAME_REQ :
			   		Send_BLE_MANUF_Status_REQ();
			   		break;

			   	case FIRMWARE_BLE_NAME_SET:
			   		memcpy(&ls.ble.name,  &Packet.data[0], 8);
			   		printf("[LOGIC] ============= BLE SETTING ============\r\n");
			   		printf("[LOGIC]  BLE NAME        : [ %s ]\r\n", ls.ble.name);
			   		printf("[LOGIC] =======================================\r\n");
			   		Send_BLE_MANUF_SET(ls.ble.name);
			   		break;

			   	case FIRMWARE_BLE_ADVDATA_REQ :
			   		Send_BLE_ADVDATA_Status_REQ();
			   		break;

			   	case FIRMWARE_BLE_ADVDATA_SET:
			   		memcpy(&ls.ble.data,  &Packet.data[0], 13);
			   		printf("[LOGIC] ============= BLE SETTING ============\r\n");
			   		printf("[LOGIC] BLE DATA        : [ %s ]\r\n", ls.ble.data);
			   		printf("[LOGIC] =======================================\r\n");
			   		Send_BLE_ADVDATA_SET();
			   		break;

		    	default:
		    		break;
			}
			uartevent = osMessageGet(bleQueueHandle, 0);
		}
*/
}

void Modem_Packet_Parse(uint8_t opcode, uint8_t id, uint8_t* buff)
{
	printf("[LOGIC] Modem Packet_Parse[%02x][%02x][%02x]\r\n", opcode, id, buff[0]);
	uint8_t temp_id = 0;

	switch (opcode)		//Opcode
	{
		case DRM_CMD_REG_REQ:			//DRM Register Requent
			//Send_Register();
			if(id != ls.modem.DeviceID)
				return;
			//ls.tick.Send = xTaskGetTickCount();
			ls.stat.isRegisterCall = pdTRUE;
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

			ls.tick.Send = xTaskGetTickCount();

			ls.wdis.Pos = 1;
			ls.wdis.FlashCount = 0;
			ls.stat.isGroupControl = pdTRUE;
			//ls.stat.isBLEADVCall = pdTRUE;
			printf("[LOGIC] DRM Group Control[BRT : %02x][RGB : %02x][Opt : %02x]\r\n",
					buff[0], buff[1], buff[2]);

			break;

		case DRM_CMD_ONE_CTL:			//One Control
			if(id != ls.modem.DeviceID)
				return;
			else
			{
				Set_DRM(&buff[0]);

				ls.wdis.Brt = buff[0];
				ls.wdis.Disp_Led.red = (buff[1] & 0xC0) >> 6;
				ls.wdis.Disp_Led.grn = (buff[1] & 0x30) >> 4;
				ls.wdis.Disp_Led.blu = (buff[1] & 0x0C) >> 2;
				ls.wdis.isIR =  (buff[1] & 0x02) >> 1;
				ls.wdis.isBLEOnOff =  (buff[1] & 0x01);

				ls.wdis.isIRFlash = buff[2] & 0x80 ? pdTRUE : pdFALSE;
				ls.wdis.isFlash = buff[2] & 0x40 ? pdTRUE : pdFALSE;
				ls.wdis.FlashMode = buff[2] & 0x20 ? pdTRUE : pdFALSE;
				ls.wdis.Flashing_InterVal = buff[2] & 0x1F;

				ls.wdis.Pos = 1;
				ls.wdis.FlashCount = 0;

				ls.stat.isGroupControl = pdFALSE;
				//ls.stat.isBLEADVCall = pdTRUE;
				printf( "[LOGIC ] OneControl [BRT : %02x][RGB : %02x][Opt : %02x]\r\n", buff[0], buff[1], buff[2]);

				Send_Status();
			}
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
									if(ls.wdis.situation != id || ls.stat.EventDistanceGap != ls.stat.NewEventDistanceGap)
									{
										printf("[LOGIC ] BLE AdvData Set\r\n");
										ls.stat.EventDistanceGap = ls.stat.NewEventDistanceGap;
										ls.wdis.situation = id;
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
			//Send_Status();
			if(id != ls.modem.DeviceID)
				return;

			ls.tick.Send = xTaskGetTickCount();
			ls.stat.isStatusCall = pdTRUE;
			break;

		case DRM_CMD_RESET:				//Reset
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
			if(id == ls.modem.DeviceID || id == 0xFF)
			{
				ls.wdis.isBLEOnOff = buff[0];
				ls.stat.isBLEADVCall = pdTRUE;
			}
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

	ls.tick.Send = xTaskGetTickCount();
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
	send_buf[bp++] = 0;		//STX
	send_buf[bp++] = 9;		//Length~LRC Size

	send_buf[bp++] = ls.modem.Status_Control;
	send_buf[bp++] = ls.modem.SectionID;
	send_buf[bp++] = ls.modem.DeviceID;
	send_buf[bp++] = ls.modem.Rf_ch;
	send_buf[bp++] = ls.modem.Tx_power;
	send_buf[bp++] = ls.modem.Tx_rssi;
	send_buf[bp++] = ls.modem.Dbl_send;
	send_buf[bp++] = ls.modem.Led_Timeout;
	send_buf[bp++] = ls.modem.Lane;

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
	send_buf[bp++] = 14;			//Length~LRC Size
	send_buf[bp++] = RF_CMD_RECV;			//OP-Code(Display Data)
	send_buf[bp++] = ls.modem.DeviceID;
	send_buf[bp++] = 0;
	send_buf[bp++] = 9;
	send_buf[bp++] = ls.modem.DeviceID;
	send_buf[bp++] = DRM_CMD_ST_RSP;
	send_buf[bp++] = ls.wdis.Brt;
	send_buf[bp++] = ls.wdis.Disp_Led.red;
	send_buf[bp++] = ls.wdis.Disp_Led.grn;
	send_buf[bp++] = ls.wdis.Disp_Led.blu;

	temp_data |= ls.wdis.isIR ? 0x80 : 0x00;
	temp_data |= ls.wdis.isFlash ? 0x40 : 0x00;
	temp_data |= ls.wdis.Flashing_InterVal;

	send_buf[bp++] = temp_data;
	send_buf[bp++] = ls.cds.Battery;

	send_buf[bp++] = CalcLRC(&send_buf, 1, bp - 1); 	//CRC;
	send_buf[bp++] = 0x7F;

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
	send_buf[bp++] = 9;			//Length~LRC Size
	send_buf[bp++] = RF_CMD_RECV;			//OP-Code(Display Data)
	send_buf[bp++] = ls.modem.DeviceID;
	send_buf[bp++] = 0;
	send_buf[bp++] = 4;
	send_buf[bp++] = ls.modem.DeviceID;
	send_buf[bp++] = DRM_CMD_REG_RSP;
	send_buf[bp++] = ls.modem.DeviceID;

	printf("[LOGIC] Modem Send_Register[%d]\r\n", ls.modem.DeviceID);

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
	if (CalcTickDiff(ls.tdis.Tick, ls.tick.tickNow) <= ls.tdis.Delay) {
		return;
	}

	ls.tdis.Tick = ls.tick.tickNow;

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
	if (CalcTickDiff(ls.tdis.Tick, ls.tick.tickNow) <= TEST_DELAY_LONG) {
		return;
	}

	ls.tdis.Tick = ls.tick.tickNow;

    memset(&ls.tdis.Disp_Led, 0x00, sizeof(LEDDATA_T));

	ls.tdis.Disp_Led.red = 0xFF;
	ls.tdis.Disp_Led.grn = 0xFF;
	ls.tdis.Disp_Led.blu = 0xFF;
	ls.tdis.Disp_Led.ir = 0xFF;

    dispSetBright(TEST_BRIGHT_MAX);

    dispSetData(&ls.tdis.Disp_Led);
    dispRefresh();
}

static void DisplayWorkingMode()
{

	const TickType_t tickNow = xTaskGetTickCount();
	if (CalcTickDiff(ls.wdis.Tick, tickNow) <= 10) {
		return;
	}

	ls.wdis.Tick = tickNow;

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
	printf("[LOGIC] Send_BLE_check \r\n");
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT\r");

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
}


void Send_BLE_Info_REQ()
{
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT+INFO?\r");

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
   printf("[LOGIC] Send_BLE_cmd : %s \n", ls.ble.command_str);
}

void Send_BLE_PacketMode_REQ()
{
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT+VER?\r");

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
   printf("[LOGIC] Send_BLE_cmd : %s \n", ls.ble.command_str);
}

void Send_BLE_ADVDATA_Status_REQ()
{
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT+ADVDATA?\r");

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
   printf("[LOGIC] Send_BLE_cmd : %s", ls.ble.command_str);

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
   printf("[LOGIC] Send_BLE_cmd : %s", ls.ble.command_str);

   //Send_BLE_ADVDATA_Status_RSP();
}

void Send_BLE_ADVDATA_Status_RSP()
{
	uint8_t bp = 0;
	printf("[LOGIC] command_str : %s", ls.ble.command_str);

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
	printf("[LOGIC] Send_BLE_ADVDATA_Situation\r\n");
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   send_buf[bp++] = section;
   send_buf[bp++] = situ;
   send_buf[bp++] = loca/1000;
   send_buf[bp++] = (loca/100)%10;
   send_buf[bp++] = (loca/10)%10;
   send_buf[bp++] = loca%10;

   sprintf(ls.ble.command_str, "AT+ADVDATA=%d%d%04d\r", section, situ, loca);

   //printf("[LOGIC] Send_BLE_cmd : AT+ADVDATA=%s\n", ls.ble.command_str);

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
}

void Send_BLE_ADVDATA_SET()
{
	printf("[LOGIC] BLE_ADVDATA_SET\r\n");
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
   printf("[LOGIC] Send_BLE_cmd : %s", ls.ble.command_str);
}

void Send_BLE_MANUF_SET()
{
	printf("[LOGIC] BLE_MANUF_SET\r\n");
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT+MANUF=DRM\r");

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
   printf("[LOGIC] Send_BLE_cmd : %s", ls.ble.command_str);
}

void Send_BLE_INTERVAL_SET(uint16_t interval)
{
	printf("[LOGIC] Send_BLE_INTERVAL_SET %d\r\n", interval);
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT+ADVINTERVAL=%d\r", interval);

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
   printf("[LOGIC] Send_BLE_cmd : %s", ls.ble.command_str);
}

void Send_BLE_UART_SET(uint32_t baud)
{
   memset(ls.ble.command_str, 0x00, sizeof(ls.ble.command_str));

   sprintf(ls.ble.command_str, "AT+UART=%d\r", baud);
   printf("[LOGIC] Send_BLE_cmd : %s \n", ls.ble.command_str);

   UART_StrOut(BLE_UART_PORT, ls.ble.command_str);
   printf("[LOGIC] Send_BLE_cmd : %s", ls.ble.command_str);
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

		stat = HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &ls.FRAM_SaveBuff, 2+Size, 1000);
		HAL_Delay(20);

		if(stat != HAL_OK)
			printf("[logic.c] Save Page1 Transmit Result [%d]\r\n", stat);
	} else {
		if ((address + Size) > 0xFFFF)	//Page 0 + 1
		{
			SplitSize1 = 0xFFFF - address;
			SubAddress1 = address;
			DevAddress = 0xA0;
			ls.FRAM_SaveBuff[0] = (uint8_t) (SubAddress1 >> 8) & 0x00FF;
			ls.FRAM_SaveBuff[1] = (uint8_t) (SubAddress1 >> 0) & 0x00FF;
			memcpy(&ls.FRAM_SaveBuff[2], pData, SplitSize1);
			stat = HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &ls.FRAM_SaveBuff[0], SplitSize1 + 2, 1000);
			HAL_Delay(20);

			if(stat != HAL_OK)
				printf("[logic.c] Save Page0+1 [0] Transmit Result [%d]\r\n", stat);
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
				printf("[logic.c] Save Page0+1 [1] Transmit Result [%d]\r\n", stat);
		} else		//Page 0
		{
			DevAddress = 0xA0;
			SubAddress1 = address;
			ls.FRAM_SaveBuff[0] = (uint8_t) (SubAddress1 >> 8) & 0x00FF;
			ls.FRAM_SaveBuff[1] = (uint8_t) (SubAddress1 >> 0) & 0x00FF;
			memcpy(&ls.FRAM_SaveBuff[2], pData, Size);

			stat = HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &ls.FRAM_SaveBuff[0], Size + 2, 1000);
			HAL_Delay(20);

			if(stat != HAL_OK)
				printf("[logic.c] Save Page0 Transmit Result [%d]\r\n", stat);
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

		stat = HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &fram_buf[0], 2, 100);
		HAL_Delay(20);
		if(stat != HAL_OK)
			printf("[logic.c] Page1 Transmit Result [%d]\r\n", stat);

		stat = HAL_ERROR;
		stat = HAL_I2C_Master_Receive(&hi2c1, DevAddress, pData, Size, 1000);
		if(stat != HAL_OK)
			printf("[logic.c] Page1 Receive Result [%d]\r\n", stat);

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
				printf("[logic.c] Page 0 - 1[1] Transmit Result [%d]\r\n", stat);

			stat = HAL_ERROR;

			stat = HAL_I2C_Master_Receive(&hi2c1, DevAddress, pData, SplitSize1, 1000);
			HAL_Delay(20);
			if (stat != HAL_OK)
				printf("[logic.c] Page 0 - 1[1] Receive Result [%d]\r\n", stat);

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
				printf("[logic.c] Page 0 - 1[2] Transmit Result [%d]\r\n", stat);

			stat = HAL_ERROR;

			stat = HAL_I2C_Master_Receive(&hi2c1, DevAddress, pData, SplitSize2, 1000);
			HAL_Delay(20);
			if (stat != HAL_OK)
				printf("[logic.c] Page 0 - 1[2] Receive Result [%d]\r\n", stat);

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
				printf("[logic.c] Page 0 Transmit Result [%d]\r\n",
						stat);

			stat = HAL_ERROR;
			stat = HAL_I2C_Master_Receive(&hi2c1, DevAddress, pData, Size, 1000);
			HAL_Delay(20);

			if (stat != HAL_OK)
				printf("[logic.c] Page 0 Receive Result [%d]\r\n", stat);

			stat = HAL_ERROR;
		}
	}
}

static HAL_StatusTypeDef lis2mdl_wait_ready(I2C_HandleTypeDef* h, uint32_t ms)
{
    uint32_t t0 = HAL_GetTick();
    while (HAL_GetTick() - t0 < ms) {
        if (HAL_I2C_IsDeviceReady(h, LIS2MDL_ADDR<<1, 1, 2) == HAL_OK) return HAL_OK;
        HAL_Delay(1);
    }
    return HAL_TIMEOUT;
}

/* We reuse your state machine symbols:
 * WAIT_FALL = idle
 * WAIT_RISE = in-event
 * and fields inside ls.magnetic used in your code.
 */

/* -------------------------
 * Utility: |B| computation
 * ------------------------- */
static inline int16_t calc_bmag_i16(int16_t x, int16_t y, int16_t z) {
    float fx = (float)x, fy = (float)y, fz = (float)z;
    float m  = sqrtf(fx*fx + fy*fy + fz*fz);
    return (int16_t)lrintf(m);
}

/* ===================================================
 * Init: SOFT_RST, 10 Hz continuous, BDU+DRDY_on_pin,
 *       then calibrate baseline and sigma on |B|
 * =================================================== */
void Magnetic_Sensor_Init(void)
{
	   uint8_t who = 0, reg = 0, status = 0;

	    printf("Magnetic_Sensor_Init ...\r\n");

	    // 1) 전원 안정 대기 + 슬레이브 준비 폴링 (고정딜레이 대신)
	    if (lis2mdl_wait_ready(&hi2c1, 20) != HAL_OK) {   // 20ms 안에 응답 기대
	        I2C_Dump("IsDeviceReady", &hi2c1);
	        I2C_Scan(&hi2c1);
	        ls.magnetic.isconnect = pdFALSE;
	        return;
	    }

	    // 2) WHO_AM_I
	    if (HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, WHO_AM_I, I2C_MEMADD_SIZE_8BIT,
	                         &who, 1, 50) != HAL_OK || who != 0x40) {
	        printf("[ERROR] WHO=0x%02X\r\n", who);
	        ls.magnetic.isconnect = pdFALSE;
	        return;
	    }
	    printf("[OK] WHO=0x%02X\r\n", who);

	    // 3) SOFT_RST → 비트 클리어될 때까지 폴링 (딜레이 불필요)
	    uint8_t rst = 0x20;
	    HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_ADDR<<1, CFG_REG_A, I2C_MEMADD_SIZE_8BIT, &rst, 1, 50);
	    uint32_t t0 = HAL_GetTick();
	    do {
	        HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, CFG_REG_A, I2C_MEMADD_SIZE_8BIT, &reg, 1, 50);
	        if ((HAL_GetTick() - t0) > 100) break;   // 100ms 가드
	    } while (reg & 0x20);

	    // 4) 모드 설정 (TEMP_EN+10Hz+Continuous)
	    uint8_t cfg_a = 0x80;
	    //uint8_t cfg_a = 0x84; // 20hz
	    HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_ADDR<<1, CFG_REG_A, I2C_MEMADD_SIZE_8BIT, &cfg_a, 1, 50);

	    // 5) BDU=1, DRDY_on_pin=1
	    uint8_t cfg_c = 0x11;
	    //cfg_c |= (1u<<4) | (1u<<7);
	    HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_ADDR<<1, CFG_REG_C, I2C_MEMADD_SIZE_8BIT, &cfg_c, 1, 50);

	    // 6) 첫 샘플 준비될 때까지 STATUS ZYXDA(=bit3) 폴링 (최대 150ms 정도)
	    t0 = HAL_GetTick();
	    do {
	        HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, STATUS_REG, I2C_MEMADD_SIZE_8BIT, &status, 1, 50);
	        if (status & 0x08) break;                 // ZYXDA=1(데이터 준비)
	        if ((HAL_GetTick() - t0) > 150) break;    // 가드
	        HAL_Delay(5);
	    } while (1);

	    // 7) 더미 1회 버스트 읽기(오버런/플래그 정리)
	    uint8_t de[6];
	    HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, OUTX_L_REG, I2C_MEMADD_SIZE_8BIT, de, 6, 50);

	    // 8) 리드백 로그
	    uint8_t cfg_regs[3] = {0};
	    HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, CFG_REG_A, I2C_MEMADD_SIZE_8BIT, cfg_regs, 3, 50);
	    HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, STATUS_REG, I2C_MEMADD_SIZE_8BIT, &status, 1, 50);
	    printf("[CFG] A=0x%02X B=0x%02X C=0x%02X  [ST]=0x%02X\r\n",
	           cfg_regs[0], cfg_regs[1], cfg_regs[2], status);

	    ls.magnetic.isconnect = pdTRUE;

    //Calibration on |B|
    int16_t mbuf[CALIB_SAMPLES];
    int count = 0;
    uint8_t rb[6] = {0};
    uint32_t t0_all = HAL_GetTick();

    while (count < CALIB_SAMPLES) {
        HAL_IWDG_Refresh(&hiwdg);
        if (HAL_GPIO_ReadPin(ADC_TEMP_GPIO_Port, ADC_TEMP_Pin) == GPIO_PIN_SET) {
            HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, OUTX_L_REG, 1, rb, 6, HAL_MAX_DELAY);
            int16_t x = (int16_t)(rb[1]<<8 | rb[0]);
            int16_t y = (int16_t)(rb[3]<<8 | rb[2]);
            int16_t z = (int16_t)(rb[5]<<8 | rb[4]);
            mbuf[count++] = calc_bmag_i16(x, y, z);

             //wait for DRDY to go low (max ~50 ms)
            uint32_t t0 = HAL_GetTick();
            while (HAL_GPIO_ReadPin(ADC_TEMP_GPIO_Port, ADC_TEMP_Pin) == GPIO_PIN_SET) {
                HAL_IWDG_Refresh(&hiwdg);
                if (HAL_GetTick() - t0 > 50) break;
                HAL_Delay(1);
            }
        } else {
            HAL_Delay(1);
        }
        if (HAL_GetTick() - t0_all > 5000) break;  /* safety timeout */
    }

    MagProc_Calibrate_from_buf(mbuf, count > 0 ? count : 1);
}



void I2C_Dump(const char* tag, I2C_HandleTypeDef* h)
{
    printf("[%s] ERR=0x%08lX STATE=0x%02X\r\n",
           tag, (unsigned long)h->ErrorCode, h->State);
    if (h->ErrorCode & HAL_I2C_ERROR_ARLO) printf(" - ARLO (arbitration lost)\r\n");
    if (h->ErrorCode & HAL_I2C_ERROR_AF)   printf(" - NACK (AF)\r\n");
    if (h->ErrorCode & HAL_I2C_ERROR_BERR) printf(" - Bus error\r\n");
    if (h->ErrorCode & HAL_I2C_ERROR_TIMEOUT) printf(" - Timeout\r\n");
}

void I2C_Scan(I2C_HandleTypeDef* h)
{
    printf("I2C scan...\r\n");
    for (uint8_t a7=0x08; a7<=0x77; a7++){
        uint16_t a8 = a7<<1;
        if (HAL_I2C_IsDeviceReady(h, a8, 2, 5)==HAL_OK)
            printf(" - 0x%02X (7b)\r\n", a7);
    }
}

/* ===================================================
 * Calibration: mean, sigma, threshold on |B|
 * Note: we store |B| baseline in ls.magnetic.Z0 for
 * compatibility with your existing fields.
 * =================================================== */
void MagProc_Calibrate_from_buf(const int16_t* buf, int n)
{
    int64_t sum = 0;
    for (int i = 0; i < n; i++) sum += buf[i];
    float mean = (float)sum / (float)n;

    double s2 = 0.0;
    for (int i = 0; i < n; i++) {
        double d = (double)buf[i] - (double)mean;
        s2 += d * d;
    }
    float sigma = sqrtf((float)(s2 / (double)n));

    ls.magnetic.Z0    = (int32_t)llroundf(mean);   /* baseline of |B| */
    ls.magnetic.sigma = sigma;

    int16_t T = (int16_t)lrintf(fmaxf((float)T_ABS_MIN, K_SIGMA * sigma));
    if (T < T_ABS_MIN) T = T_ABS_MIN;
    ls.magnetic.T = T;

    ls.magnetic.st = WAIT_FALL; /* idle */
    ls.magnetic.debounce_until   = 0;
    ls.magnetic.refractory_until = 0;
    ls.magnetic.traffic_count    = 0;
    ls.magnetic.Speed            = 0;

    printf("[MAG] Calib |B|: M0=%ld, sigma=%.2f, T=%d\r\n", (long)ls.magnetic.Z0, ls.magnetic.sigma, ls.magnetic.T);
}

void Magnetic_DRDY()
{
	uint8_t reg_val;
	uint8_t crt_regA_state[3];
	memset(&crt_regA_state, 0x00, sizeof(crt_regA_state));

	    // 1. 현재 CFG_REG_C (0x62) 읽기
	    HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR << 1, CFG_REG_C, I2C_MEMADD_SIZE_8BIT, &reg_val, 1, HAL_MAX_DELAY);

	    // 2. DRDY_ON_PIN (bit 7)이 0이면 설정
	    if (!(reg_val & 0x01))
	    {
	        reg_val |= 0x01;  // bit 7 = 1 설정
	        HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_ADDR << 1, CFG_REG_C, I2C_MEMADD_SIZE_8BIT, &reg_val, 1, HAL_MAX_DELAY);
	    }

	    // (선택) printf 또는 로그로 결과 출력
	    if (reg_val & 0x01)
	        printf("DRDY pin output is ENABLED.\r\n");
	    else
	        printf("DRDY pin output is DISABLED.\r\n");

		// Read CFG REG Set Status
		HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, CFG_REG_A, 1, crt_regA_state, 3, HAL_MAX_DELAY);
	    printf("[logic] REG_A: 0x%02X, REG_B: 0x%02X, REG_C: 0x%02X\r\n",
	    		crt_regA_state[0], crt_regA_state[1], crt_regA_state[2]);

}

/* ===================================================
 * Optional: status-check print that also shows |B|
 * =================================================== */
void Magnetic_Sensor_Read_stsus_check(void)
{
    static int16_t prevX=0, prevY=0, prevZ=0;
    static TickType_t prevTick=0;
    const TickType_t tickNow = xTaskGetTickCount();

    uint8_t status = 0;
    uint8_t rb[6] = {0};

    if (!ls.magnetic.isconnect) { printf("[logic] Magnetic Sensor Disconnect....!!\r\n"); return; }

    HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, STATUS_REG, 1, &status, 1, HAL_MAX_DELAY);
    if (status & 0x08) { /* ZYXDA set */
        HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, OUTX_L_REG, 1, rb, 6, HAL_MAX_DELAY);
        ls.magnetic.mag_x = (int16_t)(rb[1]<<8 | rb[0]);
        ls.magnetic.mag_y = (int16_t)(rb[3]<<8 | rb[2]);
        ls.magnetic.mag_z = (int16_t)(rb[5]<<8 | rb[4]);

        if (ls.magnetic.mag_x!=prevX || ls.magnetic.mag_y!=prevY || ls.magnetic.mag_z!=prevZ) {
            TickType_t interval = tickNow - prevTick; prevTick = tickNow;
            prevX = ls.magnetic.mag_x; prevY = ls.magnetic.mag_y; prevZ = ls.magnetic.mag_z;

            int16_t bmag = calc_bmag_i16(prevX, prevY, prevZ);
            float heading = atan2f((float)prevY, (float)prevX) * (180.0f / 3.14159265f);
            if (heading < 0) heading += 360.0f;

            printf("[logic] dT=%lu ms, X=%d Y=%d Z=%d |B|=%d, heading=%.1f\r\n",  interval, prevX, prevY, prevZ, bmag, heading);
            //Send_Magnetic_data();
        }
    }
}

/* ===================================================
 * Read sample and feed FSM with |B|
 * =================================================== */
void Magnetic_Sensor_Read(void)
{
    uint8_t rb[6] = {0};

    if (!ls.magnetic.isconnect) {
        printf("[logic] Magnetic Sensor Disconnect ....!!\r\n");
        return;
    }

    HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, OUTX_L_REG, 1, rb, 6, HAL_MAX_DELAY);

    ls.magnetic.mag_x = (int16_t)(rb[1] << 8 | rb[0]);
    ls.magnetic.mag_y = (int16_t)(rb[3] << 8 | rb[2]);
    ls.magnetic.mag_z = (int16_t)(rb[5] << 8 | rb[4]);

    //Traffic_Cal_Min(ls.magnetic.mag_z);

    int16_t bmag = calc_bmag_i16(ls.magnetic.mag_x, ls.magnetic.mag_y, ls.magnetic.mag_z);
    //Magnetic_Sensor_Cal();
    /* do not use Z-only path anymore */
    MagProc_Process_sample(bmag, ls.magnetic.Recv_traffic_time /* or 0 */);
    //Send_Magnetic_data();
}

void Magnetic_Sensor_Cal()
{

	static TickType_t prevTick = 0;
	const TickType_t tickNow = HAL_GetTick();
	TickType_t interval = tickNow - prevTick; //delay 측정
	prevTick = tickNow;

/*	float heading = atan2((float)ls.magnetic.mag_y, (float)ls.magnetic.mag_x) * (180.0f / 3.14159265f);

	if (heading < 0)
	{
		heading += 360.0f;
	}*/

	//printf("[logic] ΔT = %lu ms, x : %d, y : %d, z : %d, \r\n",interval, ls.magnetic.mag_x, ls.magnetic.mag_y, ls.magnetic.mag_z);
	printf("[logic] ΔT = %lu ms\r\n",interval);
}

int16_t prev_z=220;

void Traffic_Cal_Min(int16_t rec_z)
{
	const TickType_t tickNow = xTaskGetTickCount();
	int16_t diff_z = rec_z - prev_z;
	if(!ls.magnetic.z_change)
	{
		if(diff_z < -100)
		{
			ls.magnetic.z_change = pdTRUE;
			//printf("[logic] Z low check Rcev z : %d, prev z : %d, diff z : %d\r\n",rec_z, prev_z, diff_z);
			prev_z = rec_z;

		}
		else
			prev_z = rec_z;
	}
	else
	{
		if(diff_z > 100)
		{
			ls.magnetic.z_change = pdFALSE;
			//printf("[logic] Z High check Rcev z : %d, prev z : %d, diff z : %d count : %d \r\n",rec_z, prev_z, diff_z,ls.magnetic.traffic_count );
			prev_z = rec_z;
			ls.magnetic.traffic_count++;

			if(ls.stat.isTestMode)
			{
				if(ls.magnetic.Recv_traffic_time != 0)
				{
				// 차량 속도 계산
				TickType_t vehicle_exit_tick = tickNow;
				TickType_t duration_ticks = vehicle_exit_tick - ls.magnetic.Recv_traffic_time;
				float time_sec = duration_ticks * portTICK_PERIOD_MS / 1000.0f;
				printf("[logic] master Tick [%d] Slave Tick [%d] Duration Tick [%d] \r\n",vehicle_exit_tick, ls.magnetic.Recv_traffic_time, duration_ticks);
				float speed_mps = Sensor_Distance / time_sec;
				float speed_kph = speed_mps * 3.6f;

				ls.magnetic.Speed = speed_kph;

				printf("[TESTMODE] Speed = %.2f km/h (%.2f sec, %.2f m)\r\n",speed_kph, time_sec, Sensor_Distance);
				}
				ls.magnetic.Recv_traffic_time = 0;
			}
			else
			{
				Send_Magnetic_Time();
				ls.Master_sensor.send_flag = pdFALSE;
			}

		}
	}

	if (CalcTickDiff(ls.tick.traffic_avg_sec, tickNow) >= 1000)
	{
		ls.tick.traffic_avg_sec = tickNow;
		for(int i = 60; i>0; i--)
		{
			ls.magnetic.traffic_array[i] = ls.magnetic.traffic_array[i-1];
		}

		ls.magnetic.traffic_array[0] = ls.magnetic.traffic_count;
		ls.magnetic.traffic_count = 0;

		uint32_t sum = 0;
		uint32_t cnt = 0;

		for(int i = 60; i>0; i--)
		{
			if(ls.magnetic.traffic_array[i] > 0)
			{
				sum+= ls.magnetic.traffic_array[i];
				cnt++;
			}
		}

		if(cnt > 0)
			ls.magnetic.traffic_avg_60sec = sum;
		else
			ls.magnetic.traffic_avg_60sec = 0;

		//printf("[logic] Magnetic count AVG [%d] \r\n", ls.magnetic.traffic_avg_60sec);
	}

	//Send_Magnetic_data();
}

/* ===================================================
 * One-lobe detector on |B|: IDLE <-> IN_EVENT
 * Uses absolute deviation from baseline.
 * =================================================== */
void MagProc_Process_sample(int16_t rec_mag, TickType_t peer_tick_or_0)
{
    const TickType_t t = xTaskGetTickCount();

    if (t < ls.magnetic.debounce_until || t < ls.magnetic.refractory_until)
        return;

    int16_t rel = (int16_t)(rec_mag - (int16_t)ls.magnetic.Z0);
    int16_t abs_rel = (rel >= 0) ? rel : (int16_t)(-rel);

    /* very slow baseline follow only near zero band while idle */
    if (ls.magnetic.st == WAIT_FALL && abs_rel < (ls.magnetic.T / IDLE_BAND_DIV)) {
        int32_t err = (int32_t)rec_mag - ls.magnetic.Z0;
        ls.magnetic.Z0 += (err * EMA_BETA_PERMILLE) / 1000;
    }

    switch (ls.magnetic.st) {
    case WAIT_FALL: /* idle */
        if (abs_rel >= ls.magnetic.T)
        {
            ls.magnetic.st = WAIT_RISE; /* in-event */
            ls.magnetic.debounce_until = t + pdMS_TO_TICKS(DEBOUNCE_MS);
        }
        break;

    case WAIT_RISE: /* in-event */
        if (abs_rel < ls.magnetic.T)
        {
            ls.magnetic.st = WAIT_FALL; /* back to idle */
            //ls.magnetic.traffic_count++;
            ls.magnetic.refractory_until = t + pdMS_TO_TICKS(REFRACT_MS);

            /* optional speed from peer time stamp */
            if (peer_tick_or_0 != 0 && Sensor_Distance > 0.0f)
            {
                TickType_t dt = t - peer_tick_or_0;  //wrap safe with TickType_t
                float sec = (float)dt * (portTICK_PERIOD_MS / 1000.0f);
                if (sec > 0.02f)
                {
                    ls.magnetic.Speed = (Sensor_Distance / sec) * 3.6f;
                }
                ls.magnetic.Recv_traffic_time = 0;
            }
            Send_Magnetic_Time();

        }
        break;
    }

    //Send_Magnetic_data();
}

/* ===================================================
 * 1 s traffic average (unchanged)
 * =================================================== */
void Magnetic_Avg_1s_Task(void)
{
    const TickType_t t = xTaskGetTickCount();
    if (CalcTickDiff(ls.tick.traffic_avg_sec, t) < 1000) return;
    ls.tick.traffic_avg_sec = t;

    for (int i = 60; i > 0; i--) {
        ls.magnetic.traffic_array[i] = ls.magnetic.traffic_array[i-1];
    }
    ls.magnetic.traffic_array[0] = ls.magnetic.traffic_count;
    ls.magnetic.traffic_count = 0;

    uint32_t sum = 0;
    for (int i = 60; i > 0; i--) sum += ls.magnetic.traffic_array[i];
    ls.magnetic.traffic_avg_60sec = sum;
    //printf("[Master] traffic [%d] speed [%d] \r\n",ls.magnetic.traffic_avg_60sec,ls.magnetic.Speed);
}

void logicMagneticTask()
{
	const TickType_t tickNow = xTaskGetTickCount();


	if(ls.Master_sensor.Connect)
	{
		if (HAL_GPIO_ReadPin(ADC_TEMP_GPIO_Port, ADC_TEMP_Pin) == GPIO_PIN_SET)
		{
			      Magnetic_Sensor_Read();
			      ls.Master_sensor.send_flag = pdTRUE;
		}

		if (!ls.Master_sensor.send_flag && CalcTickDiff(ls.tick.Sensor, tickNow) >= 60*1000)
		{
					//1시간 주기 마스터 타임 틱 요청
					ls.tick.Sensor = tickNow;
					Send_Time_Sync_RQ();
		}
	}
	else if(!ls.Master_sensor.Connect && CalcTickDiff(ls.tick.Sensor, tickNow) >= 3000)
	{
		ls.tick.Sensor = tickNow;
		Send_Connect_RQ();
	}

}

//uint8_t send_buf[64];

void Send_Connect_RQ()
{
	uint8_t bp = 0;
	uint16_t calc_lrc = 0;

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;							//STX
	send_buf[bp++] = 0x07;							//Length~LRC Size
	send_buf[bp++] = Senser_Connect_REQ;			//OP-Code

	send_buf[bp++] = 0x01;
	send_buf[bp++] = 0x02;
	send_buf[bp++] = 0x03;
	send_buf[bp++] = 0x04;

	send_buf[bp++] = CalcLRC(send_buf,1,bp-1);
	send_buf[bp++] = 0x7F;


	commSendData(MODEM_UART_PORT, send_buf, bp);
	printf("[logic]  Connect_RQ \r\n");
}

void Send_Connect_Ack()
{
	uint8_t bp = 0;
	uint16_t calc_lrc = 0;

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;							//STX
	send_buf[bp++] = 0x07;							//Length~LRC Size
	send_buf[bp++] = Senser_Connect_ACK;			//OP-Code

	send_buf[bp++] = 0x01;
	send_buf[bp++] = 0x02;
	send_buf[bp++] = 0x03;
	send_buf[bp++] = 0x04;

	send_buf[bp++] = CalcLRC(send_buf,1,bp-1);
	send_buf[bp++] = 0x7F;

	commSendData(MODEM_UART_PORT, send_buf, bp);

	printf("[logic]  Connect_ACK \r\n");
}
void Send_Time_Sync_RQ()
{
	uint8_t bp = 0;
	uint16_t calc_lrc = 0;
	TickType_t tick = xTaskGetTickCount();

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;			//STX
	send_buf[bp++] = 0x07;			//Length~LRC Size
	send_buf[bp++] = Time_REQ;			//OP-Code(Modem Send Opcode)

	send_buf[bp++] = (tick >> 24) & 0xFF;		//Controller time Start
	send_buf[bp++] = (tick >> 16) & 0xFF;
	send_buf[bp++] = (tick >> 8) & 0xFF;
	send_buf[bp++] = (tick) & 0xFF;			//Controller time end

	send_buf[bp++] = CalcLRC(send_buf,1,bp-1);
	send_buf[bp++] = 0x7F;

	commSendData(MODEM_UART_PORT, send_buf, bp);

	printf("[logic]  Magentic_Time_RQ \r\n");
}

void Send_Time_Sync_ACK(uint8_t *data)
{

	uint8_t bp = 0;
	uint16_t calc_lrc = 0;
	TickType_t tick = xTaskGetTickCount();

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;			//STX
	send_buf[bp++] = 0x07;			//Length~LRC Size
	send_buf[bp++] = Time_ACk;			//OP-Code(Modem Send Opcode)

	send_buf[bp++] = (tick >> 24) & 0xFF;		//Controller time Start
	send_buf[bp++] = (tick >> 16) & 0xFF;
	send_buf[bp++] = (tick >> 8) & 0xFF;
	send_buf[bp++] = (tick) & 0xFF;			//Controller time end

	send_buf[bp++] = CalcLRC(send_buf,1,bp-1);
	send_buf[bp++] = 0x7F;

	commSendData(MODEM_UART_PORT, send_buf, bp);

	printf("[logic]  Magentic_Time_Ack \r\n");
}

void Send_Magnetic_Time()
{
	//HAL_RTC_GetTime(&hrtc, &ls.gps.sTime, FORMAT_BIN);
	//HAL_RTC_GetDate(&hrtc, &ls.gps.sDate, FORMAT_BIN);

	uint8_t bp = 0;
	uint16_t calc_lrc = 0;
	TickType_t local_tick = xTaskGetTickCount();
	TickType_t aligned_tick = local_tick + ls.magnetic.Recv_time_offset;

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;			//STX
	send_buf[bp++] = 0x07;			//Length~LRC Size
	send_buf[bp++] = Magnetic_Time_Send;			//OP-Code(Modem Send Opcode)

	send_buf[bp++] = (aligned_tick >> 24) & 0xFF;		//Controller time Start
	send_buf[bp++] = (aligned_tick >> 16) & 0xFF;
	send_buf[bp++] = (aligned_tick >> 8) & 0xFF;
	send_buf[bp++] = (aligned_tick) & 0xFF;			//Controller time end

	send_buf[bp++] = CalcLRC(send_buf,1,bp-1);
	send_buf[bp++] = 0x7F;

	commSendData(MODEM_UART_PORT, send_buf, bp);

	printf("[logic]  Magentic_Time_Send \r\n");
}
void Send_Magnetic_Time_Recv(uint8_t *data)
{
	uint8_t bp = 0;
	uint16_t calc_lrc = 0;
	TickType_t tick = xTaskGetTickCount();

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;			//STX
	send_buf[bp++] = 0x07;			//Length~LRC Size
	send_buf[bp++] = Magnetic_Time_Recv;			//OP-Code(Modem Send Opcode)

	send_buf[bp++] = (tick >> 24) & 0xFF;		//Controller time Start
	send_buf[bp++] = (tick >> 16) & 0xFF;
	send_buf[bp++] = (tick >> 8) & 0xFF;
	send_buf[bp++] = (tick) & 0xFF;			//Controller time end

	send_buf[bp++] = CalcLRC(send_buf,1,bp-1);
	send_buf[bp++] = 0x7F;

	commSendData(MODEM_UART_PORT, send_buf, bp);

	printf("[logic]  Magentic_Time_Recv \r\n");
}

void Send_Magnetic_data()
{
	//printf("[LOGIC] Send_Magnetic_datat\r\n");
	uint8_t bp = 0;
	uint16_t calc_lrc = 0;

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;			//STX
	send_buf[bp++] = 0x0C;			//Length~LRC Size
	send_buf[bp++] = RF_CMD_SEND;			//OP-Code(Modem Send Opcode)

	send_buf[bp++] = (ls.magnetic.mag_x >> 8) & 0xFF;
	send_buf[bp++] = (ls.magnetic.mag_x) & 0xFF;
	send_buf[bp++] = (ls.magnetic.mag_y >> 8) & 0xFF;
	send_buf[bp++] = (ls.magnetic.mag_y) & 0xFF;
	send_buf[bp++] = (ls.magnetic.mag_z >> 8) & 0xFF;
	send_buf[bp++] = (ls.magnetic.mag_z) & 0xFF;
	send_buf[bp++] = (ls.magnetic.traffic_avg_60sec >> 8) & 0xFF;
	send_buf[bp++] = (ls.magnetic.traffic_avg_60sec) & 0xFF;
	send_buf[bp++] = ls.magnetic.Speed;

	send_buf[bp++] = 0xFF; 	//CRC;
	send_buf[bp++] = 0x7F;

	commSendData(DEBUG_UART_PORT, send_buf, bp);
}
