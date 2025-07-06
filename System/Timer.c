#include "timer.h"
#include "led_driver.h"
#include "key.h"
#include "math.h"
#include "OLED.h"

void TIM2_Init(void) //1ms中断一次 用于key扫描
{
	
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//开启tim2的时钟
	
	 TIM_InternalClockConfig(TIM2);//设置tim2由内部时钟驱动
	
	  TIM_TimeBaseInitTypeDef tim_timebaseinitstructure;//定义时基结构体
	 tim_timebaseinitstructure.TIM_ClockDivision = TIM_CKD_DIV1;//1分频也就是不分频
	 tim_timebaseinitstructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	 tim_timebaseinitstructure.TIM_Period = 1000 - 1;//周期，arr自动重装器的值（取值范围0-65535）
	 tim_timebaseinitstructure.TIM_Prescaler = 72 - 1;//预分频，定时时间=(psc+1)(arr+1)/72mhz，1ms=72*1000/72mhz
	 tim_timebaseinitstructure.TIM_RepetitionCounter = 0;//重复计数器，高级定时器才有
	 TIM_TimeBaseInit(TIM2,&tim_timebaseinitstructure);//初始化时基单元
	
	 TIM_ClearFlag(TIM2,TIM_FLAG_Update);//清除更新中断标志位,计数从0开始
	 TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//开启更新中断到nvic
	
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断优先级分组2
	
	 NVIC_InitTypeDef nvic_initstructure;//定义中断结构体
	 nvic_initstructure.NVIC_IRQChannel = TIM2_IRQn;//tim2的中断通道
	 nvic_initstructure.NVIC_IRQChannelCmd = ENABLE;//使能通道
	 nvic_initstructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级2
	 nvic_initstructure.NVIC_IRQChannelSubPriority = 0;//响应优先级1
	 NVIC_Init(&nvic_initstructure);//初始化中断
	
	TIM_Cmd(TIM2,ENABLE);//启动定时器
}

void TIM3_Init(void) //10 ms中断一次 用于OLED刷新
{
	
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//开启tim2的时钟
	
	 TIM_InternalClockConfig(TIM3);//设置tim3由内部时钟驱动
	
	  TIM_TimeBaseInitTypeDef tim_timebaseinitstructure;//定义时基结构体
	 tim_timebaseinitstructure.TIM_ClockDivision = TIM_CKD_DIV1;//1分频也就是不分频
	 tim_timebaseinitstructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	 tim_timebaseinitstructure.TIM_Period = 1000 - 1;//周期，arr自动重装器的值（取值范围0-65535）
	 tim_timebaseinitstructure.TIM_Prescaler = 72 - 1;//预分频
	 tim_timebaseinitstructure.TIM_RepetitionCounter = 0;//重复计数器，高级定时器才有
	 TIM_TimeBaseInit(TIM3,&tim_timebaseinitstructure);//初始化时基单元
	
	 TIM_ClearFlag(TIM3,TIM_FLAG_Update);//清除更新中断标志位,计数从0开始
	 TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//开启更新中断到nvic
	
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断优先级分组2
	
	 NVIC_InitTypeDef nvic_initstructure;//定义中断结构体
	 nvic_initstructure.NVIC_IRQChannel = TIM3_IRQn;//tim2的中断通道
	 nvic_initstructure.NVIC_IRQChannelCmd = ENABLE;//使能通道
	 nvic_initstructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级2
	 nvic_initstructure.NVIC_IRQChannelSubPriority = 1;//响应优先级1
	 NVIC_Init(&nvic_initstructure);//初始化中断
	
	TIM_Cmd(TIM3,ENABLE);//启动定时器
}

