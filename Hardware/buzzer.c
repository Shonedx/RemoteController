#include "buzzer.h"

void Buzzer_Init(void)
{
	GPIO_InitTypeDef  GPIO_Initstructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_Initstructure);
	
	GPIO_SetBits(GPIOC,GPIO_Pin_13);

}
//·äÃùÆ÷´ò¿ª
void Buzzer_on(void)
{
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
}
//·äÃùÆ÷¹Ø±Õ
void Buzzer_off(void)
{
		GPIO_SetBits(GPIOC,GPIO_Pin_13);
}

void Buzzer_turn(void)//·äÃùÆ÷·­×ªº¯Êý
{
	if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13) == 1)
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
	}else{
		GPIO_SetBits(GPIOC,GPIO_Pin_13);
	}	
}

