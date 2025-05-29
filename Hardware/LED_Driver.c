#include "led_driver.h"

/*LED��GPIO���ú���*/
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_Initstructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST,ENABLE);		// PB4��JTRSTʧ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_Initstructure.GPIO_Pin = LED2 | LED3;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_Initstructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_4|GPIO_Pin_5);				// PB4��PB5�����
}

void led2_on(void)//LED2��
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_4);
}

void led2_off(void)//LED2�ر�
{
	GPIO_SetBits(GPIOB,GPIO_Pin_4);
}

void led2_turn(void)//LED2��ת����
{
	if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_4) == 1)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_4);
	}else{
		GPIO_SetBits(GPIOB,GPIO_Pin_4);
	}	
}

void led3_on(void)//LED3��
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
}

void led3_off(void)//LED3�ر�
{
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
}

void led3_turn(void)//LED3��ת����
{
	if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_5) == 1)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	}else{
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
	}	
}



