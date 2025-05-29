#ifndef __LED_DRIVER_H
#define __LED_DRIVER_H

#include "stm32f10x.h"

#define LED2  GPIO_Pin_4 //PB
#define LED3  GPIO_Pin_5

void LED_Init(void);

void led2_on(void);
void led2_off(void);
void led3_on(void);
void led3_off(void);

void led2_turn(void);
void led3_turn(void);
	

#endif

