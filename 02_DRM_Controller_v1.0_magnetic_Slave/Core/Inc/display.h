/*
 * display.h
 *
 *  Created on: 2018. 9. 13.
 *      Author: ShinSeungCheol
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include "logic.h"

void dispInit();
void dispSetData(LEDDATA_T* data);
void dispRefresh();
static void DoWriteConfig();
static void DoDotCorrection(uint8_t dot);
static void DisplayLDT1074Module(const LEDDATA_T* const data);
void dispSetBright(uint32_t brtR);

#endif /* DISPLAY_H_ */
