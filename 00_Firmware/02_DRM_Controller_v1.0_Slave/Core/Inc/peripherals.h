/*
 * peripherals.h
 *
 *  Created on: 2018. 7. 6.
 *      Author: S.S.C
 */

#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_

#define PERI_LAMP1      0x01
#define PERI_LAMP2      0x02
#define PERI_LAMP3      0x04
#define PERI_LAMP4      0x08

#define PERI_SWITCH1    0x80
#define PERI_SWITCH2    0x40
#define PERI_SWITCH3    0x20
#define PERI_SWITCH4    0x10
#define PERI_SWITCH5    0x08
#define PERI_SWITCH6    0x04
#define PERI_SWITCH7    0x02
#define PERI_SWITCH8    0x01

#define PERI_OUTPUT1    0x01
#define PERI_OUTPUT2    0x02
#define PERI_OUTPUT3    0x04
#define PERI_OUTPUT4    0x08
#define PERI_OUTPUT5    0x10
#define PERI_OUTPUT6    0x20
#define PERI_OUTPUT7    0x40
#define PERI_OUTPUT8    0x80

#define PERI_INPUT1    	0x01
#define PERI_INPUT2    	0x02

#define SetLamp(state)     	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, state)

#define SetCH1(state)   	HAL_GPIO_WritePin(RF_CH1_GPIO_Port, RF_CH1_Pin, state)
#define SetCH2(state)   	HAL_GPIO_WritePin(RF_CH2_GPIO_Port, RF_CH2_Pin, state)
#define SetBAND(state)   	HAL_GPIO_WritePin(RF_B1_B2_GPIO_Port, RF_B1_B2_Pin, state)
#define SetOPT(state)   	HAL_GPIO_WritePin(RF_OPT_GPIO_Port, RF_OPT_Pin, state)
#define SetWAKEUP(state)   	HAL_GPIO_WritePin(RF_WUT_GPIO_Port, RF_WUT_Pin, state)
#define SetRFRst(state)   	HAL_GPIO_WritePin(RF_SW_GPIO_Port, RF_SW_Pin, state)

#define SetBLE_GPI(state)   	HAL_GPIO_WritePin(BLE_GPI_GPIO_Port, BLE_GPI_Pin, state)
#define SetBLE_SLEEP(state)   	HAL_GPIO_WritePin(BLE_SLEEP_GPIO_Port, BLE_SLEEP_Pin, state)

#define GetBLE_GPO()   		HAL_GPIO_ReadPin(BLE_GPO_GPIO_Port, BLE_GPO_Pin)

void periInit();
void periSetLamp(uint8_t lamp);
void periSetOutput(uint8_t output);
void dispPowerSet(uint8_t mode);
void periRFModule(uint8_t output);
void periRFSet(uint8_t mode);
void periRFReset();
void periSetBLE(uint8_t mode);
void periSetBLESleep(uint8_t sleep);

uint8_t periGetBLE();

#endif /* PERIPHERALS_H_ */
