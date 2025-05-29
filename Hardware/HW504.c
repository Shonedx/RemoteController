#include "stm32f10x.h"                  // Device header
#include "AD.h"

/**************
Òý½ÅÁ¬½Ó£º

rocker_right_x - A3

rocker_right_y - A6

rocker_right_sw - PC15

rocker_left_x - A1

rocker_left_y - A2

rocker_left_sw - A8

***************/

#define Rocker_Right_SW_Pin GPIO_Pin_15 //PC15
#define Rocker_Right_SW_GPIOx GPIOC

#define Rocker_Left_SW_Pin GPIO_Pin_8 //PA8
#define Rocker_Left_SW_GPIOx GPIOA

void Rocker_SW_Init()
{
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = Rocker_Right_SW_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Rocker_Right_SW_GPIOx, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = Rocker_Left_SW_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Rocker_Left_SW_GPIOx, &GPIO_InitStructure);
	
	GPIO_SetBits(Rocker_Right_SW_GPIOx, Rocker_Right_SW_Pin);
	GPIO_SetBits(Rocker_Left_SW_GPIOx, Rocker_Left_SW_Pin);
}

uint8_t Rocker_Right_SW_Read()
{
	return GPIO_ReadInputDataBit(Rocker_Right_SW_GPIOx, Rocker_Right_SW_Pin);
}
uint8_t Rocker_Left_SW_Read()
{
	return GPIO_ReadInputDataBit(Rocker_Left_SW_GPIOx, Rocker_Left_SW_Pin);
}
//u8 HW504_R_X(void)
//{
//	return AD_Value[0];
//}

//u8 HW504_R_Y(void)
//{
//	return AD_Value[1];
//}


