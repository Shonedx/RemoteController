#include "stm32f10x.h"                  // Device header
#include "Delay.h" //Delay
#include "OLED.h"
#include "NRF24L01.h"
#include "RC.h"
#include "IWDG.h"
#include "HW504.h"
#include "font.h"
#include "Key.h"
#include "LED_DRIVER.h"
#include "AD.h"
#include "timer.h"
#include "UI.h"
#include "buzzer.h"
#include <stdio.h>  // 包含 sprintf 函数
#include <string.h> // 包含 strlen 函数 (可选，用于检查字符串长度)
// NRF24L01 引脚映射到STM32F103的GPIO
GPIO_TypeDef* NRF_CE_Port = GPIOB;
uint16_t      NRF_CE_Pin = GPIO_Pin_12;
GPIO_TypeDef* NRF_CSN_Port = GPIOB;
uint16_t      NRF_CSN_Pin = GPIO_Pin_10;
GPIO_TypeDef* NRF_IRQ_Port = GPIOB;
uint16_t      NRF_IRQ_Pin = GPIO_Pin_11;
GPIO_TypeDef* NRF_MOSI_Port = GPIOB; // SPI2
uint16_t      NRF_MOSI_Pin = GPIO_Pin_15;
GPIO_TypeDef* NRF_MISO_Port = GPIOB; // SPI2
uint16_t      NRF_MISO_Pin = GPIO_Pin_14;
GPIO_TypeDef* NRF_SCK_Port = GPIOB;  // SPI2
uint16_t      NRF_SCK_Pin = GPIO_Pin_13;

extern KeyStateInfo_t keystatebuffer[NUM_KEYS];
float BAT;//储存电池电量
char bat_value_str_buffer[20]={0};

// 发送数据缓冲区
uint8_t tx_buffer[NRF_PAYLOAD_LENGTH]={0};

extern TX_CTX_t tx_context;
extern CtrlState_t ctrl_state;

volatile uint8_t rx_flag = 0; // 接收标志
volatile uint8_t tx_done = 0; // 发送完成标志 （如果启用了发射模式）

int main (void)
{

	IWDG_Init(IWDG_Prescaler_32, 1250); //1 s 超时就喂狗
	TIM2_Init();
	TIM3_Init();
	AD_Init();
	Rocker_SW_Init();//摇杆按键初始化
	LED_Init();
	OLED_Init();
	 
	Key_Init(); //按键GPIO配置初始化
	Key_InitState(); //按键状态机初始化

	 
	OLED_NewFrame();
	OLED_PrintASCIIString(10,20,"Init Done",&afont24x12,OLED_COLOR_NORMAL);
	OLED_ShowFrame();
	Delay_ms(500);
	OLED_NewFrame();

	// SPI和GPIO初始化 (这里调用通用初始化函数)
    NRF24L01_Init_Common();
    // 初始设置为发送模式
    NRF24L01_Set_TX_Mode();

	 // 初始化发送状态机
    tx_context.state = TX_STATE_IDLE;


	while(1)
	{	
		if((BAT<3.29)|(BAT>=3.65))
		Buzzer_turn();			
		OLED_NewFrame();
		IWDG_Feed(); //喂狗
		AD_GetValue(); //获取AD模数转化后得到的数值
		BAT=(float)AD_Value[Bat_Value]/4095*3.3*3; //遥控器电池电量
		ADC1_Value_average(); //获取平均后的值
		
		Load_Data_to_Tx_Buffer();
		sprintf(bat_value_str_buffer, "%.2f", BAT); //把电池电量转换成字符串

		NRF24_Process(); //发送端状态机

		menu_ctrl();
		
//		OLED_PrintASCIIString(0,0,bat_value_str_buffer,&afont24x12,OLED_COLOR_NORMAL);
//		OLED_NewFrame();
//		OLED_PrintASCIINum(0,32,(uint16_t)tx_buffer[11]<<8|((uint16_t)tx_buffer[10]),4,&afont16x8,OLED_COLOR_NORMAL);
//		OLED_PrintASCIINum(40,32,(uint16_t)tx_buffer[13]<<8|((uint16_t)tx_buffer[12]),4,&afont16x8,OLED_COLOR_NORMAL);
//		OLED_PrintASCIINum(0,48,(uint16_t)tx_buffer[15]<<8|((uint16_t)tx_buffer[14]),4,&afont16x8,OLED_COLOR_NORMAL);
//		OLED_PrintASCIINum(40,48,(uint16_t)tx_buffer[17]<<8|((uint16_t)tx_buffer[16]),4,&afont16x8,OLED_COLOR_NORMAL);
//		if(tx_done==1)
//		OLED_PrintASCIIString(0,0,"tx achive!",&afont16x8,OLED_COLOR_NORMAL);
//		else if(tx_done==2)
//		OLED_PrintASCIIString(0,0,"tx error! ",&afont16x8,OLED_COLOR_NORMAL);
////		else if(tx_done==0)
////		OLED_PrintASCIIString(0,16,"tx_done",&afont16x8,OLED_COLOR_NORMAL);
		OLED_ShowFrame();
		
		//===================多级菜单代码====================
//		OLED_NewFrame();	
//		menu_ctrl();
//		OLED_ShowFrame();
		


//==================================================
////============老代码，和数据传输有关===========

////		Key_GetNum();
////		AD_GetValue();
////		BAT=(float)AD_Value[2]/4095*3.3*3;
////		ADC1_Value_average();
////		OLED_NewFrame();

////		if(BAT<3.29)
////		{
////			OLED_PrintASCIIString(4,52,"Battery in danger!!!",&afont12x6,OLED_COLOR_NORMAL);
////		}
//////		else
//////			OLED_PrintASCIINum(64-3,55,BAT,2,&afont8x6,OLED_COLOR_NORMAL);
////		for(int i=0;i<10;i++)
////		{
////			Send_Data_Buf[i]=F_KEY[i];
////		}
////		
////		if(RF2G4_Tx_Packet(Send_Data_Buf,16)==TX_SUCCESS)
////		{
////			OLED_PrintASCIIString(0,0,"CONNECTED",&afont24x12,OLED_COLOR_NORMAL);
////		}
////		
////		else
////		OLED_PrintASCIIString(43,0,"waiting",&afont12x6,OLED_COLOR_NORMAL);
////		UI_Ctrl();
////		

////		OLED_ShowFrame();
////	 	Delay_ms(100);
//		OLED_ShowFrame();
	}
}
// TIM3 中断服务函数
void TIM3_IRQHandler(void)
{
    // 检查是否是更新中断 (计数器溢出)
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
//		if(keystatebuffer[Keys_SW_UP].event_handle==Event_SingleClick) //上键
//		{
//			keystatebuffer[Keys_SW_UP].event_handle=Event_None;
//		}
		RC_TxPacket_Async(tx_buffer);
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

