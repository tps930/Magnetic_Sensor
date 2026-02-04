/*
 * uart.h
 *
 *  Created on: 2021. 4. 27.
 *      Author: user
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "stm32l0xx_hal.h"

#define COMM_UART1  			0		//Debug
#define COMM_UART4  			1	 	//RF Modem
#define COMM_UART5  			2	 	//BLE


#define CRC_TABLE_SIZE 		256

#define DEBUG_UART_PORT 		COMM_UART1
#define MODEM_UART_PORT			COMM_UART4
#define BLE_UART_PORT  			COMM_UART5

#define UART_COUNT      		3
#define RX_BUFLEN       		1024
#define TX_BUFLEN       		256

#define CRC_POLY_16     0xA001
#define CRC_START_16    0x0000

/* End of the Flash address */
#define USER_FLASH_END_ADDRESS        0x0801FF80
/* Define the user application size */
#define USER_FLASH_SIZE   (USER_FLASH_END_ADDRESS - BOOT_ADDRESS + 1)

/* Define the address from where user application will be loaded.
   Note: the 1st sector 0x08000000-0x08003FFF is reserved for the IAP code */
#define BOOT_ADDRESS  ((uint32_t)0x08010000) /* Base @ of Sector 32, 2 Kbyte */

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
    uint16_t rxSize;
    TickType_t rxTicks;

    uint8_t txBuff[TX_BUFLEN];
    __IO uint8_t txDmaReady;

   // uint8_t		data[RX_BUFLEN];
    uint8_t		packet[RX_BUFLEN];
    uint32_t	packet_len;
} UartData_t;

typedef struct
{
    UartData_t uartData[UART_COUNT];

    uint8_t crc_tab16_init;
    uint16_t crc_tab16[256];
} CommStatus_t;

CommStatus_t s;

void commInit();
void UART_Baud(UART_HandleTypeDef* huart, uint32_t baud);
void commSendData(uint8_t port, uint8_t* data, uint16_t size);
uint16_t CalcCheckSum(uint8_t* data, uint16_t offset, uint32_t size);
uint8_t CalcLRC(uint8_t* data, uint16_t offset, uint32_t size);
uint8_t CalcSum(uint8_t* data, uint16_t offset, uint32_t size);
uint16_t crc_16( const unsigned char *input_str, size_t num_bytes );
uint16_t CalcCRC16(uint8_t* data, uint32_t size);
void MCU_Receive_Check();
void MCU_Data_Parse();

#endif /* INC_UART_H_ */
