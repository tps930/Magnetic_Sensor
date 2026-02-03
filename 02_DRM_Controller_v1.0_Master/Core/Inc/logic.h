/*
 * logic.h
 *
 *  Created on: 2019. 9. 3.
 *      Author: S.S.C
 */

#ifndef LOGIC_H_
#define LOGIC_H_
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

#define TEST_DELAY_LONG     800UL
#define TEST_DELAY_SHORT    400UL

#define TEST_BRIGHT_MAX     0UL
#define TEST_BRIGHT_ALL     60UL
#define TEST_BRIGHT_W1      30UL
#define TEST_BRIGHT_W2      128UL
#define TEST_BRIGHT_W3      192UL

#define RF_CMD_BOOT_MSG		0xB0
#define RF_CMD_REG_STATUS	0xC0
#define RF_CMD_REG_WRITE	0xC1
#define RF_CMD_RECV 		0xD0
#define RF_CMD_SEND 		0xD1

#define DRM_CMD_REG_REQ 	0x01
#define DRM_CMD_REG_RSP		0x02
#define DRM_BLE_NAME_REQ    0x04 //BLE_NAME
#define DRM_BLE_NAME_RSP    0x05 //BLE_ADVDATA
#define DRM_BLE_NAME_SET    0x06 //BLE_RATE
#define DRM_BLE_ADVDATA_REQ 0x07
#define DRM_BLE_ADVDATA_RSP 0x08
#define DRM_BLE_ADVDATA_SET 0x09
#define DRM_BLE_ADVINTERVAL_SET 0x0A
#define DRM_BLE_ON_OFF 		0x0B
#define DRM_BLE_STANDBY		0x0C
#define DRM_BLE_SLEEP		0x0D

#define DRM_CMD_GRP_CTL		0x10
#define DRM_CMD_ONE_CTL		0x11
#define DRM_CMD_ALL_CTL		0x12
#define DRM_CMD_ALL_CTL_V2	0x14		//MAX DRM(25EA) DRM 확장 대응(20->25)
#define DRM_CMD_ALL_CTL_V3	0x17		//RF Module Change 대응(기존 128Byte -> 38Byte 축소)

//#define DRM_CMD_ALL_BLE		0x13
#define DRM_CMD_EVENT_CTL	0x13		//상황정보 제어
#define DRM_CMD_EVENT_GRP_CTL	0x15		//이벤트 그룹 On/Off 제어

#define DRM_CMD_BLE_CTL		0x40		//BLE
#define DRM_CMD_BLE_ADV		0x41		//BLE ADV Interval

#define DRM_CMD_ST_REQ		0x20
#define DRM_CMD_ST_RSP		0x21
#define DRM_CMD_RESET		0x30
#define DRM_CMD_MODE_SET	0x31
#define DRM_CMD_DISP_SET	0x32
#define DRM_CMD_INIT_SET	0x33	// 셋팅 정보
#define DRM_CMD_EVENT_SET	0x34	// 상황 셋팅 정보
#define DRM_CMD_INIT_SET_V2	0x35	// 셋팅 정보 V2 - RF Module Change 대응(기존 128Byte -> 38Byte 축소)
#define DRM_CMD_EVENT_SET_V2	0x36	// 상황 셋팅 정보  V2 - RF Module Change 대응(기존 128Byte -> 38Byte 축소)

#define FIRMWARE_CMD_REG_REQ	 0xB0
#define FIRMWARE_CMD_REG_RSP	 0xB1
#define FIRMWARE_CMD_REG_SET	 0xB2
#define FIRMWARE_BLE_NAME_REQ    0xB3
#define FIRMWARE_BLE_NAME_RSP    0xB4
#define FIRMWARE_BLE_NAME_SET    0xB5
#define FIRMWARE_BLE_ADVDATA_REQ 0xB6
#define FIRMWARE_BLE_ADVDATA_RSP 0xB7
#define FIRMWARE_BLE_ADVDATA_SET 0xB8
#define FIRMWARE_CMD_MODE		 0xC0
#define FIRMWARE_CMD_RESET		 0xA5

#define BUFF_LEN       		256

#define SetFRAM_WP(state)   	HAL_GPIO_WritePin(I2C1_WP_GPIO_Port, I2C1_WP_Pin, state)

#define TEMP_ZERO		3142
#define TEMP_MINUS_30	3867
#define TEMP_99			238

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

#define Time_REQ			0xA1
#define Time_ACk			0xA2

#define Magnetic_Time_Send	0xB1
#define Magnetic_Time_Recv	0xB2

#define Senser_Connect_REQ	0xC1
#define Senser_Connect_ACK	0xC2

#define Sensor_Distance 	5


typedef struct
{
    uint8_t red;
    uint8_t grn;
    uint8_t blu;
    uint8_t ir;
} LEDDATA_T;

typedef struct
{
    uint8_t 	opcode;
    uint8_t 	subcode;
    uint8_t 	section_id;
    uint8_t 	device_id;
    uint8_t 	datalen;
    uint8_t 	data[128];

    uint8_t		Result;
} COMMAND_T;

typedef struct
{
    uint8_t 		Program;				//PERI_LAMP1
    uint8_t 		OnCount;				//PERI_LAMP1
} INDICATOR_T;

/*typedef struct {
	uint32_t 		Value[4];
	uint8_t 		Stat;
	uint8_t			Count;
	uint32_t		Cds_Array[4][10];
	uint32_t		Cds_Sum[4];
	uint32_t		Cds_Avg[4];
	uint16_t		Solar;
	double			fSolar;
	uint16_t		Battery;
	double			fBattery;
	int16_t			Temp;
	double			fTemp;
	uint16_t		Brt;
} CDS_T;*/

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
	uint8_t			isTestMode;			//0: WorkingMode, 1:TestMode
	uint8_t			isModemRdy;
	uint8_t			isModemRdyCall;
	uint8_t			isModemRdyCount;
	uint8_t			isRegister;
	uint8_t			isRegisterCall;
	uint8_t			isStatusCall;
	uint8_t			isBLEADVCall;
	uint8_t			isBLEADVUpdate;
	uint8_t			preADVStat;
	uint8_t			isGroupControl;
	uint8_t			isEventControl;
	uint8_t 		isCommInit;
	uint8_t			EventCode;			//event 0:정상, 1:사고, 2:정체, 3:작업, 4:악천후
	uint16_t		EventPosition;		//이벤트 위치
	uint16_t		EventGroupID;			//이벤트 그룹 제어 번호
	uint16_t		EventDistanceGap;		//내 위치와 이벤트위치 차이
	uint16_t		NewEventDistanceGap;		//내 위치와 이벤트위치 차이

	uint8_t			initID;				//내 위치 ID
	uint8_t			istallDistance;		//설치간격
	uint16_t		initPosition;		//내 위치
    uint8_t			isStandByMode;
    uint8_t			RFWakeupCount;
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
    uint8_t         Lane;
	uint8_t         lat_1;
	uint8_t         lat_2;
	uint8_t         lat_3;
	uint8_t         lat_4;
	uint8_t         lon_1;
	uint8_t         lon_2;
	uint8_t         lon_3;
	uint8_t         lon_4;
    uint8_t 		Reserved;
    uint8_t 		F_Resister;
    COMMAND_T		recvData;
    uint8_t			recvFlag;
    uint8_t			isChargingMode;	//0 : WorkingMode, 1 : ChargingMode
    uint8_t			isBLEBaudInit;
} MODEM_T;

typedef struct
{
	uint8_t			Init;
	LEDDATA_T 		Disp_Led;
	LEDDATA_T 		Dummy_Led;

	uint8_t			Mode;
    uint16_t 		Pos;
    uint16_t 		OnOff;
    uint8_t 		Brt;

    uint8_t			DispLens;
    uint8_t			isFlash;
    uint8_t			FlashMode;
    uint16_t		Flashing_InterVal;

    uint8_t			isIR;
    uint8_t			isIRFlash;
    uint16_t 		IRPos;
    uint16_t		IRFlashCount;
    uint8_t			isBLEOnOff;
	uint8_t			situation;
	uint16_t		location;

    uint16_t		FlashCount;
	uint32_t 		Tick;
	uint32_t 		Delay;
} DISPLAY_T;

typedef struct
{
	uint32_t		Tick;
	uint32_t		Comm;
	uint32_t		Indi;
	uint32_t		Cds;
	uint32_t		Proc;
	uint32_t		Send;
	uint32_t		Ble;
	uint32_t		RF;
	uint32_t		traffic_avg_sec;
	uint32_t		Sensor;
	TickType_t 		tickNow;
} TIMETICK_T;

typedef struct
{
	uint8_t 		name[8];
	uint8_t 		data[13];

	uint8_t 		mac_address[6];

	uint8_t			isBaudInit;		//0 : Error, 1: OK
	uint8_t			BaudCheckStep;
	uint8_t			BaudSetCall;
	uint8_t			BaudReady;

	uint8_t			isStatus;		//0 : Error, 1: OK
	uint8_t			InitStep;
    uint8_t			isInit;			//BLE Band Check Flag

    uint8_t			isInfoCall;
    uint8_t			isBaudCheck;	//BLE Band Check Flag
    uint8_t			isSend;			//BLE Band Check Flag
    uint8_t			isSetBaud;	//BLE Band Check Flag
    uint8_t			SendCount;
    uint8_t			isRecv;			//BLE Recv Check Flag
    uint8_t			isBaudIndex;	//BLE Band Check Index
    uint8_t			isRecvTimeout;	//BLE Recv Check Flag
	uint8_t			isConnected;
	//uint8_t			isADVOnOff;
	uint16_t		isADVInterval;
	uint8_t			isMode;

	char 			command_str[256];
	COMMAND_T		recvData;
	uint8_t			recvFlag;
}BLE_T;

typedef enum { WAIT_FALL = 0, WAIT_RISE } MagState;

typedef struct
{
	int16_t 		mag_x;
	int16_t 		mag_y;
	int16_t 		mag_z;
	uint8_t			isconnect;
	uint8_t			z_change;
	uint16_t    	traffic_count;
	uint8_t 		traffic_array[60];
	uint16_t		traffic_avg_60sec;
	TickType_t		traffic_time;
	TickType_t		Recv_traffic_time;
	TickType_t		Recv_time_offset;
	uint8_t			Speed;

    // baseline & threshold
    int32_t Z0;            // baseline (int32 to avoid accumulation rounding)
    float   sigma;
    int16_t T;             // hysteresis threshold (symmetric)
    // runtime
    MagState st;
    TickType_t debounce_until;
    TickType_t refractory_until;

} Magnetic_t;

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

	uint8_t		Connect;

	uint8_t     send_flag;
} SENSOR_t;

typedef struct
{
	DISPLAY_T 		wdis;
	DISPLAY_T 		tdis;
    //CDS_T			cds;
    TIMETICK_T		tick;
    INDICATOR_T		indi;
    STATE_T			stat;
    MODEM_T			modem;
    BLE_T			ble;
	//uint8_t 		FRAM_SaveBuff[BUFF_LEN];
    Magnetic_t		magnetic;
	GPS_T			gps;
    SENSOR_t 		Master_sensor;
    SENSOR_t 		Slave_sensor;
} LogicStatus_t;


LogicStatus_t ls;

void SleepMode();
void StopMode();
void StandByMode();

void logicInit();
void logicIdleTask();
void logicUartTask();
void logicPeriTask();
void logicProcTask();
void logicBLETask();
static void DisplayTestMode();
static void DisplayWorkingMode();
static void DisplayTestWhiteMode();
static void DisplayTestOffMode();
void Modem_Packet_Parse(uint8_t opcode, uint8_t section_id, uint8_t device_id, uint8_t* buff);
void Save_Params(uint8_t* ptr, uint16_t size);
uint32_t CalcTickDiff(uint32_t oldTicks, uint32_t newTicks);

#endif /* LOGIC_H_ */
