#ifndef __AD_H
#define __AD_H
#define Rocker_Right_Y_Pin   GPIO_Pin_6 //PA6
#define Rocker_Right_X_Pin   GPIO_Pin_3 //PA3

#define Rocker_Left_Y_Pin   GPIO_Pin_2 //PA2
#define Rocker_Left_X_Pin   GPIO_Pin_1 //PA1

#define Bat_Pin   GPIO_Pin_4 //PA4

typedef enum
{
	Rocker_Right_X=0,
	Rocker_Right_Y,
	Rocker_Left_X,
	Rocker_Left_Y,
	Bat_Value
}AD_Num; //建立一个枚举用来简单表示AD采样数组中每一位代表这什么数据
void AD_Init();
void AD_GetValue(void);
void ADC1_Value_average(void);
extern uint16_t AD_Value[5];
extern uint32_t ALL_Sample_Buffer[4];
extern uint16_t AV_Sample_Buffer[4];
#endif