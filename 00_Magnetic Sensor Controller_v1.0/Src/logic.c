/*
 * logic.c
 *
 *  Created on: 2018. 9. 13.
 *      Author: ShinSeungCheol
 */

#define TITLE  	"LEDO Co., Ltd "
#define VERSION  "HW:VCU Ver2.2 SW : v1.0.0 "

char swTITLE[32] = TITLE;
char swVER[32] = VERSION;

char rep_str_buff[512];

#include <string.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "global.h"
#include "uart.h"
#include "image.h"
#include "logic.h"
#include "peripherals.h"
#include "tm_stm32_onewire.h"
#include "tm_stm32_delay.h"
#include "time.h"

extern I2C_HandleTypeDef hi2c1;
extern IWDG_HandleTypeDef hiwdg;
extern ADC_HandleTypeDef hadc1;
extern RTC_HandleTypeDef hrtc;
extern osMessageQId RFQueueHandle;
extern osMessageQId SensorQueueHandle;
extern osMessageQId IoTQueueHandle;

static TickType_t CalcTickDiff(TickType_t oldTicks, TickType_t newTicks)
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

void adcInit()
{
	HAL_NVIC_EnableIRQ(ADC_IRQn);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance == ADC1) {
		ls.cds.state = pdTRUE;
		ls.cds.value = HAL_ADC_GetValue(hadc);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == DRDY_INPUT_Pin)
    {
        Magnetic_Sensor_Read();  // 사용자 정의 처리
    }
}

static uint8_t ReverseByte(uint8_t src)
{
    uint8_t ret = 0x00;
    for (register uint8_t i=0; i<8; i++)
    {
        if (src & BITMASK_DCR[i])
        {
            ret |= BITMASK_ICR[i];
        }
    }

    return ret;
}

void logicInit()
{


	printf("======================================\r\n");
	printf("  Magnetic Sensor Control v1.0 \r\n");
	printf("  LEDO Co,. Ltd. \r\n");
	printf("======================================\r\n");
	commInit();
	periInit();
	adcInit();
	sdcardInit();

	memset(&ls, 0x00, sizeof(LogicStatus_T));
	ls.dip.Init = pdFALSE;
	ls.JamCriteria = 40;

	HAL_RTC_GetTime(&hrtc, &ls.gps.sTime, FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &ls.gps.sDate, FORMAT_BIN);

	ls.DisplayOffSend = pdTRUE;

	//DB_Read();
	Magnetic_Sensor_Init();
	sprintf(rep_str_buff, " Magnetic Sensor Control Start \r\n");
	report_append_str(ls.gps.sDate, ls.gps.sTime, rep_str_buff);

	ls.RfModule.isModemRdy = pdTRUE;

}

void logicADCTask()
{
	const TickType_t tickNow = xTaskGetTickCount();

	if (CalcTickDiff(ls.cds.timeoutADCTicks, tickNow) < 1000) {	//1초 마다 수행
		return;
	}

	//Recv_Rsponse(0, 0, 0);
	ls.cds.timeoutADCTicks = tickNow;

	ls.cds.state = pdFALSE;

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ls.adc.adc_dma, ADC_CHANNEL);

	//HAL_ADC_Start_IT(&hadc1);

/*
	while (ls.cds.state == pdFALSE) {
	}
*/
	//======================================
	// CDS
	//======================================

	ls.cds.value = ls.adc.adc_dma[0];

	for (uint8_t i = 9; i > 0; i--) {
		ls.cds.average[i] = ls.cds.average[i - 1];
	}

	ls.cds.average[0] = ls.cds.value;

	uint32_t sum = 0;

	for (uint8_t i = 0; i < 10; i++) {
		sum += ls.cds.average[i];
	}

	ls.cds.avg_value = sum / 10;

	if (ls.cds.avg_value < CDS_MIN) {
		ls.cds.Brt = MAX_BRIGHT_STEP;
	} else if (ls.cds.avg_value > CDS_MAX) {
		ls.cds.Brt = MIN_BRIGHT_STEP;
	} else {
		ls.cds.Brt = MIN_BRIGHT_STEP
				+ ((MAX_BRIGHT_STEP - MIN_BRIGHT_STEP)
						- ((MAX_BRIGHT_STEP - MIN_BRIGHT_STEP)
								* ((ls.cds.avg_value - CDS_MIN)
										/ (CDS_MAX - CDS_MIN))));
	}

	//======================================
	// battery
	//======================================
	uint16_t battAdc = ls.adc.adc_dma[1];
	double battDouble = ((battAdc * ADC_VACC) / (ADC_RESOLUTION - 1)) * (1.0 + 4.7 / 1.0);

	//double battDouble = ((battAdc * ADC_VACC) / (ADC_RESOLUTION - 1)) * (0.1 + 4.7 / 0.1);
	if (battDouble > 25.0) {
		ls.adc.Value = 250;
	} else {
		ls.adc.Value = (uint8_t) (battDouble * 10.0);
	}

	if(ls.adc.Value >= 135)
		ls.stat.Battery = 100;
	else if(ls.adc.Value <= 115)
		ls.stat.Battery = 5;
	else
	{
		ls.stat.Battery = (ls.adc.Value - 115) * 5;
	}

	HAL_ADC_Stop_DMA(&hadc1);

	printf("[LOGIC ] ADC [%4d][%4d] BRT[%4d] BATT[%4d/%d %%]\r\n", ls.adc.adc_dma[0], ls.adc.adc_dma[1], ls.cds.Brt, ls.adc.Value, ls.stat.Battery);
}

void logicPeriTask()
{
	const TickType_t tickNow = xTaskGetTickCount();
	if (CalcTickDiff(ls.tick.Indi, tickNow) <= 100) {
		return;
	}

	ls.tick.Indi = tickNow;

	uint8_t lamp = 0x00;

	if (ls.indi.Prog) {
		ls.indi.Prog = pdFALSE;

		lamp |= PERI_LAMP1;
	} else
		ls.indi.Prog = pdTRUE;

	if (ls.indi.Sensor) {
		ls.indi.Sensor = pdFALSE;
		lamp |= PERI_LAMP2;
	}

	if (ls.RfModule.isRecv )
	{
		ls.RfModule.isRecv = pdFALSE;
		lamp |= PERI_LAMP3;
	}

	periSetLamp(lamp);

	uint8_t input = periGetSwitch();

	if(ls.dip.Init == pdFALSE || (ls.dip.Last != input))
	{
		ls.dip.Last = input;
		ls.stat.isTestMode = (input & INPUT_TESTMODE_MASK) >> 7;			//test mode
		ls.stat.TestMode = (input & INPUT_TESTCONTRL_MODE_MASK) >> 6;		//test control(light(on)/speed(off))
		ls.stat.ControlMode = (input & INPUT_TESTCONTRL_NO_MASK) >> 5;		//all or group control

		ls.stat.SectionID = ((input & INPUT_SECTION_MASK) >> 3) ? 11 : 10;	//section set
		ls.stat.Jam = (input & INPUT_JAM_MASK) >> 2;						//jam 50km(on)/40km(off)

		if((input & INPUT_DISPLAY_RESET_MASK) >> 4)							//display reset
		{
			printf("[UART ] PMS Reset\r\n ");
			Send_Modem_Display_Reset();
		}

		ls.stat.TestPaceSpeed = (input & INPUT_TEST_PACESPEED_MASK);		//test speed/all or brt
		ls.stat.TestNo = (input & INPUT_TEST_PACESPEED_MASK);				//test speed/brt

		/*if (ls.RfModule.SectionID != ls.stat.SectionID) {
			printf("[UART ] Modem isRegisterCall\r\n ");

			ls.RfModule.isModemRdy = pdFALSE;
			ls.RfModule.isModemRdyCall = pdFALSE;
		}*/

		if(ls.stat.isTestMode){
			ls.SendCount = 1000;
		}
		else
		{
			ls.DisplayOffSend = pdTRUE;
		}

		if(ls.stat.Jam)
			ls.JamCriteria = 50;
		else
			ls.JamCriteria = 40;

		printf("[LOGIC] DIP S/W VALUE [%02X][%02d]\r\n", ls.dip.Last, ls.stat.isTestMode);
		ls.OnDeviceID = 0;

		if(ls.dip.Init == pdFALSE)
			Send_Modem_Standby(ls.stat.SectionID);

		ls.dip.Init = pdTRUE;
	}

	uint8_t GP_input = periGetInput();
	if(ls.gpin.Init == pdFALSE || (ls.gpin.Last != GP_input))
	{
		ls.gpin.Last = GP_input;
		//printf("[LOGIC] GP INPUT VALUE [%02X]\r\n", ls.gpin.Last);
		ls.gpin.Init = pdTRUE;

	}
	/*for (;;) {
	    GPIO_PinState drdy = HAL_GPIO_ReadPin(DRDY_INPUT_GPIO_Port, DRDY_INPUT_Pin);
	    printf("DRDY = %s\n", drdy == GPIO_PIN_RESET ? "LOW" : "HIGH");
	    HAL_Delay(5);
	}*/


	if (ls.RfModule.isModemRdy == pdFALSE) {
		if (ls.RfModule.isModemRdyCall) {
			if (ls.RfModule.isModemRdyCount-- == 0)
				ls.RfModule.isModemRdyCall = pdFALSE;
		} else {
			ls.RfModule.isModemRdyCall = pdTRUE;
			ls.RfModule.isModemRdyCount = 10;
			Send_Modem_Standby(ls.stat.SectionID);
		}
	}
}



void logicIdleTask()
{
    HAL_IWDG_Refresh(&hiwdg);
}


void logicProcTask()
{
	if(ls.RfModule.isModemRdy == pdFALSE)
		return;

	if(ls.stat.isTestMode)
		ProcTestMode();
	else if(ls.temp_config.Mode == 0x01) {
		ProcManualMode();
	}
	else {
		ProcWorkMode();
	}
}

static void ProcTestMode()
{
	if(ls.stat.TestMode)		//전체점등 모드
	{
		switch (ls.stat.TestNo) {
			case 0:
				ls.Bright = TEST_BRIGHT_100;
				break;
			case 1:
				ls.Bright = TEST_BRIGHT_80;
				break;
			case 2:
				ls.Bright = TEST_BRIGHT_50;
				break;
			case 3:
				ls.Bright = TEST_BRIGHT_20;
				break;
		}

		if (ls.SendCount) {
			ls.SendCount--;
			if (ls.SendCount == 0) {
				ls.SendCount = 100;

				if (ls.stat.ControlMode){
					Send_Modem_Display(ls.stat.TestNo, ls.Bright);
				}
				else
				{
					ls.Bright = ls.cds.Brt;
					Send_Modem_Display(0xFF, ls.Bright);
				}
			}
		}

	}
	else
	{
		ls.NowPaceSpeed = 90;

		if (ls.OnDeviceID == 0) {
			ls.OnCount = ((3600 * 12) / ls.NowPaceSpeed); 		//((시간 * 거리) / 속도)

			ls.OnDeviceID++;
			ls.TestSendCount = ls.OnCount;
			printf("[LOGIC] Speed [%3d km/h] OnCount[%02d]\r\n", ls.NowPaceSpeed, ls.OnCount);
		}

		if (ls.TestSendCount) {
			ls.TestSendCount--;
			if (ls.TestSendCount == 0) {
				ls.TestSendCount = ls.OnCount;
				ls.Bright = ls.cds.Brt;
				uint16_t brt;
				if(ls.stat.TestNo)
				{
					switch(ls.OnDeviceID)
					{
					case 1:
						brt = ls.Bright * (1-ls.stat.TestNo * 0.3);
						break;
					case 2:
						brt = ls.Bright * (1-ls.stat.TestNo * 0.2);
						break;
					case 3:
						brt = ls.Bright * (1-ls.stat.TestNo * 0.1);
						break;
					case 4:
						brt = ls.Bright;
						break;
					}

					if(brt<= 2 )
					{
						brt = 2;
					}

					Send_Modem_Display(ls.OnDeviceID - 1, brt);

				}
				else
				{
					Send_Modem_Display(ls.OnDeviceID - 1, ls.Bright);
				}

				ls.OnDeviceID++;
				if (ls.OnDeviceID > 4)
					ls.OnDeviceID = 0;
			}
		}

	}
}

static void ProcWorkMode()
{
	if(ls.SendOn)
	{
		if (ls.RfModule.isModemRdy == pdTRUE) {
			if (ls.OnDeviceID == 0) {
				ls.OnCount = (36000 / ls.NowPaceSpeed);

				if (ls.OnCount >= 0) {
					ls.OnDeviceID++;
					ls.SendCount = ls.OnCount;
					printf(
							"[LOGIC] Vehicle Speed[%3d] PaceSpeed [%3d] OnCount[%02d]\r\n",
							ls.old_sensor.VehicleSpeed, ls.NowPaceSpeed,
							ls.OnCount);
				}
			}

			if (ls.SendCount) {
				ls.SendCount--;
				if (ls.SendCount == 0) {
					ls.SendCount = ls.OnCount;
					ls.Bright = ls.cds.Brt;

					uint16_t brt;

					if(ls.stat.TestNo)
					{
						switch(ls.OnDeviceID)
						{
						case 1:
							brt = ls.Bright * (1-ls.stat.TestNo * 0.3);
							break;
						case 2:
							brt = ls.Bright * (1-ls.stat.TestNo * 0.2);
							break;
						case 3:
							brt = ls.Bright * (1-ls.stat.TestNo * 0.1);
							break;
						case 4:
							brt = ls.Bright;
							break;
						}

						if(brt<= 2 )
						{
							brt = 2;
						}

						Send_Modem_Display(ls.OnDeviceID - 1, brt);

					}
					else
					{
						Send_Modem_Display(ls.OnDeviceID - 1, ls.Bright);
					}


					ls.OnDeviceID++;
					if (ls.OnDeviceID > 4)
						ls.OnDeviceID = 0;
				}
			}
		}
	}
	else
	{
		ls.OnDeviceID = 0;

		if (ls.SendCount) {
			ls.SendCount--;
			if (ls.SendCount == 0) {
				ls.SendCount = 1000;
				Send_Modem_Display_Off();
			}
		}
		else
		{
			ls.SendCount = 1000;
			Send_Modem_Display_Off();
		}
	}

	if (ls.logEnable == pdTRUE) {
		ls.logEnable = pdFALSE;

		if(ls.SendOn)
		{
			sprintf(rep_str_buff,
				"PMS On - Vehicle Speed %3d km/h PaceMakerSpeed %3d km/h\r\n", ls.old_sensor.VehicleSpeed, ls.NowPaceSpeed);
		}
		else
		{
			sprintf(rep_str_buff, "PMS OFF - Vehicle Speed %3d km/h \r\n", ls.old_sensor.VehicleSpeed);
		}

		HAL_RTC_GetTime(&hrtc, &ls.gps.sTime, FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &ls.gps.sDate, FORMAT_BIN);

		report_append_str(ls.gps.sDate, ls.gps.sTime, rep_str_buff);
	}
}

static void ProcManualMode()
{
		if (ls.RfModule.isModemRdy == pdTRUE) {
			if (ls.OnDeviceID == 0) {
				ls.NowPaceSpeed = ls.config.PaceSpeed;

				ls.OnCount = (36000 / ls.config.PaceSpeed);

				if (ls.OnCount >= 0) {
					ls.OnDeviceID++;
					ls.SendCount = ls.OnCount;
					printf(
							"[LOGIC] Vehicle Speed[%3d] PaceSpeed [%3d] OnCount[%02d]\r\n",
							ls.old_sensor.VehicleSpeed, ls.NowPaceSpeed,
							ls.OnCount);
				}
			}

			if (ls.SendCount) {
				ls.SendCount--;
				if (ls.SendCount == 0) {
					ls.SendCount = ls.OnCount;
					ls.Bright = ls.cds.Brt;

					uint16_t brt;

					if(ls.stat.TestNo)
					{
						switch(ls.OnDeviceID)
						{
						case 1:
							brt = ls.Bright * (1-ls.stat.TestNo * 0.3);
							break;
						case 2:
							brt = ls.Bright * (1-ls.stat.TestNo * 0.2);
							break;
						case 3:
							brt = ls.Bright * (1-ls.stat.TestNo * 0.1);
							break;
						case 4:
							brt = ls.Bright;
							break;
						}

						if(brt<= 2 )
						{
							brt = 2;
						}

						Send_Modem_Display(ls.OnDeviceID - 1, brt);

					}
					else
					{
						Send_Modem_Display(ls.OnDeviceID - 1, ls.Bright);
					}

					ls.OnDeviceID++;
					if (ls.OnDeviceID > 4)
						ls.OnDeviceID = 0;
				}
			}
		}

	if (ls.logEnable == pdTRUE) {
		ls.logEnable = pdFALSE;

		if(ls.SendOn)
		{
			sprintf(rep_str_buff,
				"PMS Manual On PaceMakerSpeed %3d km/h\r\n", ls.NowPaceSpeed);
		}
		else
		{
			sprintf(rep_str_buff, "PMS OFF - Vehicle Speed %3d km/h \r\n", ls.old_sensor.VehicleSpeed);
		}

		HAL_RTC_GetTime(&hrtc, &ls.gps.sTime, FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &ls.gps.sDate, FORMAT_BIN);

		report_append_str(ls.gps.sDate, ls.gps.sTime, rep_str_buff);
	}
}

void logicUartTask()
{
	Sensor_Receive_Check();
	Gps_Receive_Check();

	const TickType_t tickNow = xTaskGetTickCount();
	if (CalcTickDiff(ls.tick.Sensor, tickNow) >= 10000 && ls.old_sensor.SensorStatus != 0x34) {
		ls.old_sensor.VehicleSpeed = 0;
		ls.old_sensor.VehicleSpeed_Max = 0;
		ls.old_sensor.SensorReason = 0;

		ls.SendCount = 0;
		ls.SendOn = pdFALSE;

		ls.old_sensor.SensorStatus = 0x34;
		printf("[LOGIC] Sensor TimeOut\r\n");
	}

	osEvent event = osMessageGet(SensorQueueHandle, 0);

	while (event.status == osEventMessage) {
		COMMAND_T commandPacket;

		memcpy(&commandPacket, (COMMAND_T*) event.value.v, sizeof(COMMAND_T));

		if (commandPacket.Result != 0x00) {
			printf("[LOGIC ] Comm Data Recv Nak[%02x][%02x][%02x] \r\n",
					commandPacket.opcode, commandPacket.subcode,
					commandPacket.Result);
		} else {
			switch (commandPacket.opcode) {
				case OP_SENSORDATA_SEND:		//SensorData
					ls.indi.Sensor = pdTRUE;
					ls.tick.Sensor = tickNow;

					ls.new_sensor.SensorValue = commandPacket.data[1];
					ls.new_sensor.VehicleSpeed = commandPacket.data[2];
					ls.new_sensor.VehicleSpeed_Max = commandPacket.data[3];
					ls.new_sensor.SensorReason = commandPacket.data[4];
					ls.new_sensor.BatteryLevel = commandPacket.data[5];
					ls.new_sensor.SensorStatus = commandPacket.data[6];

					ls.old_sensor.VehicleSpeed = ls.new_sensor.VehicleSpeed;
					ls.old_sensor.VehicleSpeed_Max = ls.new_sensor.VehicleSpeed_Max;
					ls.old_sensor.SensorReason = ls.new_sensor.SensorReason;
					ls.old_sensor.BatteryLevel = ls.new_sensor.BatteryLevel;
					ls.old_sensor.SensorStatus = ls.new_sensor.SensorStatus;

					if(ls.old_sensor.VehicleSpeed > ls.config.StartSpeed && ls.old_sensor.VehicleSpeed <= ls.config.EndSpeed)
					{
						ls.ChangeOnCount++;
						ls.ChangeOffCount = 0;

						if (ls.ChangeOnCount >= 10) {	//10초 이상

							if (ls.ChangeOnCount > 100)
								ls.ChangeOnCount = 15;

							if(ls.SendOn == pdFALSE)
								ls.logEnable = pdTRUE;

							ls.SendOn = pdTRUE;

							ls.NowPaceSpeed = ls.config.PaceSpeed;
						}
					}
					else
					{
						ls.ChangeOnCount = 0;
						ls.ChangeOffCount++;

						if (ls.ChangeOffCount >= 10) {	//10초 이상

							if (ls.ChangeOffCount > 100)
								ls.ChangeOffCount = 15;

							if (ls.SendOn == pdTRUE)
								ls.logEnable = pdTRUE;

							ls.SendOn = pdFALSE;

							ls.NowPaceSpeed = 0;

							ls.SendCount = 1000;	//1초간격
						}
					}

					printf("[LOGIC ] SENSOR Speed(Avg/Max)[%3d/%3d][%d][On : %3d][Off : %3d]\r\n",
							commandPacket.data[2], commandPacket.data[3], ls.SendOn, ls.ChangeOnCount, ls.ChangeOffCount);
					break;
				case OP_GPSDATA_SEND:		//SensorData
					if(ls.gps.sTime.Minutes != commandPacket.data[4])
					{
						if(ls.SendOn)
							ls.logEnable = pdTRUE;
					}

					ls.gps.sDate.Year = commandPacket.data[0];
					ls.gps.sDate.Month = commandPacket.data[1];
					ls.gps.sDate.Date = commandPacket.data[2];
					ls.gps.sTime.Hours = (commandPacket.data[3] + 9);
					ls.gps.sTime.Minutes = commandPacket.data[4];
					ls.gps.sTime.Seconds = commandPacket.data[5];

					ls.gps.Stat = pdTRUE;

					HAL_RTC_SetTime(&hrtc, &ls.gps.sTime, FORMAT_BIN);
					HAL_RTC_SetDate(&hrtc, &ls.gps.sDate, FORMAT_BIN);

					printf("[Logic][Gps  ] %02d-%02d-%02d %02d:%02d:%02d \r\n",
							ls.gps.sDate.Year, ls.gps.sDate.Month, ls.gps.sDate.Date,
							ls.gps.sTime.Hours, ls.gps.sTime.Minutes, ls.gps.sTime.Seconds);

					ls.gps.Hour_KST = (ls.gps.sTime.Hours + 9) % 24;
					ls.gps.Min = ls.gps.sTime.Minutes;
					ls.gps.Sec = ls.gps.sTime.Seconds;

					break;
			}
		}
		event = osMessageGet(SensorQueueHandle, 0);
	}
}

void logicIoTTask()
{
	IoT_Receive_Check();

	osEvent event = osMessageGet(IoTQueueHandle, 0);
		while (event.status == osEventMessage) {
			COMMAND_T commandPacket;

			memcpy(&commandPacket, (COMMAND_T*) event.value.v, sizeof(COMMAND_T));

			printf("[LOGIC ] IoTQueueHandle[%2d] \r\n", commandPacket.size);
			if (commandPacket.Result != RESPONSE_ACK) {
				printf("[LOGIC ] Comm Data Recv Nak[%02x][%02x][%02x] \r\n", commandPacket.opcode, commandPacket.subcode, commandPacket.Result);
				//Set_Response(commandPacket.opcode, commandPacket.subcode, commandPacket.Result);
			} else {

				for(int i = 0; i < commandPacket.size; )
				{
					printf("[LOGIC ] [IoT] [%d/%d][%02X]\r\n", i, commandPacket.size, commandPacket.data[i]);
					switch(commandPacket.data[i++])
					{
						case TYPE_PROTOCAL_VERSION:
							i++;
							printf("[LOGIC ] [IoT] Protocol Version [%d] \r\n", commandPacket.data[i++]);
							break;
						case TYPE_REBOOTING:
							i++;
							ls.isReboot = commandPacket.data[i++];
							printf("[LOGIC ] [IoT] Rebooting [%d] \r\n", ls.isReboot);

							Send_Response(TYPE_REBOOTING, RESPONSE_ACK);

							if(ls.isReboot == 0x02 || ls.isReboot == 0x03)
								NVIC_SystemReset();
							break;
						case TYPE_TRANSMISSSION_DURATION:
							i++;
							ls.temp_config.Duration = (commandPacket.data[i++] << 24)
									| (commandPacket.data[i++] << 16)
									| (commandPacket.data[i++] << 8)
									| (commandPacket.data[i++]);

							if(ls.temp_config.Duration <= 60)
								ls.temp_config.Duration = 60;

							printf("[LOGIC ] [IoT] Transmission Duration [%d] \r\n", ls.temp_config.Duration);
							DB_Save();
							Send_Response(TYPE_TRANSMISSSION_DURATION, RESPONSE_ACK);
							break;
						case TYPE_START_SPEED:
							i++;
							ls.temp_config.StartSpeed = commandPacket.data[i++];
							break;
						case TYPE_END_SPEED:
							i++;
							ls.temp_config.EndSpeed = commandPacket.data[i++];
							break;
						case TYPE_PACE_SPEED:
							i++;
							ls.temp_config.PaceSpeed = commandPacket.data[i++];
							break;
						case TYPE_OPERAT_MODE:
							i++;
							ls.temp_config.Mode = commandPacket.data[i++];
							printf("[LOGIC ] [IoT] Mode [%d] \r\n", ls.temp_config.Mode);
							break;
						default:
							printf("[LOGIC ] [IoT] Type Error [%d] \r\n", commandPacket.data[i]);
							break;
					}
				}
			}

			memcpy(&ls.config, &ls.temp_config, sizeof(Config_t));

			DB_Save();

			event = osMessageGet(IoTQueueHandle, 0);
		}


	const TickType_t tickNow = xTaskGetTickCount();

    if (ls.iot.isJoin == pdFALSE && CalcTickDiff(ls.tick.IoT, tickNow) >= 10000) {
    	ls.tick.IoT = tickNow;
    	printf("[Logic][IoT  ] IoT Joind Check[%d]\r\n", ls.iot.isJoin);
		if (ls.iot.isJoin == pdFALSE) {
			Send_command(0x02, 0);
		}
	}

    if (ls.iot.isJoin == pdTRUE && CalcTickDiff(ls.tick.IoT, tickNow) >= (ls.config.Duration * 1000)) {
       	ls.tick.IoT = tickNow;

		//if (ls.iot.isCall) {
		//	ls.iot.isCall = pdFALSE;

			printf("[Logic][IoT  ] IoT Send Packet\r\n");

			Send_SensorValue();
		//}
	}
}
uint8_t response_buf[128];
char command_str[256];

void Set_IoT_Joined(uint8_t v)
{
	ls.iot.isJoin = v;
	printf("[Logic][IoT  ] IoT Joind Check Result[%d]\r\n", ls.iot.isJoin);
}

void Send_command(uint8_t command, uint8_t set_value)
{
	memset(command_str, 0x00, sizeof(command_str));
	switch(command)
	{
		case 0x00:
			sprintf(command_str, "AT+LOG %1d\n", set_value);
			break;
		case 0x01:
			sprintf(command_str, "AT+CFM %1d\n", set_value);
			break;
		case 0x02:
			sprintf(command_str, "AT+JOINED?\n");
			break;
		case 0x03:
			sprintf(command_str, "AT+RST\n");
			break;
		case 0x04:
			sprintf(command_str, "AT+DEVT\n");
			break;
		case 0x05:
			sprintf(command_str, "AT+RST\n");
			break;
	}

	UART_StrOut(IOT_UART_PORT, &command_str);

	printf("[logic.c] Send_command - %s\r\n", command_str);
}

void Send_SensorValue()
{
	uint8_t bp = 0;
	uint8_t ptr = 0;
	uint8_t calc_lrc = 0;

	memset((uint8_t*)&response_buf, 0, sizeof(response_buf));

	response_buf[bp++] = TYPE_PROTOCAL_VERSION;				// Type
	response_buf[bp++] = UINT8_LENGTH;						// Length
	response_buf[bp++] = PROTOCOL_VERSION_MAJOR;			// Protocol Version

	response_buf[bp++] = TYPE_BATTERY;						// Type
	response_buf[bp++] = UINT8_LENGTH;						// Length
	response_buf[bp++] = ls.stat.Battery;								// Battery(0~100%)

	response_buf[bp++] = TYPE_SENSING;						// Type
	response_buf[bp++] = UINT8_LENGTH;						// Length
	response_buf[bp++] = TYPE_SENSING_CYCLE;				// 센싱타입(0x01 : 주기 정보, 0x02 : 이벤트)

	response_buf[bp++] = TYPE_TRANSMISSSION_DURATION;						// Type
	response_buf[bp++] = UINT32_LENGTH;						// Length
	response_buf[bp++] = (ls.config.Duration >> 24) & 0xFF;				// 전송주기
	response_buf[bp++] = (ls.config.Duration >> 16) & 0xFF;;				// 전송주기
	response_buf[bp++] = (ls.config.Duration >> 8) & 0xFF;;				// 전송주기
	response_buf[bp++] = (ls.config.Duration) & 0xFF;;		// 전송주기

	response_buf[bp++] = TYPE_OPERATION_STATUS;				// Type
	response_buf[bp++] = UINT8_LENGTH;						// Length
	response_buf[bp++] = ls.SendOn;							// 동작상태

	response_buf[bp++] = TYPE_VEHICLE_SPEED;				// Type
	response_buf[bp++] = UINT8_LENGTH;						// Length
	response_buf[bp++] = ls.old_sensor.VehicleSpeed;		// 차량속도

	response_buf[bp++] = TYPE_SENSOR_STATUS;				// Type
	response_buf[bp++] = UINT8_LENGTH;						// Length
	response_buf[bp++] = ls.old_sensor.SensorStatus - 0x30;	// Sensor Status

	memset(command_str, 0x00, sizeof(command_str));

	sprintf(command_str, "AT+SEND 0x02 ");

	BinToHex(response_buf, bp, &command_str[13], (sizeof(command_str)));
	UART_StrOut(IOT_UART_PORT, command_str);

	UART_StrOut(IOT_UART_PORT, "\r\n");

	printf("[logic.c] Recv_Rsponse - %s \r\n", &command_str);
}


void Send_Response(uint8_t command, uint8_t result)
{
	uint8_t bp = 0;
	uint8_t ptr = 0;
	uint8_t calc_lrc = 0;

	memset((uint8_t*) &response_buf, 0, sizeof(response_buf));

	response_buf[bp++] = TYPE_PROTOCAL_VERSION;				// Type
	response_buf[bp++] = UINT8_LENGTH;						// Length
	response_buf[bp++] = PROTOCOL_VERSION_MAJOR;			// Protocol Version

	response_buf[bp++] = TYPE_CONTROL_RESPONSE;				// Type
	response_buf[bp++] = UINT16_LENGTH;						// Length
	response_buf[bp++] = command;							// Protocol Version
	response_buf[bp++] = result;							// Protocol Version

	memset(command_str, 0x00, sizeof(command_str));

	sprintf(command_str, "AT+SEND 0x02 ");

	BinToHex(response_buf, bp, &command_str[13], (sizeof(command_str)));
	UART_StrOut(IOT_UART_PORT, command_str);

	UART_StrOut(IOT_UART_PORT, "\r\n");

	printf("[logic.c] Recv_Rsponse - %s \r\n", &command_str);

	printf("[logic.c] Send_command - %s\r\n", command_str);
}


void logicDRMTask()
{
	const TickType_t tickNow = xTaskGetTickCount();
	RF_Receive_Check();

	osEvent event = osMessageGet(RFQueueHandle, 0);
	while (event.status == osEventMessage) {
		COMMAND_T Packet;

		memcpy(&Packet, (COMMAND_T*) event.value.v, sizeof(COMMAND_T));

		printf("\r\n[Logic] Modem OP_CODE [%d][%2X]\r\n", Packet.groupIndex, Packet.opcode);

		//Packet.groupIndex = Find_GroupIndex(Packet.groupID);

		ls.RfModule.isRecv = pdTRUE;

		switch (Packet.opcode)		//Opcode
		{
		   	case RF_CMD_BOOT_MSG:		//초기 부팅시 수신 메세지
		   		printf("[LOGIC] Modem Boot [S-ID : %d][D-ID : %d]\r\n", Packet.data[0], Packet.data[1]);
		   		//Send_Modem_Standby(ls.RfModule.SectionID);
		    	break;

		   	case RF_CMD_REG_STATUS:		//C-Resigter 상태 정보
		   		memcpy(&ls.RfModule.Status_Control,  &Packet.data[0], 10);
		   		printf("[LOGIC] =============Modem[%d] Register============\r\n", Packet.groupIndex);
		   		printf("[LOGIC] 1. Status      : [ %02x ]\r\n", ls.RfModule.Status_Control);
		   		printf("[LOGIC] 2. Section ID  : [ %02x ]\r\n", ls.RfModule.SectionID);
		   		printf("[LOGIC] 3. Device ID   : [ %02x ]\r\n", ls.RfModule.DeviceID);
		   		printf("[LOGIC] 4. RF Channel  : [ %02x ]\r\n", ls.RfModule.Rf_ch);
		   		printf("[LOGIC] 5. TX Power    : [ %02x ]\r\n", ls.RfModule.Tx_power);
		   		printf("[LOGIC] 6. Tx RSSI(dBm): [ %02x ]\r\n", ls.RfModule.Tx_rssi);
		   		printf("[LOGIC] 7. Double Send : [ %02x ]\r\n", ls.RfModule.Dbl_send);
		   		printf("[LOGIC] 8. LED TIMEOUT : [ %02x ]\r\n", ls.RfModule.Led_Timeout);
		   		printf("[LOGIC] =======================================\r\n");
		   		ls.RfModule.isModemRdy = pdTRUE;

				Send_Modem_Display_Off();
	    		break;

	    	default:
	    		break;
		}
		event = osMessageGet(RFQueueHandle, 0);
	}
}

void Send_Modem_Standby(uint8_t SectionID)
{
	//printf("[LOGIC] Modem Send_Standby[%d]\r\n", SectionID);
	uint8_t bp = 0;
	uint16_t calc_lrc = 0;

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;			//STX
	send_buf[bp++] = 0x0A;			//Length~LRC Size

	send_buf[bp++] = RF_CMD_REG_WRITE;			//OP-Code(Display Data)

	send_buf[bp++] = 0x11;
	send_buf[bp++] = SectionID;		//SectionID
	send_buf[bp++] = 0x00;			//DeviceID
	send_buf[bp++] = 0x00;			//RF-CH
	send_buf[bp++] = 0x09;
	send_buf[bp++] = 0x78;			//typical setting(-120dBm)
	send_buf[bp++] = 0x00;			//Double-Send

	send_buf[bp++] = 0xFF; 	//CRC;
	send_buf[bp++] = 0x7F;

	commSendData(RF_UART_PORT, send_buf, bp);
}


void Send_Modem_Reset_Call(uint8_t GroupIndex)
{
	printf("[LOGIC] Modem Send_Modem_Reset Call\r\n");
	uint8_t bp = 0;
	uint16_t calc_lrc = 0;

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;			//STX
	send_buf[bp++] = 0x07;			//Length~LRC Size
	send_buf[bp++] = RF_CMD_SEND;			//OP-Code(Modem Send Opcode)

	send_buf[bp++] = 4;
	send_buf[bp++] = 0xFF;
	send_buf[bp++] = DRM_CMD_RESET;
	send_buf[bp++] = 0x00;

	send_buf[bp++] = 0xFF; 	//CRC;
	send_buf[bp++] = 0x7F;


	commSendData(RF_UART_PORT, send_buf, bp);
}


void Send_Modem_Display(uint8_t deviceID, uint8_t brt)
{
	printf("[LOGIC] Send_Modem_DisplayMode Test [%d] [%d]\r\n", deviceID, brt);
	uint8_t bp = 0;
	uint16_t calc_lrc = 0;

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;			//STX
	send_buf[bp++] = 0x08;			//Length~LRC Size
	send_buf[bp++] = RF_CMD_SEND;	//OP-Code(Modem Send Opcode)

	send_buf[bp++] = 5;
	send_buf[bp++] = deviceID;
	send_buf[bp++] = DRM_CMD_DISP_SET;
	send_buf[bp++] = deviceID;
	send_buf[bp++] = brt;

	send_buf[bp++] = 0xFF; 	//CRC;
	send_buf[bp++] = 0x7F;

	if (ls.RfModule.isModemRdy == pdTRUE)
		commSendData(RF_UART_PORT, send_buf, bp);
}

void Send_Modem_Display_Off()
{
	printf("[LOGIC] Send_Modem_Display_Off\r\n");
	uint8_t bp = 0;
	uint16_t calc_lrc = 0;

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;			//STX
	send_buf[bp++] = 0x06;			//Length~LRC Size
	send_buf[bp++] = RF_CMD_SEND;			//OP-Code(Modem Send Opcode)

	send_buf[bp++] = 3;
	send_buf[bp++] = 0xFF;
	send_buf[bp++] = DRM_CMD_DISP_OFF;

	send_buf[bp++] = 0xFF; 	//CRC;
	send_buf[bp++] = 0x7F;

	if (ls.RfModule.isModemRdy == pdTRUE)
		commSendData(RF_UART_PORT, send_buf, bp);
}

void Send_Modem_Display_Reset()
{
	printf("[LOGIC] Send_Modem_Display_Reset\r\n");
	uint8_t bp = 0;
	uint16_t calc_lrc = 0;

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;			//STX
	send_buf[bp++] = 0x06;			//Length~LRC Size
	send_buf[bp++] = RF_CMD_SEND;			//OP-Code(Modem Send Opcode)

	send_buf[bp++] = 3;
	send_buf[bp++] = 0xFF;
	send_buf[bp++] = DRM_CMD_RESET;

	send_buf[bp++] = 0xFF; 	//CRC;
	send_buf[bp++] = 0x7F;

	if (ls.RfModule.isModemRdy == pdTRUE)
		commSendData(RF_UART_PORT, send_buf, bp);
}


void DB_Save()
{
	int result = 0;
	uint8_t Write_Buff[64];
	uint16_t FRAM_Sub_Address = 0x00;

	memset(&Write_Buff, 0x00, sizeof(Write_Buff));

	/// I2C F-RAM Write
	Write_Buff[0] = (uint8_t)(FRAM_Sub_Address >> 8) & 0x00FF;
	Write_Buff[1] = (uint8_t)(FRAM_Sub_Address >> 0) & 0x00FF;


	memcpy(&Write_Buff[2], &ls.config, sizeof(Config_t));

	printf("[Logic][DB_Save] Mode(0:Auto, 1:Manual) :[%d][%s] \r\n", ls.config.Mode, ls.config.Mode?"Manual":"Auto");
	printf("[Logic][DB_Save] Speed(Start/End)	:[%3d/%3d] km/h \r\n", ls.config.StartSpeed, ls.config.EndSpeed);
	printf("[Logic][DB_Save] PaceSpeed			:[%3d] km/h \r\n", ls.config.PaceSpeed);
	printf("[Logic][DB_Save] Duration  			:[%5d] sec \r\n", ls.config.Duration);

	HAL_I2C_Master_Transmit(&hi2c1, 0xA0, &Write_Buff[0], sizeof(Config_t) + 2, 100);
	HAL_Delay(5);

	//printf("[Logic][DB_Sa] DB Save Complete...!! \r\n");
}


void DB_Read()
{
	uint8_t Send_Buff[8];
	uint8_t Read_Buff[8];
	uint16_t FRAM_Sub_Address = 0x00;

	memset(&Read_Buff, 0x00, sizeof(Read_Buff));
	memset(&Send_Buff, 0x00, sizeof(Send_Buff));

	//I2C F-RAM Read
	Send_Buff[0] = (uint8_t)(FRAM_Sub_Address >> 8) & 0x00FF;
	Send_Buff[1] = (uint8_t)(FRAM_Sub_Address >> 0) & 0x00FF;
	HAL_I2C_Master_Transmit(&hi2c1, 0xA0,  &Send_Buff[0], 2, 100);
	HAL_Delay(5);
	HAL_I2C_Master_Receive(&hi2c1, 0xA0,  &ls.config,  sizeof(Config_t), 100);
	HAL_Delay(5);

	if(ls.config.Duration <= 60)
		ls.config.Duration = 60;	//1 Minute(1 min x 60 sec)

	if(ls.config.EndSpeed == 0)
		ls.config.EndSpeed = 80;	//1 Minute(1 min x 60 sec)

	if(ls.config.PaceSpeed == 0)
		ls.config.PaceSpeed = 90;	//1 Minute(1 min x 60 sec)

	printf("[Logic][DB_Read] Mode(0:Auto, 1:Manual) :[%d][%s] \r\n", ls.config.Mode, ls.config.Mode?"Manual":"Auto");
	printf("[Logic][DB_Read] Speed(Start/End)	:[%3d/%3d] km/h \r\n", ls.config.StartSpeed, ls.config.EndSpeed);
	printf("[Logic][DB_Read] PaceSpeed			:[%3d] km/h \r\n", ls.config.PaceSpeed);
	printf("[Logic][DB_Read] Duration  			:[%5d] sec \r\n", ls.config.Duration);


	memcpy(&ls.temp_config, &ls.config, sizeof(Config_t));
}


void Magnetic_Sensor_Init()
{
	 uint8_t who_am_i = 0;
	    uint8_t cfg_a = 0x80;  // TEMP_EN=1, ODR=10Hz, MODE=Continuous
	    uint8_t cfg_c = 0x01;  // DRDY_on_PIN = 1
	    uint8_t check_reg = 0;
	    uint8_t status = 0;
	    uint8_t cfg_regs[3] = {0};

	    // 1. WHO_AM_I 확인 센서 연결상태 체크
	    HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR << 1, WHO_AM_I, 1, &who_am_i, 1, HAL_MAX_DELAY);
	    if (who_am_i != 0x40) {
	        printf("[ERROR] LIS2MDL not detected. WHO_AM_I = 0x%02X\r\n", who_am_i);
	        ls.magnetic.isconnect = pdFALSE;
	        return;
	    }

	    printf("[OK] LIS2MDL connected. WHO_AM_I = 0x%02X\r\n", who_am_i);
	    ls.magnetic.isconnect = pdTRUE;

	    // 2. SOFT_RST 설정
	    uint8_t rst = 0x20;
	    HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_ADDR << 1, CFG_REG_A, 1, &rst, 1, HAL_MAX_DELAY);
	    HAL_Delay(100); // 리셋 대기

	    // 3. 리셋 완료 대기
	    do {
	        HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR << 1, CFG_REG_A, 1, &check_reg, 1, HAL_MAX_DELAY);
	        HAL_Delay(10);
	    } while (check_reg & 0x20); // SOFT_RST 비트가 0이 될 때까지 대기

	    // 4. CFG_REG_A 설정 (ODR, MODE, TEMP_EN)
	    HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_ADDR << 1, CFG_REG_A, 1, &cfg_a, 1, HAL_MAX_DELAY);

	    // 5. CFG_REG_C 설정 (DRDY 핀 출력 설정)
	    HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_ADDR << 1, CFG_REG_C, 1, &cfg_c, 1, HAL_MAX_DELAY);

	    // 상태 레지스터 확인 및 출력
	    HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR << 1, CFG_REG_A, 1, cfg_regs, 3, HAL_MAX_DELAY);
	    printf("[CFG] REG_A: 0x%02X, REG_B: 0x%02X, REG_C: 0x%02X\r\n",
	        cfg_regs[0], cfg_regs[1], cfg_regs[2]);

	    HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR << 1, STATUS_REG, 1, &status, 1, HAL_MAX_DELAY);
	    printf("[STATUS] Magnetic sensor STATUS_REG: 0x%02X\r\n", status);

	    // DRDY 핀 활성화 상태 확인
	    if (!(cfg_regs[2] & 0x80))
	    {
	        Magnetic_DRDY();  // 필요 시 추가 동작
	    }

}
/*void Magnetic_Sensor_Set(uint8_t Set)
{
	uint8_t Set;

}*/

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
		printf("[DRDY set] REG_A: 0x%02X, REG_B: 0x%02X, REG_C: 0x%02X\r\n",
				crt_regA_state[0], crt_regA_state[1], crt_regA_state[2]);



}

void Magnetic_Sensor_Read()
{
	static int16_t prevX = 0, prevY = 0, prevZ = 0;
	static TickType_t prevTick = 0;
	const TickType_t tickNow = xTaskGetTickCount();

	uint8_t status = 0;
	uint8_t Read_Buff[6];
	memset(&Read_Buff, 0x00, sizeof(Read_Buff));

	if(ls.magnetic.isconnect)
	{
		/*//1. STATUS_REG 확인
		HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR << 1, STATUS_REG, 1, &status, 1, HAL_MAX_DELAY);

		//2. 새로운 데이터가 있는 경우만 처리 (ZYXDA == 1)
		 if (status & 0x08)
		    {*/

			 	 	HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, OUTX_L_REG, 1, Read_Buff, 6, HAL_MAX_DELAY);

			 		ls.magnetic.mag_x = (int16_t)(Read_Buff[1] << 8 | Read_Buff[0]);
			 		ls.magnetic.mag_y = (int16_t)(Read_Buff[3] << 8 | Read_Buff[2]);
			 		ls.magnetic.mag_z = (int16_t)(Read_Buff[5] << 8 | Read_Buff[4]);
			 		if(ls.magnetic.mag_x != prevX || ls.magnetic.mag_y != prevY || ls.magnetic.mag_z != prevZ)
			 		{
			 			TickType_t interval = tickNow - prevTick; //delay 측정
			 			prevTick = tickNow;

			 			prevX = ls.magnetic.mag_x;
			 			prevY = ls.magnetic.mag_y;
			 			prevZ = ls.magnetic.mag_z;
			 			float heading = atan2((float)ls.magnetic.mag_y, (float)ls.magnetic.mag_x) * (180.0f / 3.14159265f);

			 			if (heading < 0)
			 			{
			 				heading += 360.0f;
			 			}

			 			printf("[logic] ΔT = %lu ms, Magnetic mag_x : %d, mag_y : %d, mag_z : %d, heading : %f\r\n",interval, ls.magnetic.mag_x, ls.magnetic.mag_y, ls.magnetic.mag_z, heading);
			 			Send_Magnetic_data(Read_Buff);
			 		}

		    //}

	}
	else
	{
		printf("[logic] Magnetic Sensor Disconnect....!! \r\n");
	}

}



void Send_Magnetic_data(uint8_t *data)
{
	//printf("[LOGIC] Send_Magnetic_datat\r\n");
	uint8_t bp = 0;
	uint16_t calc_lrc = 0;

	memset((uint8_t*) &send_buf, 0, sizeof(send_buf));

	send_buf[bp++] = 0x7E;			//STX
	send_buf[bp++] = 0x09;			//Length~LRC Size
	send_buf[bp++] = RF_CMD_SEND;			//OP-Code(Modem Send Opcode)

	send_buf[bp++] = data[0];
	send_buf[bp++] = data[1];
	send_buf[bp++] = data[2];
	send_buf[bp++] = data[3];
	send_buf[bp++] = data[4];
	send_buf[bp++] = data[5];

	send_buf[bp++] = 0xFF; 	//CRC;
	send_buf[bp++] = 0x7F;

	if (ls.RfModule.isModemRdy == pdTRUE)
		commSendData(SENSOR_UART_PORT, send_buf, bp);
}

void LIS2MDL_SelfCheck() {
    uint8_t whoami = 0;
    uint8_t status = 0;
    uint8_t cfg_a = 0x80;  // TEMP_EN=1, ODR=10Hz, Continuous mode
    uint8_t cfg_c = 0x01;  // DRDY_on_PIN=1
    uint8_t rst = 0x20;    // SOFT_RST
    uint8_t data[6];


    // 5. STATUS_REG 확인 및 데이터 읽기
    HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR << 1, STATUS_REG, 1, &status, 1, HAL_MAX_DELAY);
    printf("STATUS_REG = 0x%02X\r\n", status);

    if (status & 0x08) {  // ZYXDA 비트 확인
        HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR << 1, OUTX_L_REG, 1, data, 6, HAL_MAX_DELAY);
        int16_t mx = (int16_t)(data[1] << 8 | data[0]);
        int16_t my = (int16_t)(data[3] << 8 | data[2]);
        int16_t mz = (int16_t)(data[5] << 8 | data[4]);
        printf(" Magnetic Data: X=%d, Y=%d, Z=%d\r\n", mx, my, mz);
    } else {
        printf(" No new magnetic data available yet.\r\n");
    }

    // 6. DRDY 상태 직접 polling
    for (int i = 0; i < 10; i++) {
        GPIO_PinState drdy = HAL_GPIO_ReadPin(DRDY_INPUT_GPIO_Port, DRDY_INPUT_Pin);
        printf("DRDY = %s\r\n", (drdy == GPIO_PIN_RESET) ? "LOW" : "HIGH");
        HAL_Delay(10);  // 10ms 간격
    }
}



void logicMagneticTask()
{
	/*static GPIO_PinState prevDrdyState = GPIO_PIN_SET;
	    GPIO_PinState currDrdyState = HAL_GPIO_ReadPin(DRDY_INPUT_GPIO_Port, DRDY_INPUT_Pin);

	    // DRDY rising edge 감지 (LOW → HIGH)
	    if (prevDrdyState == GPIO_PIN_RESET && currDrdyState == GPIO_PIN_SET) {
	        Magnetic_Sensor_Read();  // DRDY HIGH 시 센서 읽기
	    }

	    prevDrdyState = currDrdyState;*/
	//Magnetic_Sensor_Read();
}
