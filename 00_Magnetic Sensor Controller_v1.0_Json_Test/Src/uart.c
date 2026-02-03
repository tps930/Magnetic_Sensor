/*
 * uart.c
 *
 *  Created on: 2017. 9. 29.
 *      Author: Dongwon
 */


#include <string.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "global.h"
#include "uart.h"
#include "logic.h"


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;
extern UART_HandleTypeDef huart7;

extern osMessageQId RFQueueHandle;
extern osMessageQId SensorQueueHandle;
extern osMessageQId IoTQueueHandle;

COMMAND_T packet;
COMMAND_T IoTpacket;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	/*
    for (uint8_t i=0; i<UART_COUNT; i++)
    {
        if (huart->Instance == s.uartData[i].huart->Instance)
        {
            s.uartData[i].txDmaReady = pdTRUE;
            break;
        }
    }
    */
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	/*
    for (uint8_t i=0; i<UART_COUNT; i++)
    {
        if (huart->Instance == s.uartData[i].huart->Instance)
        {
            s.uartData[i].rxDmaError = pdTRUE;
            break;
        }
    }
    */
}

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

uint8_t CalcLRC(uint8_t* data, uint16_t offset, uint32_t size)
{
    uint8_t lrc = 0;
    for (register uint32_t i=0; i<size; i++)
    {
        lrc ^= data[offset+i];
    }

    return lrc;
}

uint8_t CalcSum(uint8_t* data, uint16_t offset, uint32_t size)
{
    uint8_t sum = 0;
    for (register uint32_t i = 0; i < size; i++)
    {
    	sum += data[offset + i];
    }

    return sum;
}

uint16_t CalcCheckSum(uint8_t* data, uint16_t offset, uint32_t size)
{
    uint16_t checksum = 0xFFFF;
    for (register uint32_t i = 0; i <= size; i++)
    {
    	checksum -= data[offset + i];
    }

    checksum += 1;
    return checksum;
}

void commInit()
{
    memset(&s, 0x00, sizeof(CommStatus_t));

    s.uartData[COMM_UART1].huart = &huart1;		//Debug
    s.uartData[COMM_UART2].huart = &huart2;		//PMS
    s.uartData[COMM_UART3].huart = &huart3;		//IoT
    s.uartData[COMM_UART6].huart = &huart6;		//Sensor
    s.uartData[COMM_UART7].huart = &huart7;		//GPS

    s.uartData[COMM_UART1].init = pdTRUE;
    s.uartData[COMM_UART2].init = pdTRUE;
    s.uartData[COMM_UART3].init = pdTRUE;
    s.uartData[COMM_UART6].init = pdTRUE;
    s.uartData[COMM_UART7].init = pdTRUE;

}

void UART_StrOut(uint8_t port, char *str)
{
	//configASSERT(port >= COMM_UART1 && port <= COMM_UART7);

	int len;

	len= strlen(str);

	HAL_UART_Transmit((UART_HandleTypeDef*)s.uartData[port].huart, str, len, 100);
}


void UART_FmtSend(uint8_t port, char *fmt, ... )
{
	va_list args;

	va_start(args, fmt);
	vsprintf(s.uartData[port].txBuff, fmt, args);
	va_end(args);

	commSendData(port, s.uartData[port].txBuff, strlen(fmt));
}


void commSendData(uint8_t port, uint8_t* data, uint16_t size)
{

	/*printf("commSendData[%d] ", port);
	for(int i = 0; i < size; i++)
		printf("[%02x]", data[i]);
	printf("\r\n");*/

    HAL_UART_Transmit((UART_HandleTypeDef*)s.uartData[port].huart, data, size, 100);
}

void USART1_IRQHandler(void)
{
	if(s.uartData[COMM_UART1].init != pdTRUE)
		return;

	//printf("[UART ]USART1_IRQHandler\r\n");
	if(__HAL_UART_GET_IT_SOURCE(s.uartData[COMM_UART1].huart, UART_IT_RXNE))
	{
		s.uartData[COMM_UART1].rxCh = s.uartData[COMM_UART1].huart->Instance->DR & 0xff;
		//printf("[%02x]", s.uartData[COMM_UART1].rxCh);
		s.uartData[COMM_UART1].rxBuff[s.uartData[COMM_UART1].rxReadPtr++] = s.uartData[COMM_UART1].rxCh;
		s.uartData[COMM_UART1].rxReadPtr %= RX_BUFLEN;
	}
}

void USART2_IRQHandler(void)
{
	if(s.uartData[COMM_UART2].init != pdTRUE)
			return;
	//printf("[UART ]USART2_IRQHandler\r\n");
	if(__HAL_UART_GET_IT_SOURCE(s.uartData[COMM_UART2].huart, UART_IT_RXNE))
	{
		s.uartData[COMM_UART2].rxCh = s.uartData[COMM_UART2].huart->Instance->DR & 0xff;
		//printf("[%02x]", s.uartData[COMM_UART2].rxCh);
		s.uartData[COMM_UART2].rxBuff[s.uartData[COMM_UART2].rxReadPtr++] = s.uartData[COMM_UART2].rxCh;
		s.uartData[COMM_UART2].rxReadPtr %= RX_BUFLEN;
	}
}

void USART3_IRQHandler(void)
{
	if(s.uartData[COMM_UART3].init != pdTRUE)
			return;

	//printf("[UART ]USART3_IRQHandler[%d] \r\n", s.uartData[COMM_UART3].huart->Instance->DR & 0xff);

	if(__HAL_UART_GET_IT_SOURCE(s.uartData[COMM_UART3].huart, UART_IT_RXNE))
	{
		s.uartData[COMM_UART3].rxCh = s.uartData[COMM_UART3].huart->Instance->DR & 0xff;
		//printf("[%02x]", s.uartData[COMM_UART3].rxCh);
		s.uartData[COMM_UART3].rxBuff[s.uartData[COMM_UART3].rxReadPtr++] = s.uartData[COMM_UART3].rxCh;
		s.uartData[COMM_UART3].rxReadPtr %= RX_BUFLEN;
	}
}

void USART6_IRQHandler(void)
{
	if(s.uartData[COMM_UART6].init != pdTRUE)
			return;
	//printf("[UART ]USART1_IRQHandler\r\n");
	if (__HAL_UART_GET_IT_SOURCE(s.uartData[COMM_UART6].huart, UART_IT_RXNE)) {
		s.uartData[COMM_UART6].rxCh = s.uartData[COMM_UART6].huart->Instance->DR & 0xff;
		//printf("[%02x]", s.uartData[COMM_UART6].rxCh);
		s.uartData[COMM_UART6].rxBuff[s.uartData[COMM_UART6].rxReadPtr++] = s.uartData[COMM_UART6].rxCh;
		s.uartData[COMM_UART6].rxReadPtr %= RX_BUFLEN;
	}
}

void UART7_IRQHandler(void)
{
	if(s.uartData[COMM_UART7].init != pdTRUE)
			return;
	//printf("[UART ]USART7_IRQHandler\r\n");
	if (__HAL_UART_GET_IT_SOURCE(s.uartData[COMM_UART7].huart, UART_IT_RXNE)) {
		s.uartData[COMM_UART7].rxCh = s.uartData[COMM_UART7].huart->Instance->DR & 0xff;
		//printf("[%02x]", s.uartData[COMM_UART7].rxCh);
		s.uartData[COMM_UART7].rxBuff[s.uartData[COMM_UART7].rxReadPtr++] = s.uartData[COMM_UART7].rxCh;
		s.uartData[COMM_UART7].rxReadPtr %= RX_BUFLEN;
	}
}



static void RF_Recv_Data_Parse()
{
	UartData_t* const uartData = &s.uartData[RF_UART_PORT];
	uint8_t recvLrc;
	uint8_t calcLrc;
	printf("RF_Recv_Data_Parse \r\n");
	memset(&uartData->data, 0x00, RX_BUFLEN);
	memset(&uartData->packet, 0x00, RX_PACKETLEN);
	memset(&packet, 0x00, sizeof(COMMAND_T));

	memcpy(&uartData->data, &uartData->rxDataBuff[0], uartData->rxSize);
	memcpy(&uartData->packet, &uartData->data, uartData->rxSize);

	calcLrc = 0;
	calcLrc = CalcLRC(uartData->packet, 0, (uartData->rxSize - 1));
	recvLrc = uartData->packet[uartData->rxSize - 1];

	if (recvLrc != 0xFF) {
		if (recvLrc != calcLrc) {
			printf("\r\n[Modem] Rcv_LRC Error[%2X][%2X]\r\n", calcLrc, recvLrc);
			packet.Result = pdFALSE;
			osMessagePut(RFQueueHandle, (uint32_t) &packet, 0);
			return;
		}
	}

	packet.opcode = uartData->packet[1];
	packet.Result = pdTRUE;
	memcpy(&packet.data, &uartData->packet[2], 4);
	osMessagePut(RFQueueHandle, (uint32_t) &packet, 0);
	/*switch (packet.opcode)		//Opcode
	{
	case RF_CMD_BOOT_MSG:		//초기 부팅시 수신 메세지
		packet.Result = pdTRUE;
		memcpy(&packet.data, &uartData->packet[2], 2);	//Section-ID, Device-ID

		osMessagePut(RFQueueHandle, (uint32_t) &packet, 0);
		break;

	case RF_CMD_REG_STATUS:		//C-Resigter 상태 정보
		packet.Result = pdTRUE;
		memcpy(&packet.data, &uartData->packet[2], 10);
		osMessagePut(RFQueueHandle, (uint32_t) &packet, 0);
		break;

	default:
		packet.Result = pdFALSE;
		//osMessagePut(messageQueueHandle, (uint32_t) &packet, 0);
		printf("\r\n[Modem] OP_CODE Error[%2X]\r\n", packet.opcode);
		break;
	}*/

}

void RF_Receive_Check()
{
	UartData_t* const uartData = &s.uartData[RF_UART_PORT];
	uint8_t rx_ch;

	while (uartData->rxReadPtr != uartData->rxProcPtr) {
		rx_ch = uartData->rxBuff[uartData->rxProcPtr++];
		uartData->rxProcPtr %= RX_BUFLEN;

		switch (uartData->rxMode) {
		case 0:							// STX1
			if (rx_ch == 0x7E)
			{
				uartData->rxMode++;

				uartData->rxCount = 0;
				uartData->recvLength = 0;
				//printf("[UART ] Modem_Receive_Check ", rx_ch);
				//printf("[%02x]", rx_ch);
			}
			break;
		case 1:							// Length
			uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
			uartData->recvLength = rx_ch;
			//printf("[%d-%02x]",uartData->recvLength, rx_ch);
			uartData->recvLength--;
			uartData->rxMode++;
			break;
		case 2:							// data
			uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
			//printf("[%d-%02x]", uartData->recvLength, rx_ch);

			uartData->recvLength--;

			if (uartData->recvLength == 0)
				uartData->rxMode++;

			break;
		case 3:							// ETX
			if (rx_ch == 0x7F) {
				uartData->rxRdy = pdTRUE;
				uartData->rxMode = 0;
				//printf("[%02x] END\r\n", rx_ch);
			} else {
				uartData->rxMode = 0;
			}
			break;
		}

		if (uartData->rxRdy == pdTRUE)
			break;
	}

	if (uartData->rxRdy == pdFALSE)
		return;

    uartData->rxRdy = pdFALSE;

	uartData->rxSize = uartData->rxCount;

	RF_Recv_Data_Parse();
}

void Sensor_Receive_Check()
{
	UartData_t* const uartData = &s.uartData[SENSOR_UART_PORT];

	const TickType_t tickNow = xTaskGetTickCount();

	uint8_t rx_ch;
	while (uartData->rxReadPtr != uartData->rxProcPtr) {
		rx_ch = uartData->rxBuff[uartData->rxProcPtr++];
		uartData->rxProcPtr %= RX_BUFLEN;
		uartData->rxTicks = tickNow;

		switch (uartData->rxMode) {
			case 0:							// opcode
				if (rx_ch == 0x7E)
				{
					uartData->rxMode++;
				}
				break;
			case 1:							// frameNo
				if (rx_ch == 0x7E)
				{
					uartData->rxMode++;

					uartData->rxCount = 0;
					//printf("[%02x]\r\n", rx_ch);
				}
				else
					uartData->rxMode = 0;

				break;
			case 2:							// length
				uartData->rxBuff[uartData->rxCount++] = rx_ch;
				//printf("[%02x]\r\n", rx_ch);
				uartData->rxMaxCount = rx_ch;
				uartData->rxMaxCount--;
				uartData->rxMode++;
				break;

			case 3:							// VSLID, OPCODE, DATAILD[0~7], LRC
				uartData->rxBuff[uartData->rxCount++] = rx_ch;
				uartData->rxMaxCount--;

				if(uartData->rxMaxCount == 0) {
					uartData->rxMode++;
				}
				break;

			case 4:							// ETX
				if (rx_ch == 0x7F) {
					uartData->rxRdy = pdTRUE;
					uartData->rxMode = 0;
				}
				break;
		}

		if (uartData->rxRdy == pdTRUE)
			break;
	}


	if (CalcTickDiff(uartData->rxTicks, tickNow) >= 5000) {
		memset(&uartData->rxBuff, 0x00, RX_BUFLEN);
		uartData->rxProcPtr = 0;
		uartData->rxReadPtr = 0;
		uartData->rxMode = 0;
	}

	if (uartData->rxRdy == pdFALSE)
		return;

	uartData->rxRdy = pdFALSE;

	uartData->rxSize = uartData->rxCount;

	Sensor_Data_Parse();
}

void Sensor_Data_Parse()
{
    UartData_t* const uartData = &s.uartData[SENSOR_UART_PORT];
    uint8_t recvCRC;
    uint8_t calcCRC;
    memset(&uartData->data, 0x00, RX_BUFLEN);
    memset(&uartData->packet, 0x00, RX_PACKETLEN);

    memcpy(&uartData->data, &uartData->rxBuff[0], uartData->rxSize);
    memcpy(&uartData->packet, &uartData->data, uartData->rxSize);

    calcCRC = 0;
    calcCRC = CalcLRC(&uartData->packet[0], 0, uartData->rxSize - 1);
    recvCRC = uartData->packet[uartData->rxSize - 1];

	if (recvCRC) {
		if (recvCRC != calcCRC) {
			printf("\r\n[UART] Rcv_LRC Error[%2X][%2X]\r\n", calcCRC, recvCRC);
			return;
		}
	}

	packet.deviceID = uartData->packet[1];
    packet.opcode = uartData->packet[2];

    switch (packet.opcode)		//Opcode
    {
    	case OP_SENSORDATA_SEND:			//센서 데이터 수신
    		packet.Result = 0x00;
    		memcpy(&packet.data[0],  &uartData->packet[3], 7);

			osMessagePut(SensorQueueHandle, (uint32_t) &packet, 0);
    		break;

    	default:
    		printf("[UART ] SENSOR_Rcv_OPCODE Error[opcode : %X]\r\n", packet.opcode);
    		break;
    }
}

void Gps_Receive_Check()
{
	UartData_t* const uartData = &s.uartData[GPS_UART_PORT];

	uint8_t rx_ch;

	while(uartData->rxReadPtr != uartData->rxProcPtr){
		rx_ch = uartData->rxBuff[uartData->rxProcPtr++];
		uartData->rxProcPtr %= RX_BUFLEN;

/*		if (rx_ch == '\n') {
			if (uartData->rxCount >= 0) {
				uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
				uartData->rxSize = uartData->rxCount;
				uartData->rxRdy = pdTRUE;
			}
		} else {
			uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
		}*/


		switch (uartData->rxMode) {
			case 0:							// STX1
				if (rx_ch == '$')
				{
					uartData->rxCount = 0;
					uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
					uartData->rxMode++;
				}

				break;
			case 1:							// ETX Check
				uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
				if(rx_ch == '*')
				{
					uartData->rxRdy = pdTRUE;
					uartData->rxMode = 0;
				}
				break;
		}

		if (uartData->rxRdy == pdTRUE)
			break;
	}

	if (uartData->rxRdy == pdFALSE)
		return;


	uartData->rxRdy = pdFALSE;
	uartData->rxSize = uartData->rxCount;
	uartData->rxCount = 0;

	Gps_Recv_Data_Parse();
}


void Gps_Recv_Data_Parse()
{
	//configASSERT(port == COMM_UART1);
	uint8_t calc_lrc;
	uint16_t calc_checksum, recv_checksum;


    uint8_t recv_rdy;
    uint8_t gps_st;
    uint8_t gps_cnt;
	UartData_t* const uartData = &s.uartData[GPS_UART_PORT];
	char* str = "";
	char* pos = "";
	uint8_t gps_end = pdFALSE;
	uint32_t	gps_time;
	uint32_t	gps_date;

	uartData->rxRdy = pdFALSE;

    memcpy(&uartData->data, &uartData->rxDataBuff[0], uartData->rxSize);

    str = uartData->data;

    pos = strtok(str, ",");

	if (strstr(pos, "$GPRMC") != NULL)
		recv_rdy = pdTRUE;
	else
		recv_rdy = pdFALSE;

	if (recv_rdy != pdTRUE)
		return;


	gps_cnt = 0;

    while(pos != NULL && gps_end == pdFALSE)
    {
		pos = strtok(NULL, ",");

		gps_cnt++;

		if (pos == NULL)
			break;

		switch (gps_cnt) {
			case 1:		//시간
				gps_time = atoi(pos);
				break;
			case 2:		//상태
				if (strstr(pos, "A") != NULL)
					gps_st = pdTRUE;
				else
					gps_st = pdFALSE;
				break;
			case 3:	//위도
				//packet.lat = atof(pos);
				break;
			case 5:	//경도
				//packet.lon = atof(pos);
				break;
			case 7:		//Speed Over Ground 속도(knots)
				//packet.knots = atof(pos);

				//gps_end = pdTRUE;
				break;
			case 8:		//날짜
			case 9:		//날짜
				//printf("[%d][%d][%s]\r\n", gps_cnt, strlen(pos), pos);
				if(strlen(pos) == 6)
				{
					gps_date = atoi(pos);

					gps_end = pdTRUE;
				}
				break;
		}
	}
    //printf("\r\n");

    if(gps_end == pdTRUE && gps_st == pdTRUE)
    {
    	  packet.opcode = OP_GPSDATA_SEND;
    	  packet.Result = 0x00;
    	  packet.data[0] = gps_date % 100;			//Year
    	  packet.data[1] = ((gps_date / 100) % 100);	//Mon
    	  packet.data[2] = (gps_date / 10000);		//Day

    	  packet.data[3] = gps_time / 10000;			//Hour
    	  packet.data[4] = ((gps_time / 100) % 100);	//Min
    	  packet.data[5] = (gps_time % 100);			//Sec

    	osMessagePut(SensorQueueHandle, (uint32_t) &packet, 0);
    }

}

void IoT_Recv_Data_Parse()
 {
	UartData_t* const uartData = &s.uartData[IOT_UART_PORT];
	const uint16_t rxsize_copy = uartData->rxSize;
	uint32_t queueData;
	uint8_t recvLrc;
	uint8_t calcLrc;
	uint8_t recv_rdy;
	char* pos;
	char* recvptr = uartData->rxBuff;

	memset(&uartData->data, 0x00, RX_BUFLEN);

	memcpy(&uartData->data, &uartData->rxBuff[0], rxsize_copy);

	//printf("[UART RECV START 1  ] ------------------------------\r\n");
	recv_rdy = pdFALSE;
	//commSendData(COMM_UART1, uartData->data, rxsize_copy);
	printf("[IoT RECV] %s \r\n", uartData->data);
	//printf("\r\n [UART RECV END] ------------------------------\r\n");

	recvptr = uartData->data;

	pos = strtok(recvptr, " >\r\n");

	while (pos != NULL) {
		if (strstr(pos, "BUSY") != NULL) {
			printf("[UART  ] Packet Send Retry\r\n");
			return;
		}
		if (strstr(pos, "rxTimeOut") != NULL) {
			printf("[UART  ] Packet Send Possible\r\n");
			return;
		}
		if (strncmp(pos, "JOINED:YES", 10) == 0) {
			Set_IoT_Joined(pdTRUE);
		}

		if (strncmp(pos, "JOINED:NO", 9) == 0) {
			Set_IoT_Joined(pdFALSE);
		}

		if (strncmp(pos, "AT+RECV", 7) == 0) {
			pos = strtok(NULL, " \r\n");

			recv_rdy = pdTRUE;
			break;
		}
		pos = strtok(NULL, " \r\n");

		if (pos == NULL)
			return;
	}

	if (recv_rdy != pdTRUE)
		return;

	//printf("[UART RECV START 2  ] ------------------------------\r\n");

	uint8_t recv_str_len, payload_len, data_len, opcode;

	uartData->str_len = strlen(pos);

	if (!strcmp(pos, &uartData->Pre_packet)) {
		printf("[UART  ] Recv Packet Equal \r\n");
		return;
	}

	memset(&uartData->packet, 0x00, RX_PACKETLEN);

	recv_str_len = HexToBin(pos, &uartData->packet[0], uartData->str_len);

	if (recv_str_len <= 0) {
		printf("[UART  ] PACKET LENGTH ZERO \r\n");
		return;
	}

	payload_len = uartData->packet[PACKET_LENGTH_INDEX];//전체 패킷 사이즈(SKT 고정 패킷 4Byte 제외)

	if (payload_len <= 0) {
		printf("[UART  ] PayLoad_Length Zero\r\n");
		//ls.isSend = pdTRUE;
		return;
	}

	if (payload_len > recv_str_len) {
		printf("[UART  ] PACKET LENGTH ERROR[%02d][%02d]\r\n", payload_len,
				recv_str_len);
		IoTpacket.Result = NAK_DATASIZE_ERROR;

		osMessagePut(IoTQueueHandle, (uint32_t) &IoTpacket, 0);
		return;
	}

	IoTpacket.size = payload_len;
	IoTpacket.Result = RESPONSE_ACK;
	memcpy(&IoTpacket.data, &uartData->packet[PAYLOAD_START_INDEX], payload_len);
	osMessagePut(IoTQueueHandle, (uint32_t) &IoTpacket, 0);
	return;
}

void IoT_Receive_Check()
{
	UartData_t* const uartData = &s.uartData[IOT_UART_PORT];

	uint8_t rx_ch;

	while (uartData->rxReadPtr != uartData->rxProcPtr) {
		rx_ch = uartData->rxBuff[uartData->rxProcPtr++];
		uartData->rxProcPtr %= RX_BUFLEN;

		if (rx_ch == '\n') {

			if (uartData->rxCount >= 0) {
				uartData->rxBuff[uartData->rxCount++] = rx_ch;
				uartData->rxSize = uartData->rxCount;
				IoT_Recv_Data_Parse();		//IoT 수신메세지 분석
				uartData->rxCount = 0;
			}
		} else {
			uartData->rxBuff[uartData->rxCount++] = rx_ch;
		}
	}
}


unsigned char HexChar (char c)
{
    if ('0' <= c && c <= '9') return (unsigned char)(c - '0');
    if ('A' <= c && c <= 'F') return (unsigned char)(c - 'A' + 10);
    if ('a' <= c && c <= 'f') return (unsigned char)(c - 'a' + 10);
    return 0xFF;
}


int HexToBin (const char* s, uint8_t * buff, int length)
{
    int result;
    if (!s || !buff || length <= 0) return -1;

    for (result = 0; *s; ++result)
    {
        unsigned char msn = HexChar(*s++);
        if (msn == 0xFF) return -1;
        unsigned char lsn = HexChar(*s++);
        if (lsn == 0xFF) return -1;
        unsigned char bin = (msn << 4) + lsn;

        if (length-- <= 0) return -1;
        *buff++ = bin;
    }
    return result;
}

void BinToHex (const unsigned char * buff, int length, char * output, int outLength)
{
    char binHex[] = "0123456789ABCDEF";

    //if (!output || outLength < 4) return;
    if (outLength < 4) return;
    //*output = '\0';

    if (!buff || length <= 0 || outLength <= 2 * length)
    {
        //memcpy(output, "ERR", 4);
        return;
    }

    for (; length > 0; --length, outLength -= 2)
    {
        unsigned char byte = *buff++;

        *output++ = binHex[(byte >> 4) & 0x0F];
        *output++ = binHex[byte & 0x0F];
    }
    if (outLength-- <= 0) return;
    *output++ = '\0';
}


