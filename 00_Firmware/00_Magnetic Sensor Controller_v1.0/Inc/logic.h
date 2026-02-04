/*
 * logic.h
 *
 *  Created on: 2019. 05. 03.
 *      Author: Shin Seung Cheol
 */

#ifndef LOGIC_H_
#define LOGIC_H_

#include "global.h"

#define TEST_DELAY_LONG     800UL
#define TEST_DELAY_SHORT    400UL

#define TEST_BRIGHT_100     255UL
#define TEST_BRIGHT_80      204UL
#define TEST_BRIGHT_50      128UL
#define TEST_BRIGHT_40      100UL
#define TEST_BRIGHT_20      50UL

#define INPUT_TESTMODE_MASK 			0x80
#define INPUT_TESTCONTRL_MODE_MASK 		0x40
#define INPUT_TESTCONTRL_NO_MASK 		0x20
#define INPUT_DISPLAY_RESET_MASK 		0x10

#define INPUT_SECTION_MASK 				0x08
#define INPUT_JAM_MASK	 				0x04
#define INPUT_TEST_PACESPEED_MASK		0x03


#define SetIoTWakeUp(state)   HAL_GPIO_WritePin(IOT_WAKEUP_GPIO_Port, IOT_WAKEUP_Pin, state)

#define COMM_TIMEOUT        		(1000*6)

#define TRAFFIC_JAM_ENABLE_RADAR		5
#define TRAFFIC_JAM_DISABLE_RADAR		5

#define TRAFFIC_JAM_ENABLE_SONAR		5
#define TRAFFIC_JAM_DISABLE_SONAR		50


#define CDS_MIN		 		(0.2 * CDS_MAX)
#define CDS_MAX				800

#define DEFAULT_BRIGHT      70
#define MIN_BRIGHT      	20
#define FULL_BRIGHT      	100

#define MAX_BRIGHT_STEP     255
#define MIN_BRIGHT_STEP     20

#define NOISE_MIN		 		30
#define NOISE_MAX				120

#define DRM_CMD_REG_REQ 	0x01
#define DRM_CMD_REG_RSP		0x02
#define DRM_CMD_GRP_CTL		0x10
#define DRM_CMD_ONE_CTL		0x11
#define DRM_CMD_ALL_CTL		0x12
#define DRM_CMD_ST_REQ		0x20
#define DRM_CMD_ST_RSP		0x21
#define DRM_CMD_RESET		0x30
#define DRM_CMD_MODE_SET	0x31
#define DRM_CMD_DISP_SET	0x32
#define DRM_CMD_DISP_OFF	0x33

#define RF_CMD_BOOT_MSG		0xB0
#define RF_CMD_REG_STATUS	0xC0
#define RF_CMD_REG_WRITE	0xC1
#define RF_CMD_RECV 		0xD0
#define RF_CMD_SEND 		0xD1

#define CENTER_CMD_REG_REQ		0x01
#define CENTER_CMD_REG_RSP		0x02
#define CENTER_CMD_MODEM_SET	0x03
#define CENTER_CMD_GRP_CTL		0x10
#define CENTER_CMD_ONE_CTL		0x11
#define CENTER_CMD_ALL_CTL		0x12
#define CENTER_CMD_ST_REQ		0x20
#define CENTER_CMD_ST_RSP		0x21
#define CENTER_CMD_RESET		0x30
#define CENTER_CMD_MODE_SET		0x31

#define PROTOCOL_VERSION_MAJOR	1
#define PROTOCOL_VERSION_MINOR	0

#define UINT8_LENGTH				1
#define UINT16_LENGTH				2
#define UINT32_LENGTH				4

#define TYPE_PROTOCAL_VERSION			0x01
#define TYPE_BATTERY					0x02
#define TYPE_SENSING					0x03
#define TYPE_LORA_RSSI					0x04
#define TYPE_CONTROL_RESPONSE			0x07
#define TYPE_REBOOTING					0x08
#define TYPE_TRANSMISSSION_DURATION		0x10

#define TYPE_OPERATION_STATUS			0x30
#define TYPE_VEHICLE_SPEED				0x31
#define TYPE_START_SPEED				0x32
#define TYPE_END_SPEED					0x33
#define TYPE_SENSOR_STATUS				0x34
#define TYPE_PACE_SPEED					0x35
#define TYPE_OPERAT_MODE				0x36

#define TYPE_SENSING_CYCLE				0x01
#define TYPE_SENSING_EVENT				0x02

#define OP_SENSORDATA_SEND	0x21
#define OP_GPSDATA_SEND		0x22

#define ADC_CHANNEL         2
#define ADC_VACC      		3.3
#define ADC_RESOLUTION      1024

//megnetic_Reg
#define LIS2MDL_ADDR  		0x1E

#define OFFSET_X_REG_L		0x45
#define OFFSET_X_REG_H		0x46
#define OFFSET_Y_REG_L		0x47
#define OFFSET_Y_REG_H		0x48
#define OFFSET_Z_REG_L		0x49
#define OFFSET_Z_REG_H		0x4A

#define WHO_AM_I			0x4F

#define CFG_REG_A			0x60
#define	CFG_REG_B			0x61
#define	CFG_REG_C			0x62

#define INT_CRTL_REG		0x63
#define INT_SOURCE_REG		0x64
#define	INT_THS_L_REG		0x65
#define INT_THS_H_REG		0x66

#define STATUS_REG			0x67

#define OUTX_L_REG			0x68
#define	OUTX_H_REG			0x69
#define OUTY_L_REG			0x6A
#define OUTY_H_REG			0x6B
#define OUTZ_L_REG			0x6C
#define OUTZ_H_REG			0x6D
#define	TEMP_OUT_L_REG		0x6E
#define TEMP_OUT_H_REG		0x6F


typedef struct
{
	uint8_t 	Init;
	uint8_t 	Last;
} DIPSW_T;

typedef struct
{
	uint8_t 	Init;
	uint8_t 	Last;
} GPINPUT_T;

typedef struct
{
    uint8_t 	state;
    uint16_t 	value;
    uint16_t 	average[10];	//10회 평균
    uint16_t	avg_value;

    uint32_t	timeoutADCTicks;

    uint8_t		Brt;
} CDS_t;

typedef struct {
	uint16_t 		Value;
	uint8_t 		Stat;
	uint8_t			Channel;
	uint16_t		Adc_Array[10];
	uint16_t		Adc_Sum;
	uint16_t		Adc_Avg;
	uint32_t		adc_dma[2];
} ADC_T;

typedef struct
{
	uint8_t 	Stat;
	uint8_t 	isCall;
	uint8_t 	isSubCall;
	uint8_t 	isComplete;
	uint8_t		Hour_KST;
	uint8_t		Min;
	uint8_t		Sec;
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
} GPS_T;

typedef struct
{
    uint8_t 	Prog;				//PERI_LAMP1
    uint8_t 	Center;				//PERI_LAMP2
    uint8_t 	Section[2];				//PERI_LAMP3~4
    uint8_t 	Sensor;
} INDICATOR_T;

typedef struct
{
	uint32_t	Comm;
	uint32_t	Section[2];
	uint32_t	Sensor;
	uint32_t 	Indi;
	uint32_t 	Proc;
	uint32_t	IoT;
} TIMETICK_T;

typedef struct
{
	uint8_t 	IoT_Stat;
	uint8_t 	isJoin;
	uint8_t 	isReady;
	uint8_t 	isPower;
	uint8_t 	WakeupCall;
	uint8_t 	isComplete;
	uint8_t 	isCall;
	uint8_t 	isSendCompleate;
	uint8_t 	isRecv;
	uint8_t		isSubCall;
	uint8_t 	isLineCheck;
} IOT_MODEM_T;


typedef struct
{
	uint8_t		isTestMode;
	uint8_t		isStatusCall;

	uint8_t		TestPaceSpeed;
	uint8_t		TestMode;
	uint8_t		ControlMode;
	uint8_t		TestNo;
	uint8_t		Jam;
	uint8_t		SectionID;
	uint8_t		Battery;
} STATE_T;

typedef struct
{
    uint8_t			Status_Control;
	uint8_t			SectionID;
    uint8_t			DeviceID;
    uint8_t 		Rf_ch;
    uint8_t 		Tx_power;
    uint8_t 		Tx_rssi;
    uint8_t 		Dbl_send;
    uint8_t 		Led_Timeout;
    uint8_t 		Reserved;
    uint8_t 		F_Resister;

	uint8_t		isModemRdy;
	uint8_t		isModemRdyCall;
	uint8_t		isModemRdyCount;

	uint8_t 	isRecv;
} MODEM_T;

typedef struct
{
	uint8_t groupID;
	uint8_t groupIndex;
	uint8_t deviceID;
	uint8_t deviceIndex;
	uint8_t opcode;
	uint8_t subcode;
	uint8_t datalen;
    uint8_t 	size;
	uint8_t data[256];

	uint8_t Result;
} COMMAND_T;

typedef struct {
	uint8_t		Init;				//0 : NotConnect, 1 : Connect OK
	uint8_t 	DeviceID;
	uint8_t 	SensorValue;		//0 : Normal, 1 : TrafficJam
	uint8_t 	SensorCount;		//Disable Count
	uint8_t 	SensorReason;		//0 : Normal, 1 : radar, 2 : sonar
	uint8_t 	VehicleSpeed;		//vehicle Average Speed for 1sec
	uint8_t 	VehicleSpeed_Max;		//vehicle Max Speed for 1sec
	uint8_t		BatteryLevel;
	uint8_t		SensorStatus;

	uint8_t		CommandResult;		//Command Result( 0 : false, 1 : ok , 2 : Request, 3: Send ok)
	uint16_t 	timeout;			//Sensor Time Out Count
} SENSOR_t;

typedef struct
{
	uint32_t	Duration;		//전송주기
	uint8_t		Mode;			//동작모드
	uint8_t		PaceSpeed;
	uint8_t		StartSpeed;
	uint8_t		EndSpeed;
} Config_t;

typedef struct
{
	int16_t 	mag_x;
	int16_t 	mag_y;
	int16_t 	mag_z;
	uint8_t		isconnect;
} Magnetic_t;

typedef struct
{
	STATE_T			stat;
	Config_t		config;
	Config_t		temp_config;
	DIPSW_T			dip;
	GPINPUT_T		gpin;
    TIMETICK_T		tick;
    INDICATOR_T		indi;
    MODEM_T			RfModule;
    ADC_T			adc;
    CDS_t			cds;
	GPS_T			gps;
    SENSOR_t 		new_sensor;
    SENSOR_t 		old_sensor;
    IOT_MODEM_T		iot;
    Magnetic_t		magnetic;
	uint8_t			JamCriteria;		//정체 기준 속도 40 or 50
	uint8_t			SendOn;
	uint8_t			NowPaceSpeed;
	uint8_t			NextPaceSpeed;
	uint8_t			Bright;
    uint16_t		OnCount;
    uint16_t		SendCount;
    uint16_t		TestSendCount;
    uint16_t		ChangeOnCount;
    uint16_t		ChangeOffCount;
    uint8_t			OnDeviceID;
    uint8_t			DisplayOffSend;
    uint8_t			DisplayOnSend;
    uint8_t			logEnable;

	uint8_t			isReboot;
} LogicStatus_T;

static LogicStatus_T ls;

uint8_t send_buf[256];

void logicInit();
void logicLampTask();
void logicInputTask();
void logicDisplayTask();
void logicLaserTask();
void logicSoundTask();
void logicIdleTask();
void logicUartTask();
void logicDRMTask();
static void ProcTestMode();
static void ProcWorkMode();
static void ProcManualMode();
void Set_IoT_Joined(uint8_t v);
uint8_t DRM_Register(uint8_t Section, uint8_t DeviceID);
uint8_t Find_DeviceIndex(uint8_t Section, uint8_t DeviceID);
uint8_t Find_GroupIndex(uint8_t GroupID);
void DB_Read();
void DB_Save();
void Send_SensorValue();
void logicMagneticTask();
#endif /* LOGIC_H_ */

