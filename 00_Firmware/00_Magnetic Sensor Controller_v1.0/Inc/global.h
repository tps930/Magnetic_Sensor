/*
 * global.h
 *
 *  Created on: 2017. 6. 7.
 *      Author: Dongwon
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>

#define CRC_WIDTH       64				//2^n ½Â
#define CRC_HEIGHT      16
#define CRC_STRIDE      (CRC_WIDTH/8)

#define LED_MODULE_WIDTH       	32
#define LED_MODULE_HEIGHT      	16
#define LED_MODULE_COUNT   		2
#define LED_MODULE_STRIDE      	(LED_MODULE_WIDTH/8)
#define LED_MODULE_H_STRIDE      (LED_MODULE_STRIDE/2)
#define LED_MODULE_V_STRIDE      (LED_MODULE_STRIDE/2)

#define LED_WIDTH       LED_MODULE_WIDTH*LED_MODULE_COUNT
#define LED_HEIGHT      LED_MODULE_HEIGHT
#define LED_STRIDE      (LED_WIDTH/8)
#define LED_BUFSIZE     (LED_STRIDE*LED_HEIGHT)


static const uint8_t BITMASK_DCR[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, };
static const uint8_t BITMASK_ICR[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, };

typedef struct
{
    uint8_t red[LED_BUFSIZE];
    uint8_t grn[LED_BUFSIZE];
    uint8_t blu[LED_BUFSIZE];
} LedData_t;

#endif /* GLOBAL_H_ */
