/*
 * uart.h
 *
 *  Created on: 2017. 9. 29.
 *      Author: Dongwon
 */

#ifndef COMM_H_
#define COMM_H_

#define COMM_UART1  0		//Debug
#define COMM_UART2  1		//RF
#define COMM_UART3  2		//IoT
#define COMM_UART6  3		//Sensor
#define COMM_UART7  4		//GPS

#define SENSOR_UART_PORT  		COMM_UART6
#define RF_UART_PORT  			COMM_UART2
#define DEBUG_UART_PORT 		COMM_UART1
#define GPS_UART_PORT 			COMM_UART7
#define IOT_UART_PORT 			COMM_UART3

#define UART_COUNT      5
#define RX_BUFLEN       1024
#define TX_BUFLEN       1024
#define RX_PACKETLEN      64

#define PACKET_LENGTH_INDEX     				4

#define PAYLOAD_START_INDEX    					5
#define PAYLOAD_DATASIZE_INDEX    				PAYLOAD_START_INDEX+1
#define PAYLOAD_PROTOCOL_VER_MAJOR_INDEX    	PAYLOAD_DATASIZE_INDEX+1
#define PAYLOAD_PROTOCOL_VER_MINOR_INDEX    	PAYLOAD_PROTOCOL_VER_MAJOR_INDEX+1
#define PAYLOAD_OPCODE_INDEX    				PAYLOAD_PROTOCOL_VER_MINOR_INDEX+1
#define PAYLOAD_SENSOR_OPCODE_INDEX    			PAYLOAD_OPCODE_INDEX+1
#define PAYLOAD_DATA_INDEX    					PAYLOAD_SENSOR_OPCODE_INDEX+1


#define RESPONSE_ACK		0x01
#define RESPONSE_NAK		0x02

#define NAK_STX_ERROR					0x02
#define NAK_DATASIZE_ERROR				0x03
#define NAK_DATASIZE_MISS				0x04
#define NAK_PROTOCOL_VERSION_ERROR		0x05
#define NAK_UNDEFIND_OPCODE				0x06
#define NAK_LRC_CHECK_ERROR				0x07
#define NAK_ETX_ERROR					0x08
#define NAK_NOT_USE_OPCODE				0x09
#define NAK_NOT_VALUE					0x10
#define NAK_CONVERT_ERROR				0x11
#define NAK_FORMAT_ERROR				0x12
#define NAK_DATA_PARSING_ERROR			0x13
#define NAK_OVERLAP_VALUE				0x14
#define NAK_UNKNOWN_ERROR				0x99

typedef struct
{
    UART_HandleTypeDef* huart;
    uint8_t	init;
    uint8_t rxCh;
    uint8_t rxBuff[RX_BUFLEN];

    uint8_t rxMode;
    uint8_t rxCount;
    uint8_t rxRdy;
    uint16_t rxMaxCount;

    uint16_t rxReadPtr;
    uint16_t rxProcPtr;

    uint8_t rxDataBuff[RX_BUFLEN];
    uint8_t recvLength;
    uint16_t rxSize;
    TickType_t rxTicks;

    uint8_t txBuff[TX_BUFLEN];
    __IO uint8_t txDmaReady;

    uint8_t		data[RX_BUFLEN];
    uint8_t		packet[RX_PACKETLEN];
    uint8_t		Pre_packet[RX_PACKETLEN];
    uint32_t	packet_len;
    uint32_t	str_len;
    uint32_t	payload_len;
} UartData_t;

typedef struct
{
    UartData_t uartData[UART_COUNT];

} CommStatus_t;

CommStatus_t s;


void commInit();
void commSendData(uint8_t port, uint8_t* data, uint16_t size);
uint16_t CalcCheckSum(uint8_t* data, uint16_t offset, uint32_t size);
uint8_t CalcLRC(uint8_t* data, uint16_t offset, uint32_t size);
uint8_t CalcSum(uint8_t* data, uint16_t offset, uint32_t size);
void commRecvPolling();
void Gps_Recv_Data_Parse();
void Gps_Receive_Check();

void Sensor_Receive_Check();
void Sensor_Data_Parse();

#endif /* COMM_H_ */
