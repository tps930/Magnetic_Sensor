/*
 * led.h
 *
 *  Created on: 2017. 6. 7.
 *      Author: Dongwon
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include "global.h"
#include <stdint.h>


void imgInit();
const LedData_t* Get_ImgData(const uint8_t no);
uint8_t Get_ImgCount();

#endif /* IMAGE_H_ */
