#include "timer.h"
#include "led_driver.h"
#include "key.h"
#include "math.h"
#include "OLED.h"

void TIM2_Init(void) //1ms�ж�һ�� ����keyɨ��
{
	
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//����tim2��ʱ��
	
	 TIM_InternalClockConfig(TIM2);//����tim2���ڲ�ʱ������
	
	  TIM_TimeBaseInitTypeDef tim_timebaseinitstructure;//����ʱ���ṹ��
	 tim_timebaseinitstructure.TIM_ClockDivision = TIM_CKD_DIV1;//1��ƵҲ���ǲ���Ƶ
	 tim_timebaseinitstructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
	 tim_timebaseinitstructure.TIM_Period = 1000 - 1;//���ڣ�arr�Զ���װ����ֵ��ȡֵ��Χ0-65535��
	 tim_timebaseinitstructure.TIM_Prescaler = 72 - 1;//Ԥ��Ƶ����ʱʱ��=(psc+1)(arr+1)/72mhz��1ms=72*1000/72mhz
	 tim_timebaseinitstructure.TIM_RepetitionCounter = 0;//�ظ����������߼���ʱ������
	 TIM_TimeBaseInit(TIM2,&tim_timebaseinitstructure);//��ʼ��ʱ����Ԫ
	
	 TIM_ClearFlag(TIM2,TIM_FLAG_Update);//��������жϱ�־λ,������0��ʼ
	 TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//���������жϵ�nvic
	
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�ж����ȼ�����2
	
	 NVIC_InitTypeDef nvic_initstructure;//�����жϽṹ��
	 nvic_initstructure.NVIC_IRQChannel = TIM2_IRQn;//tim2���ж�ͨ��
	 nvic_initstructure.NVIC_IRQChannelCmd = ENABLE;//ʹ��ͨ��
	 nvic_initstructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�2
	 nvic_initstructure.NVIC_IRQChannelSubPriority = 0;//��Ӧ���ȼ�1
	 NVIC_Init(&nvic_initstructure);//��ʼ���ж�
	
	TIM_Cmd(TIM2,ENABLE);//������ʱ��
}

void TIM3_Init(void) //10 ms�ж�һ�� ����OLEDˢ��
{
	
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//����tim2��ʱ��
	
	 TIM_InternalClockConfig(TIM3);//����tim3���ڲ�ʱ������
	
	  TIM_TimeBaseInitTypeDef tim_timebaseinitstructure;//����ʱ���ṹ��
	 tim_timebaseinitstructure.TIM_ClockDivision = TIM_CKD_DIV1;//1��ƵҲ���ǲ���Ƶ
	 tim_timebaseinitstructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
	 tim_timebaseinitstructure.TIM_Period = 1000 - 1;//���ڣ�arr�Զ���װ����ֵ��ȡֵ��Χ0-65535��
	 tim_timebaseinitstructure.TIM_Prescaler = 72 - 1;//Ԥ��Ƶ
	 tim_timebaseinitstructure.TIM_RepetitionCounter = 0;//�ظ����������߼���ʱ������
	 TIM_TimeBaseInit(TIM3,&tim_timebaseinitstructure);//��ʼ��ʱ����Ԫ
	
	 TIM_ClearFlag(TIM3,TIM_FLAG_Update);//��������жϱ�־λ,������0��ʼ
	 TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//���������жϵ�nvic
	
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�ж����ȼ�����2
	
	 NVIC_InitTypeDef nvic_initstructure;//�����жϽṹ��
	 nvic_initstructure.NVIC_IRQChannel = TIM3_IRQn;//tim2���ж�ͨ��
	 nvic_initstructure.NVIC_IRQChannelCmd = ENABLE;//ʹ��ͨ��
	 nvic_initstructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�2
	 nvic_initstructure.NVIC_IRQChannelSubPriority = 1;//��Ӧ���ȼ�1
	 NVIC_Init(&nvic_initstructure);//��ʼ���ж�
	
	TIM_Cmd(TIM3,ENABLE);//������ʱ��
}

