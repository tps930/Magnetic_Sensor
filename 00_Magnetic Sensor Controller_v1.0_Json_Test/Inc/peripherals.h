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
#define PERI_LAMP5      0x10
#define PERI_LAMP6      0x20
#define PERI_LAMP7      0x40
#define PERI_LAMP8      0x80

#define PERI_SWITCH1    0x01
#define PERI_SWITCH2    0x02
#define PERI_SWITCH3    0x04
#define PERI_SWITCH4    0x08
#define PERI_SWITCH5    0x10
#define PERI_SWITCH6    0x20
#define PERI_SWITCH7    0x40
#define PERI_SWITCH8    0x80

#define PERI_OUTPUT1    0x01
#define PERI_OUTPUT2    0x02
#define PERI_OUTPUT3    0x04
#define PERI_OUTPUT4    0x08
#define PERI_OUTPUT5    0x10
#define PERI_OUTPUT6    0x20
#define PERI_OUTPUT7    0x40
#define PERI_OUTPUT8    0x80

#define PERI_INPUT1    0x01
#define PERI_INPUT2    0x02
#define PERI_INPUT3    0x04
#define PERI_INPUT4    0x08

#define PERI_DRDY      0x80

#define SetLamp1(state)     HAL_GPIO_WritePin(LAMP_LED0_GPIO_Port, LAMP_LED0_Pin, state)
#define SetLamp2(state)     HAL_GPIO_WritePin(LAMP_LED1_GPIO_Port, LAMP_LED1_Pin, state)
#define SetLamp3(state)     HAL_GPIO_WritePin(LAMP_LED2_GPIO_Port, LAMP_LED2_Pin, state)
#define SetLamp4(state)     HAL_GPIO_WritePin(LAMP_LED3_GPIO_Port, LAMP_LED3_Pin, state)
#define SetLamp5(state)     HAL_GPIO_WritePin(LAMP_LED4_GPIO_Port, LAMP_LED4_Pin, state)
#define SetLamp6(state)     HAL_GPIO_WritePin(LAMP_LED5_GPIO_Port, LAMP_LED5_Pin, state)
#define SetLamp7(state)     HAL_GPIO_WritePin(LAMP_LED6_GPIO_Port, LAMP_LED6_Pin, state)
#define SetLamp8(state)     HAL_GPIO_WritePin(LAMP_LED7_GPIO_Port, LAMP_LED7_Pin, state)


#define GetSwitch1()        HAL_GPIO_ReadPin(DIP_SW0_GPIO_Port, DIP_SW0_Pin)
#define GetSwitch2()        HAL_GPIO_ReadPin(DIP_SW1_GPIO_Port, DIP_SW1_Pin)
#define GetSwitch3()        HAL_GPIO_ReadPin(DIP_SW2_GPIO_Port, DIP_SW2_Pin)
#define GetSwitch4()        HAL_GPIO_ReadPin(DIP_SW3_GPIO_Port, DIP_SW3_Pin)
#define GetSwitch5()        HAL_GPIO_ReadPin(DIP_SW4_GPIO_Port, DIP_SW4_Pin)
#define GetSwitch6()        HAL_GPIO_ReadPin(DIP_SW5_GPIO_Port, DIP_SW5_Pin)
#define GetSwitch7()        HAL_GPIO_ReadPin(DIP_SW6_GPIO_Port, DIP_SW6_Pin)
#define GetSwitch8()        HAL_GPIO_ReadPin(DIP_SW7_GPIO_Port, DIP_SW7_Pin)



#define SetOutput1(state)   HAL_GPIO_WritePin(GP_OUT0_GPIO_Port, GP_OUT0_Pin, state)
#define SetOutput2(state)   HAL_GPIO_WritePin(GP_OUT1_GPIO_Port, GP_OUT1_Pin, state)
#define SetOutput3(state)   HAL_GPIO_WritePin(GP_OUT2_GPIO_Port, GP_OUT2_Pin, state)
#define SetOutput4(state)   HAL_GPIO_WritePin(GP_OUT3_GPIO_Port, GP_OUT3_Pin, state)
#define SetOutput5(state)   HAL_GPIO_WritePin(GP_OUT4_GPIO_Port, GP_OUT4_Pin, state)
#define SetOutput6(state)   HAL_GPIO_WritePin(GP_OUT5_GPIO_Port, GP_OUT5_Pin, state)
#define SetOutput7(state)   HAL_GPIO_WritePin(GP_OUT6_GPIO_Port, GP_OUT6_Pin, state)
#define SetOutput8(state)   HAL_GPIO_WritePin(GP_OUT7_GPIO_Port, GP_OUT7_Pin, state)

#define GetInput1()	 		HAL_GPIO_ReadPin(GPIO_INPUT0_GPIO_Port, GPIO_INPUT0_Pin)
#define GetInput2()	 		HAL_GPIO_ReadPin(GPIO_INPUT1_GPIO_Port, GPIO_INPUT1_Pin)
#define GetInput3()	 		HAL_GPIO_ReadPin(GPIO_INPUT2_GPIO_Port, GPIO_INPUT2_Pin)
#define GetInput4()	 		HAL_GPIO_ReadPin(GPIO_INPUT3_GPIO_Port, GPIO_INPUT3_Pin)

#define GetDRDYInput()		HAL_GPIO_ReadPin(DRDY_INPUT_GPIO_Port, DRDY_INPUT_Pin)

void periInit();
void periSetLamp(uint8_t lamp);
uint8_t periGetLamp();
uint8_t periGetSwitch();
void periSetOutput(uint8_t output);
uint8_t periGetInput();

#endif /* PERIPHERALS_H_ */
