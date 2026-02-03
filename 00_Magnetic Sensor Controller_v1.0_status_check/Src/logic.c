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
#include <math.h>

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
    uint8_t who_am_i = 0;
    uint8_t check_reg = 0;
    uint8_t status = 0;

    HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, WHO_AM_I, 1, &who_am_i, 1, HAL_MAX_DELAY);
    if (who_am_i != 0x40) {
        printf("[ERROR] LIS2MDL not detected. WHO_AM_I=0x%02X\r\n", who_am_i);
        ls.magnetic.isconnect = pdFALSE;
        return;
    }
    printf("[OK] LIS2MDL connected. WHO_AM_I=0x%02X\r\n", who_am_i);
    ls.magnetic.isconnect = pdTRUE;

    /* SOFT_RST */
    uint8_t rst = 0x20;
    HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_ADDR<<1, CFG_REG_A, 1, &rst, 1, HAL_MAX_DELAY);
    HAL_Delay(100);
    do {
        HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, CFG_REG_A, 1, &check_reg, 1, HAL_MAX_DELAY);
        HAL_Delay(10);
    } while (check_reg & 0x20);

    /* CFG_REG_A: TEMP_EN=1, ODR=10 Hz, continuous -> 0x80 */
    uint8_t cfg_a = 0x80;
    HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_ADDR<<1, CFG_REG_A, 1, &cfg_a, 1, HAL_MAX_DELAY);

    /* CFG_REG_C: BDU(bit4)=1, DRDY_on_pin(bit0)=1 -> 0x11 */
    uint8_t cfg_c = 0x11;
    HAL_I2C_Mem_Write(&hi2c1, LIS2MDL_ADDR<<1, CFG_REG_C, 1, &cfg_c, 1, HAL_MAX_DELAY);

    /* Read-back */
    uint8_t cfg_regs[3] = {0};
    HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, CFG_REG_A, 1, cfg_regs, 3, HAL_MAX_DELAY);
    printf("[CFG] REG_A=0x%02X REG_B=0x%02X REG_C=0x%02X\r\n",
           cfg_regs[0], cfg_regs[1], cfg_regs[2]);

    HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, STATUS_REG, 1, &status, 1, HAL_MAX_DELAY);
    printf("[STATUS] STATUS_REG=0x%02X\r\n", status);

    /* Calibration on |B| */
    int16_t mbuf[CALIB_SAMPLES];
    int count = 0;
    uint8_t rb[6] = {0};
    uint32_t t0_all = HAL_GetTick();

    while (count < CALIB_SAMPLES) {
        HAL_IWDG_Refresh(&hiwdg);
        if (HAL_GPIO_ReadPin(DRDY_INPUT_GPIO_Port, DRDY_INPUT_Pin) == GPIO_PIN_SET) {
            HAL_I2C_Mem_Read(&hi2c1, LIS2MDL_ADDR<<1, OUTX_L_REG, 1, rb, 6, HAL_MAX_DELAY);
            int16_t x = (int16_t)(rb[1]<<8 | rb[0]);
            int16_t y = (int16_t)(rb[3]<<8 | rb[2]);
            int16_t z = (int16_t)(rb[5]<<8 | rb[4]);
            mbuf[count++] = calc_bmag_i16(x, y, z);

            /* wait for DRDY to go low (max ~50 ms) */
            uint32_t t0 = HAL_GetTick();
            while (HAL_GPIO_ReadPin(DRDY_INPUT_GPIO_Port, DRDY_INPUT_Pin) == GPIO_PIN_SET) {
                HAL_IWDG_Refresh(&hiwdg);
                if (HAL_GetTick() - t0 > 50) break;
                HAL_Delay(1);
            }
        } else {
            HAL_Delay(1);
        }
        if (HAL_GetTick() - t0_all > 5000) break; /* safety timeout */
    }

    MagProc_Calibrate_from_buf(mbuf, count > 0 ? count : 1);
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

    printf("[MAG] Calib |B|: M0=%ld, sigma=%.2f, T=%d\r\n",
           (long)ls.magnetic.Z0, ls.magnetic.sigma, ls.magnetic.T);
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

            printf("[logic] dT=%lu ms, X=%d Y=%d Z=%d |B|=%d, heading=%.1f\r\n",
                   interval, prevX, prevY, prevZ, bmag, heading);
            Send_Magnetic_data();
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

    int16_t bmag = calc_bmag_i16(ls.magnetic.mag_x, ls.magnetic.mag_y, ls.magnetic.mag_z);
    Magnetic_Sensor_Cal();
    /* do not use Z-only path anymore */
    MagProc_Process_sample(bmag, ls.magnetic.Recv_traffic_time /* or 0 */);
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

	Send_Magnetic_data();
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
        if (abs_rel >= ls.magnetic.T) {
            ls.magnetic.st = WAIT_RISE; /* in-event */
            ls.magnetic.debounce_until = t + pdMS_TO_TICKS(DEBOUNCE_MS);
        }
        break;

    case WAIT_RISE: /* in-event */
        if (abs_rel < ls.magnetic.T) {
            ls.magnetic.st = WAIT_FALL; /* back to idle */
            ls.magnetic.traffic_count++;
            ls.magnetic.refractory_until = t + pdMS_TO_TICKS(REFRACT_MS);

            /* optional speed from peer time stamp */
            if (peer_tick_or_0 != 0 && Sensor_Distance > 0.0f) {
                TickType_t dt = t - peer_tick_or_0; /* wrap safe with TickType_t */
                float sec = (float)dt * (portTICK_PERIOD_MS / 1000.0f);
                if (sec > 0.02f) {
                    ls.magnetic.Speed = (Sensor_Distance / sec) * 3.6f;
                }
            }

            if (ls.stat.isTestMode) {
                ls.magnetic.Recv_traffic_time = 0;
            } else {
                Send_Magnetic_Time();
                ls.Master_sensor.send_flag = pdFALSE;
            }
        }
        break;
    }

    Send_Magnetic_data();
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

	commSendData(IOT_UART_PORT, send_buf, bp);
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


	if(ls.stat.isTestMode)
		logicMasterMode();
	else
		logicSlaveMode();

	Magnetic_Avg_1s_Task();
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

