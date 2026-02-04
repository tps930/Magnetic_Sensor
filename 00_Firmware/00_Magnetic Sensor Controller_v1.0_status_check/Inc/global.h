/*
 * global.h
 *
 *  Created on: 2017. 6. 7.
 *      Author: Dongwon
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>

#define CRC_WIDTH       64				//2^n 승
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

#define CALIB_SAMPLES          50      // 10 Hz에서 5초 동안 수집 → 50개가 딱 맞습니다
#define K_SIGMA                4.5f    // 필드용 권장 (오검 줄임). 벤치에서 약하면 4.2까지
#define T_ABS_MIN              12      // ≈ 1.8 µT (LIS2MDL ~0.15 µT/LSB 가정)
#define EMA_BETA_PERMILLE      1       // idle일 때 기준선 아주 느리게 추적(0.1%/샘플 ≈ 100 s)
#define IDLE_BAND_DIV          3       // 기준선 추적은 ±T/3 안에서만 (이벤트에 끌려가지 않음)
#define DEBOUNCE_MS            150     // 10 Hz에서 최소 2샘플(≥200 ms) 권장, 시작값 150 ms
#define REFRACT_MS             300     // 0.3 s(=3샘플) 휴지구간 → 재중복 방지


static const uint8_t BITMASK_DCR[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, };
static const uint8_t BITMASK_ICR[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, };

typedef struct
{
    uint8_t red[LED_BUFSIZE];
    uint8_t grn[LED_BUFSIZE];
    uint8_t blu[LED_BUFSIZE];
} LedData_t;

#endif /* GLOBAL_H_ */
