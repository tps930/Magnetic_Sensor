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
#include "cJSON.h"

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
	ls.magnetic.z_change = pdFALSE;
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
			//Send_Modem_Standby(ls.stat.SectionID);

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
			//Send_Modem_Standby(ls.stat.SectionID);
		}
	}
}



void logicIdleTask()
{
    HAL_IWDG_Refresh(&hiwdg);
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




void logicUartTask()
{
	Sensor_Receive_Check();
	Gps_Receive_Check();

	const TickType_t tickNow = xTaskGetTickCount();

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

					break;
				case OP_GPSDATA_SEND:		//SensorData
					break;
			}
		}
		event = osMessageGet(SensorQueueHandle, 0);
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
TickType_t master_tick;
TickType_t slave_tick;
void logicDRMTask()
{
	const TickType_t tickNow = xTaskGetTickCount();
	RF_Receive_Check();

	osEvent event = osMessageGet(RFQueueHandle, 0);
	while (event.status == osEventMessage) {
		COMMAND_T Packet;

		memcpy(&Packet, (COMMAND_T*) event.value.v, sizeof(COMMAND_T));

		//printf("\r\n[Logic] Modem OP_CODE [%d][%2X]\r\n", Packet.groupIndex, Packet.opcode);

		//Packet.groupIndex = Find_GroupIndex(Packet.groupID);

		ls.RfModule.isRecv = pdTRUE;

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
	    printf("[logic] REG_A: 0x%02X, REG_B: 0x%02X, REG_C: 0x%02X\r\n",
	    		crt_regA_state[0], crt_regA_state[1], crt_regA_state[2]);

}

void Magnetic_Sensor_Read_stsus_check()
{
	static int16_t prevX = 0, prevY = 0, prevZ = 0;
	static TickType_t prevTick = 0;
	const TickType_t tickNow = xTaskGetTickCount();

	uint8_t status = 0;
	uint8_t Read_Buff[6];
	memset(&Read_Buff, 0x00, sizeof(Read_Buff));

	if(ls.magnetic.isconnect)
	{
		//1. STATUS_REG 확인
		HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR << 1, STATUS_REG, 1, &status, 1, HAL_MAX_DELAY);

		//2. 새로운 데이터가 있는 경우만 처리 (ZYXDA == 1)
		 if (status & 0x08)
		    {

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

		    }

	}
	else
	{
		printf("[logic] Magnetic Sensor Disconnect....!! \r\n");
	}

}

void Magnetic_Sensor_Read()
{
		static int16_t prevX = 0, prevY = 0, prevZ = 0;


		uint8_t status = 0;
		uint8_t Read_Buff[6];
		memset(&Read_Buff, 0x00, sizeof(Read_Buff));

		if(ls.magnetic.isconnect)
		{

			HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, OUTX_L_REG, 1, Read_Buff, 6, HAL_MAX_DELAY);


			ls.magnetic.mag_x = (int16_t)(Read_Buff[1] << 8 | Read_Buff[0]);
			ls.magnetic.mag_y = (int16_t)(Read_Buff[3] << 8 | Read_Buff[2]);
			ls.magnetic.mag_z = (int16_t)(Read_Buff[5] << 8 | Read_Buff[4]);


			//Send_Magnetic_data(Read_Buff);
			//Magnetic_Sensor_Cal();
			Traffic_Cal_Min(ls.magnetic.mag_z);
		}
		else
		{
			printf("[logic] Magnetic Sensor Disconnect ....!! \r\n");
		}
}

void Magnetic_Sensor_Cal()
{

	static TickType_t prevTick = 0;
	const TickType_t tickNow = xTaskGetTickCount();
	TickType_t interval = tickNow - prevTick; //delay 측정
	prevTick = tickNow;

	float heading = atan2((float)ls.magnetic.mag_y, (float)ls.magnetic.mag_x) * (180.0f / 3.14159265f);

	if (heading < 0)
	{
		heading += 360.0f;
	}

	printf("[logic] ΔT = %lu ms, Magnetic mag_x : %d, mag_y : %d, mag_z : %d, heading : %f\r\n",interval, ls.magnetic.mag_x, ls.magnetic.mag_y, ls.magnetic.mag_z, heading);

}

int16_t prev_z=0;

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

		printf("[logic] Magnetic count AVG [%d] \r\n", ls.magnetic.traffic_avg_60sec);
	}


}

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


	commSendData(RF_UART_PORT, send_buf, bp);
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

	commSendData(RF_UART_PORT, send_buf, bp);

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

	commSendData(RF_UART_PORT, send_buf, bp);

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

	commSendData(RF_UART_PORT, send_buf, bp);

	printf("[logic]  Magentic_Time_Ack \r\n");
}

void Send_Magnetic_Time()
{
	HAL_RTC_GetTime(&hrtc, &ls.gps.sTime, FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &ls.gps.sDate, FORMAT_BIN);

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

	commSendData(RF_UART_PORT, send_buf, bp);

	printf("[logic]  Magentic_Time_Send local[%d] offset[%d] send[%d] \r\n",local_tick, ls.magnetic.Recv_time_offset, aligned_tick);
}
void Send_Magnetic_Time_Recv(uint8_t *data)
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

	commSendData(RF_UART_PORT, send_buf, bp);

	printf("[logic]  Magentic_Time_Recv \r\n");
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

	commSendData(RF_UART_PORT, send_buf, bp);
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
	const TickType_t tickNow = xTaskGetTickCount();

	if(ls.stat.isTestMode)
		logicMasterMode();
	else
		logicSlaveMode();
	if (CalcTickDiff(ls.tick.Proc, tickNow) >= 5000)
		{
			ls.tick.Proc = tickNow;
			SendSensorDataJson();
		}
}

void logicMasterMode()
{
	const TickType_t tickNow = xTaskGetTickCount();
	if (HAL_GPIO_ReadPin(DRDY_INPUT_GPIO_Port, DRDY_INPUT_Pin) == GPIO_PIN_SET)
	{
		     Magnetic_Sensor_Read();
	}

}

void logicSlaveMode()
{
	const TickType_t tickNow = xTaskGetTickCount();


	if(ls.Master_sensor.Connect)
	{
		if (HAL_GPIO_ReadPin(DRDY_INPUT_GPIO_Port, DRDY_INPUT_Pin) == GPIO_PIN_SET)
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

void AddFixed1Decimal(cJSON *object, const char *key, float value)
{
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%.1f", value);  // 소수점 1자리
    cJSON_AddStringToObject(object, key, buffer);     // 문자열로 추가
}

void SendSensorDataJson(void)
{
    // 최상위 JSON 객체
    cJSON *root = cJSON_CreateObject();

    // traffic_sensor 객체 생성
    cJSON *traffic = cJSON_CreateObject();
    cJSON_AddNumberToObject(traffic, "traffic-counts", ls.magnetic.traffic_avg_60sec);
    cJSON_AddNumberToObject(traffic, "speed", ls.magnetic.Speed);
    AddFixed1Decimal(traffic, "charging", 45.6);
    cJSON_AddItemToObject(root, "traffic_sensor", traffic);

    // surface_sensor 객체 생성
    cJSON *surface = cJSON_CreateObject();
    AddFixed1Decimal(surface, "surface-temp", 12.3);
    AddFixed1Decimal(surface, "freezing-temp", -3.4);
    AddFixed1Decimal(surface, "salt-concentration", 7.8);
    cJSON_AddNumberToObject(surface, "road-condition", 1);
    AddFixed1Decimal(surface, "water-level", 45.7);
    AddFixed1Decimal(surface, "charging", 33.2);
    cJSON_AddItemToObject(root, "surface_sensor", surface);

    // JSON 문자열 생성 (최소화된 형태)
    char *jsonStr = cJSON_PrintUnformatted(root);

    // UART 전송
/*
    HAL_UART_Transmit(&huart1, (uint8_t *)jsonStr, strlen(jsonStr), HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart1, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY); // 구분자
*/
    printf("%s\r\n", jsonStr);
    // 메모리 해제
    cJSON_Delete(root);
    free(jsonStr);
}
