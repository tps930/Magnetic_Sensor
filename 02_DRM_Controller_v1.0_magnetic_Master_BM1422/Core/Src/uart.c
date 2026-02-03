/*
 * uart.c
 *
 *  Created on: 2021. 4. 27.
 *      Author: user
 */

#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"
#include "uart.h"
#include "logic.h"

extern UART_HandleTypeDef huart1;		//Firmware Download
extern UART_HandleTypeDef huart4;		//RF Modem
extern UART_HandleTypeDef huart5;		//BLE

extern RTC_HandleTypeDef hrtc;

extern osMessageQId bleQueueHandle;
extern osMessageQId uartQueueHandle;

static COMMAND_T packet;

static unsigned short crc16_tab[CRC_TABLE_SIZE] = {
		0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
		0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
		0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
		0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
		0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
		0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
		0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
		0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
		0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
		0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
		0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
		0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
		0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
		0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
		0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
		0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
		0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
		0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
		0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
		0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
		0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
		0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
		0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
		0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
		0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
		0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
		0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
		0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
		0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
		0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
		0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
		0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040 };


extern LogicStatus_t ls;

void USART1_IRQHandler(void)
{
	  HAL_UART_IRQHandler(&huart1);

		/*if(s.uartData[COMM_UART1].init != pdTRUE)
			return;

		if(__HAL_UART_GET_IT_SOURCE(s.uartData[COMM_UART1].huart, UART_IT_RXNE))
		{
			s.uartData[COMM_UART1].rxTicks = xTaskGetTickCount();
			s.uartData[COMM_UART1].rxCh = s.uartData[COMM_UART1].huart->Instance->RDR & 0xff;
			s.uartData[COMM_UART1].rxBuff[s.uartData[COMM_UART1].rxReadPtr++] = s.uartData[COMM_UART1].rxCh;
			s.uartData[COMM_UART1].rxReadPtr %= RX_BUFLEN;
			//printf("Uart USART1 : %c\r\n",s.uartData[COMM_UART1].rxCh);
		}*/
}

void USART4_5_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart4);
	HAL_UART_IRQHandler(&huart5);

	/*if (s.uartData[COMM_UART2].init != pdTRUE)
		return;

	if (__HAL_UART_GET_IT_SOURCE(s.uartData[COMM_UART2].huart, UART_IT_RXNE)) {
		s.uartData[COMM_UART2].rxTicks = xTaskGetTickCount();
		s.uartData[COMM_UART2].rxCh = s.uartData[COMM_UART2].huart->Instance->RDR & 0xff;
		s.uartData[COMM_UART2].rxBuff[s.uartData[COMM_UART2].rxReadPtr++] = s.uartData[COMM_UART2].rxCh;
		s.uartData[COMM_UART2].rxReadPtr %= RX_BUFLEN;

		//printf("%02x\r\n", s.uartData[COMM_UART2].rxCh);
	}*/
}

/*
void USART5_IRQHandler(void)
 {
	  HAL_UART_IRQHandler(&huart5);
	if (s.uartData[COMM_UART3].init != pdTRUE)
		return;

	//printf("Uart USART3_IRQHandler.....\r\n");
	if (__HAL_UART_GET_IT_SOURCE(s.uartData[COMM_UART3].huart, UART_IT_RXNE)) {
		s.uartData[COMM_UART3].rxTicks = xTaskGetTickCount();
		s.uartData[COMM_UART3].rxCh = s.uartData[COMM_UART3].huart->Instance->RDR & 0xff;
		s.uartData[COMM_UART3].rxBuff[s.uartData[COMM_UART3].rxReadPtr++] = s.uartData[COMM_UART3].rxCh;
		s.uartData[COMM_UART3].rxReadPtr %= RX_BUFLEN;
		//printf("Uart USART3 : %02x [%d / %d]\r\n",s.uartData[COMM_UART3].rxCh, s.uartData[COMM_UART3].rxProcPtr, s.uartData[COMM_UART3].rxReadPtr);
	}
}*/

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
 {
	if (huart->Instance == USART1) {
		HAL_UART_DMAStop(&huart1);
		HAL_UART_Receive_DMA(&huart1, &s.uartData[COMM_UART1].rxCh, 1);
	}else if (huart->Instance == USART4) {
		HAL_UART_DMAStop(&huart4);
		HAL_UART_Receive_DMA(&huart4, &s.uartData[COMM_UART4].rxCh, 1);
	}else if (huart->Instance == USART5) {
		HAL_UART_DMAStop(&huart5);
		HAL_UART_Receive_DMA(&huart5, &s.uartData[COMM_UART5].rxCh, 1);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)
	{
		s.uartData[COMM_UART1].rxTicks = xTaskGetTickCount();
		s.uartData[COMM_UART1].rxBuff[s.uartData[COMM_UART1].rxReadPtr++] = s.uartData[COMM_UART1].rxCh;
		s.uartData[COMM_UART1].rxReadPtr %= RX_BUFLEN;
		//printf("Uart USART1 : [%02x]\r\n", s.uartData[COMM_UART4].rxCh);
		HAL_UART_Receive_DMA(&huart1, &s.uartData[COMM_UART1].rxCh, 1);
	}else if(huart->Instance==USART4)
	{
		s.uartData[COMM_UART4].rxTicks = xTaskGetTickCount();
		s.uartData[COMM_UART4].rxBuff[s.uartData[COMM_UART4].rxReadPtr++] = s.uartData[COMM_UART4].rxCh;
		s.uartData[COMM_UART4].rxReadPtr %= RX_BUFLEN;
		//printf("Uart USART4 : [%02x]\r\n", s.uartData[COMM_UART4].rxCh);
		HAL_UART_Receive_DMA(&huart4, &s.uartData[COMM_UART4].rxCh, 1);
	}else if(huart->Instance==USART5)
	{
		s.uartData[COMM_UART5].rxTicks = xTaskGetTickCount();
		s.uartData[COMM_UART5].rxBuff[s.uartData[COMM_UART5].rxReadPtr++] = s.uartData[COMM_UART5].rxCh;
		s.uartData[COMM_UART5].rxReadPtr %= RX_BUFLEN;
		//printf("Uart USART5 : [%02x]\r\n", s.uartData[COMM_UART4].rxCh);
		HAL_UART_Receive_DMA(&huart5, &s.uartData[COMM_UART5].rxCh, 1);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)
	{
		printf("Uart USART1 : HAL_UART_TxCpltCallback\r\n");
	}else if(huart->Instance==USART4)
	{
		printf("Uart USART4 : HAL_UART_TxCpltCallback\r\n");
	}else if(huart->Instance==USART5)
	{
		printf("Uart USART5 : HAL_UART_TxCpltCallback\r\n");
	}
}

void UART_IDLECallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)
	{
		printf("Uart USART1 : UART_IDLECallback\r\n");
		//HAL_UART_Receive_DMA(&huart1, &s.uartData[COMM_UART1].rxCh, 1);
	}else if(huart->Instance==USART4)
	{
		printf("Uart USART4 : UART_IDLECallback\r\n");
		//HAL_UART_Receive_DMA(&huart2, &s.uartData[COMM_UART2].rxCh, 1);
	}else if(huart->Instance==USART5)
	{
		printf("Uart USART5 : UART_IDLECallback\r\n");
		//HAL_UART_Receive_DMA(&huart6, &s.uartData[COMM_UART6].rxCh, 1);
	}
}


void commInit()
{
	printf("Uart Init.....\r\n");

    memset(&s, 0x00, sizeof(CommStatus_t));

    s.uartData[COMM_UART1].huart = &huart1;		//Firmware
    s.uartData[COMM_UART4].huart = &huart4;		//RF Modem
    s.uartData[COMM_UART5].huart = &huart5;		//BLE

    s.uartData[COMM_UART1].init = pdTRUE;
    s.uartData[COMM_UART4].init = pdTRUE;
    s.uartData[COMM_UART5].init = pdTRUE;


	HAL_UART_Receive_DMA(&huart1, &s.uartData[COMM_UART1].rxCh, 1);
	HAL_UART_Receive_DMA(&huart4, &s.uartData[COMM_UART4].rxCh, 1);
	HAL_UART_Receive_DMA(&huart5, &s.uartData[COMM_UART5].rxCh, 1);

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_RXNE);

	HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	HAL_NVIC_SetPriority(USART4_5_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(USART4_5_IRQn);
}

/* Set Baud function */
void UART_Baud(UART_HandleTypeDef* huart, uint32_t baud)
{
	huart->Init.BaudRate = baud;
	printf("[Uart] BLE Baud Change [%d]\r\n", baud);

	if (HAL_UART_Init(huart) != HAL_OK)
	{
		Error_Handler();
	}
}

void UART_StrOut(uint8_t port, char *str)
{
	configASSERT(port >= COMM_UART1 && port <= COMM_UART5);

	int len;

	len= strlen(str);

	HAL_UART_Transmit((UART_HandleTypeDef*)s.uartData[port].huart, str, len, 1000);
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
    HAL_UART_Transmit((UART_HandleTypeDef*)s.uartData[port].huart, data, size, 100);
}



void BLE_Data_Parse()
{
    UartData_t* const uartData = &s.uartData[BLE_UART_PORT];
    uint16_t recvCRC;
    uint16_t calcCRC;
    char* pos;
    char* value;
    char* recvptr;

    memset(&uartData->packet, 0x00, RX_BUFLEN);
    memcpy(&uartData->packet, &uartData->rxDataBuff[0], uartData->rxSize);

    recvptr = uartData->packet;

	pos = strtok(recvptr, " \r\n");

	ls.ble.isInit = pdTRUE;

	if (strncmp(pos, "+OK", 3) == 0) {
		return;
	}
	if (strncmp(pos, "+ERROR", 6) == 0) {
		return;
	}

	if (strncmp(pos, "+ADVERTISING", 12) == 0) {
		ls.stat.preADVStat = pdTRUE;
		return;
	}

	if (strncmp(pos, "+STANDBY", 8) == 0) {
		ls.stat.preADVStat = pdFALSE;
		return;
	}

	printf("BLE_Data [%s] \r\n", pos);


}

void BLE_Receive_Check(uint8_t isCheckMode)
{
	UartData_t* const uartData = &s.uartData[BLE_UART_PORT];

	uint8_t rx_ch;
	uint8_t recv_length = 10;

	while (uartData->rxReadPtr != uartData->rxProcPtr) {
		rx_ch = uartData->rxBuff[uartData->rxProcPtr++];
		uartData->rxProcPtr %= RX_BUFLEN;

		if (isCheckMode) {
			switch (uartData->rxMode) {
				case 0:							// STX1
					if (rx_ch == 0x7E)
						uartData->rxMode++;

					uartData->rxCount = 0;
					//printf("[UART ]", rx_ch);

					//printf("%c", rx_ch);
					//printf("[%02x]", rx_ch);
					break;
				case 1:							// opcode
					uartData->rxCount = 0;
					recv_length = 0;
					uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
					uartData->rxMode++;
					//printf("[%02x]", rx_ch);
					break;
				case 2:							// frameNo
					uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
					//printf("[%02x]", rx_ch);
					uartData->rxMode++;
					break;
				case 3:							// Totalframe
					uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
					//printf("[%02x]", rx_ch);
					uartData->rxMode++;
					break;
				case 4:							// length Highd
					uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
					recv_length = (rx_ch << 8);
					//printf("[%02x]", rx_ch);
					uartData->rxMode++;
					break;
				case 5:							// length Low
					uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
					recv_length |= rx_ch;
					//printf("[%02x]", rx_ch);
					if (recv_length)
						uartData->rxMode++;
					else
						uartData->rxMode = 7;
					break;
				case 6:							// Data
					uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
					recv_length--;
					printf("[%02x]", rx_ch);

					if (recv_length == 0)
						uartData->rxMode++;
					break;

				case 7:							// CRC
					uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
					//printf("[%02x]", rx_ch);
					uartData->rxMode++;
					break;

				case 8:							// CRC
					uartData->rxDataBuff[uartData->rxCount++] = rx_ch;

					//printf("[%02x]", rx_ch);
					uartData->rxMode++;
					break;
				case 9:							// ETX
					if (rx_ch == 0x7F) {
						uartData->rxRdy = pdTRUE;
						uartData->rxMode = 0;
						//printf("[%02x]\r\n", rx_ch);
					} else {
						uartData->rxMode = 0;
					}
					break;
			}

			if (uartData->rxRdy == pdTRUE)
				break;

		} else {
			if (rx_ch == '\r') {
				printf("\r\n", rx_ch);
				//if (uartData->rxCount > 0) {
					uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
					uartData->rxDataBuff[uartData->rxCount++] = '\n';
					uartData->rxRdy = pdTRUE;
				//}
			} else {
				uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
				printf("%c", rx_ch);
			}

			if (uartData->rxRdy == pdTRUE)
				break;
		}
	}
	if (uartData->rxRdy == pdFALSE)
		return;

	uartData->rxRdy = pdFALSE;

	uartData->rxSize = uartData->rxCount;
	uartData->rxCount = 0;
	if (isCheckMode) {
		Firmware_Data_Parse();
	} else {
		BLE_Data_Parse();
	}
}

void Modem_Data_Parse()
{
    UartData_t* const uartData = &s.uartData[MODEM_UART_PORT];
    uint8_t recvLrc;
    uint8_t calcLrc;

    memset(&uartData->packet, 0x00, RX_BUFLEN);
    memset(&packet, 0x00, sizeof(COMMAND_T));

    memcpy(&uartData->packet, &uartData->rxDataBuff[0], uartData->rxSize);

    calcLrc = 0;
    calcLrc = CalcLRC(uartData->packet, 0, (uartData->rxSize - 1));
    recvLrc = uartData->packet[uartData->rxSize - 1];

    if(recvLrc != 0xFF){
    	if (recvLrc != calcLrc) {
			printf("\r\n[Modem] Rcv_LRC Error[%2X][%2X]\r\n", calcLrc, recvLrc);
			packet.Result = pdFALSE;
			osMessagePut(uartQueueHandle, (uint32_t) &packet, 0);
			return;
    	}
   	}

	packet.opcode = uartData->packet[1];
	packet.Result = pdTRUE;
	memcpy(&packet.data, &uartData->packet[2], 4);
	osMessagePut(uartQueueHandle, (uint32_t) &packet, 0);

/*    switch (packet.opcode)		//Opcode
    {
    	case RF_CMD_BOOT_MSG:		//초기 부팅시 수신 메세지
    		packet.Result = pdTRUE;
    		memcpy(&packet.data, &uartData->packet[2], 2);		//Section-ID, Device-ID

    		osMessagePut(uartQueueHandle, (uint32_t) &packet, 0);
    		break;

    	case RF_CMD_REG_STATUS:		//C-Resigter 상태 정보
    		packet.Result = pdTRUE;
    		memcpy(&packet.data, &uartData->packet[2], 11);
    		osMessagePut(uartQueueHandle, (uint32_t) &packet, 0);
    		break;

    	case RF_CMD_SEND:
    	case RF_CMD_RECV:			//Recv Data Packet
    		packet.Result = pdTRUE;
    		packet.datalen = uartData->packet[1];
    		memcpy(&packet.data[0], &uartData->packet[2], packet.datalen - 2);
    		osMessagePut(uartQueueHandle, (uint32_t) &packet, 0);
    		break;

    	default:
    		packet.Result = pdFALSE;
    		//osMessagePut(messageQueueHandle, (uint32_t) &packet, 0);
    		printf("\r\n[Modem] OP_CODE Error[%2X]\r\n", packet.opcode);
    		break;
    }*/
}

void Modem_Receive_Check()
{
	TickType_t tickNow = xTaskGetTickCount();
	UartData_t* const uartData = &s.uartData[MODEM_UART_PORT];

	uint8_t rx_ch;
	uint8_t recv_length = 0;

	if (uartData->rxProcPtr != 0 && CalcTickDiff(uartData->rxTicks, xTaskGetTickCount()) > 2000) {
		uartData->rxTicks = xTaskGetTickCount();
		memset(uartData->rxBuff, 0x00, RX_BUFLEN);
		uartData->rxReadPtr = 0;
		uartData->rxProcPtr = 0;
		uartData->rxMode = 0;
		uartData->rxCount = 0;
		printf("RecvTimeout Buffer Clear \r\n", rx_ch);
		return;
	}

	while (uartData->rxReadPtr != uartData->rxProcPtr) {
		uartData->rxTicks = tickNow;
		rx_ch = uartData->rxBuff[uartData->rxProcPtr++];
		uartData->rxProcPtr %= RX_BUFLEN;
		//printf("[%02x]\r\n", rx_ch);

		switch (uartData->rxMode) {
			case 0:							// STX1
				if (rx_ch == 0x7E)
					uartData->rxMode++;

				uartData->rxCount = 0;
				//printf("[UART ] Modem_Receive_Check ", rx_ch);
				//printf("[%02x]", rx_ch);
				break;
			case 1:							// Length
				uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
				//printf("[%02x]", rx_ch);
				uartData->rxMaxCount = rx_ch;
				uartData->rxMaxCount--;
				uartData->rxMode++;
				break;
			case 2:							// data
				uartData->rxDataBuff[uartData->rxCount++] = rx_ch;
				uartData->rxMaxCount--;
				//printf("[%02x]", rx_ch);

				if(uartData->rxMaxCount == 0)
					uartData->rxMode++;

				break;
			case 3:							// ETX
				if (rx_ch == 0x7F) {
					uartData->rxRdy = pdTRUE;
					uartData->rxMode = 0;
					//printf("etx \r\n", rx_ch);
				}
				else
				{
					uartData->rxMode = 0;
					printf("ETX Error [%02x]\r\n", rx_ch);
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

	Modem_Data_Parse();
}

void Firmware_Data_Parse()
{
    UartData_t* const uartData = &s.uartData[BLE_UART_PORT];
    uint16_t recvCRC;
    uint16_t calcCRC;

    memset(&uartData->packet, 0x00, RX_BUFLEN);
    memset(&packet, 0x00, sizeof(COMMAND_T));

    memcpy(&uartData->packet,  &uartData->rxDataBuff[0], uartData->rxSize);

    calcCRC = 0;
    calcCRC = crc_16(&uartData->packet[0], uartData->rxSize - 2);
    recvCRC = uartData->packet[uartData->rxSize - 2] << 8 | uartData->packet[uartData->rxSize - 1];

	if (recvCRC) {
		if (recvCRC != calcCRC) {
			printf("\r\n[Modem] Rcv_LRC Error[%2X][%2X]\r\n", calcCRC, recvCRC);
			return;
		}
	}

    packet.opcode = uartData->packet[0];

    switch (packet.opcode)		//Opcode
    {
    	case FIRMWARE_CMD_REG_REQ:		//모뎀 상태 요청
    		packet.Result = pdTRUE;
    		osMessagePut(uartQueueHandle, (uint32_t) &packet, 0);
    		break;

    	case FIRMWARE_CMD_REG_SET:		//C-Resigter 설정
    		packet.Result = pdTRUE;
    		memcpy(&packet.data, &uartData->packet[5], 8);
    		osMessagePut(uartQueueHandle, (uint32_t) &packet, 0);
    		break;

    	case FIRMWARE_CMD_MODE:			//펌웨어 다운로드 모드 진입
			printf("[Firmware] Bootloader Mode \r\n");

			HAL_RTCEx_BKUPWrite(&hrtc, 0, 1);
			NVIC_SystemReset();
    		break;

    	case FIRMWARE_CMD_RESET:			//리셋명령
			NVIC_SystemReset();
    	    break;

    	default:
    		packet.Result = pdFALSE;
    		//osMessagePut(messageQueueHandle, (uint32_t) &packet, 0);
    		printf("\r\n[Firmware] OP_CODE Error[%2X]\r\n", packet.opcode);
    		break;
    }
}

uint8_t CalcLRC(uint8_t* data, uint16_t offset, uint32_t size)
{
    uint8_t lrc = 0;
    for (register uint32_t i = 0; i < size; i++)
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

static void init_crc16_tab( void )
{

    uint16_t i;
    uint16_t j;
    uint16_t crc;
    uint16_t c;

    for (i=0; i<256; i++) {

        crc = 0;
        c   = i;

        for (j=0; j<8; j++) {

            if ( (crc ^ c) & 0x0001 ) crc = ( crc >> 1 ) ^ CRC_POLY_16;
            else                      crc =   crc >> 1;

            c = c >> 1;
        }

        s.crc_tab16[i] = crc;
    }

    s.crc_tab16_init = 1;

}


uint16_t crc_16( const unsigned char *input_str, size_t num_bytes )
{
    uint16_t crc;
    uint16_t tmp;
    uint16_t short_c;
    const unsigned char *ptr;
    size_t a;

    if ( ! s.crc_tab16_init ) init_crc16_tab();

    crc = CRC_START_16;
    ptr = input_str;

    if ( ptr != NULL ) for (a=0; a<num_bytes; a++) {

        short_c = 0x00ff & (uint16_t) *ptr;
        tmp     =  crc       ^ short_c;
        crc     = (crc >> 8) ^ s.crc_tab16[ tmp & 0xff ];

        ptr++;
    }

    return crc;
}


uint16_t CalcCRC16(uint8_t* data, uint32_t size)
{
	uint16_t crc = 0x00;

	uint8_t *p = data;

	while (size--)
	{
		crc = crc16_tab[(crc ^ (*p++)) & 0xFF] ^ (crc >> 8);
	}

	return crc;
}
