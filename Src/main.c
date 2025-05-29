#include "stm32f10x.h"                  // Device header
#include "Delay.h" //Delay
#include "OLED.h"
#include "NRF24L01.h"


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
extern KeyStateInfo_t keystatebuffer[NUM_KEYS];
float BAT;//储存电池电量
char bat_value_str_buffer[20]={0};

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


// 发送完成标志 (或重发失败标志)
volatile uint8_t nrf_tx_complete_flag = 0; // 0:空闲, 1:发送成功, 2:重发失败

 int main (void)
 {

	TIM2_Init();
//	TIM3_Init();
	AD_Init();
	Rocker_SW_Init();//摇杆按键初始化
	LED_Init();
	OLED_Init();
	 
	Key_Init(); //按键GPIO配置初始化
	Key_InitState(); //
	MenuCountInit();
	 
	OLED_NewFrame();
	OLED_PrintASCIIString(10,20,"Init Done",&afont24x12,OLED_COLOR_NORMAL);
	
	Delay_ms(500);
	OLED_NewFrame();

	//============UI菜单用的索引，用来限定显示范围================
	static int current_index=0;
	static int first_index=0;
	//============
	
	//============

	//============
	// SPI和GPIO初始化 (这里调用通用初始化函数)
    NRF24L01_Init_Common();
    // 初始设置为发送模式
    NRF24L01_Set_TX_Mode();

	while(1)
	{	
//		AD_GetValue(); //获取AD模数转化后得到的数值
//		BAT=(float)AD_Value[Bat_Value]/4095*3.3*3; //遥控器电池电量
//		ADC1_Value_average(); //获取平均后的值
//		
//		Load_Data_to_Tx_Buffer();
//		sprintf(bat_value_str_buffer, "%.2f", BAT);

//		
//		OLED_NewFrame();
//		
//		OLED_PrintASCIIString(0,16,bat_value_str_buffer,&afont16x8,OLED_COLOR_NORMAL);
////		OLED_PrintASCIINum(0,32,tx_buffer[18],1,&afont16x8,OLED_COLOR_NORMAL);
////		OLED_PrintASCIINum(16,32,tx_buffer[19],1,&afont16x8,OLED_COLOR_NORMAL);
////		OLED_PrintASCIINum(32,32,tx_buffer[2],1,&afont16x8,OLED_COLOR_NORMAL);
////		OLED_PrintASCIINum(48,32,tx_buffer[3],1,&afont16x8,OLED_COLOR_NORMAL);
//		OLED_PrintASCIINum(0,32,(uint16_t)tx_buffer[11]<<8|((uint16_t)tx_buffer[10]),4,&afont16x8,OLED_COLOR_NORMAL);
//		OLED_PrintASCIINum(40,32,(uint16_t)tx_buffer[13]<<8|((uint16_t)tx_buffer[12]),4,&afont16x8,OLED_COLOR_NORMAL);
//		OLED_PrintASCIINum(0,48,(uint16_t)tx_buffer[15]<<8|((uint16_t)tx_buffer[14]),4,&afont16x8,OLED_COLOR_NORMAL);
//		OLED_PrintASCIINum(40,48,(uint16_t)tx_buffer[17]<<8|((uint16_t)tx_buffer[16]),4,&afont16x8,OLED_COLOR_NORMAL);
//		
//        // NRF24L01_TxPacket 函数内部会短暂轮询，但大部分时间是等待IRQ
//        uint8_t tx_result = NRF24L01_TxPacket(tx_buffer);

//        if(tx_result == 1)
//        {
////			OLED_PrintASCIIString(0,16,(char*)tx_buffer,&afont16x8,OLED_COLOR_NORMAL);
//           OLED_PrintASCIIString(0,0,"Success!",&afont16x8,OLED_COLOR_NORMAL);
//        }
//        else if(tx_result == 2)
//        {
//           
//            // 这里可以考虑策略，例如降低发送速率，或重新初始化模块
//			OLED_PrintASCIIString(0,0,"Max Sent Counts!",&afont16x8,OLED_COLOR_NORMAL);
//        }
//        else // tx_result == 0 (超时)
//        {
//			OLED_PrintASCIIString(0,0,"Timeout!",&afont16x8,OLED_COLOR_NORMAL);
//          
//        }


//		OLED_ShowFrame();

		//===================多级菜单代码====================
		OLED_NewFrame();
//	
		if(keystatebuffer[Keys_SW_UP].event_handle==Event_SingleClick) //上键
		{
			current_index--;
			current_index=constrain(0,currentMenuItemsBufferSize-1,current_index);
//			while(keystatebuffer[Keys_SW_UP].event_handle==Event_SingleClick);
			keystatebuffer[Keys_SW_UP].event_handle=Event_None;
		}
		if(keystatebuffer[Keys_SW_DOWN].event_handle==Event_SingleClick) //下键
		{
			current_index++;
			current_index=constrain(0,currentMenuItemsBufferSize-1,current_index);
//			while(keystatebuffer[Keys_SW_DOWN].event_handle==Event_SingleClick);
			keystatebuffer[Keys_SW_DOWN].event_handle=Event_None;
		}
		
		first_index=current_index-3;
		if(current_index<3)
			first_index=0;
		if(keystatebuffer[Keys_KEY_1].event_handle==Event_SingleClick) //Enter键
		{
			if(currentMenu[current_index].subMenu!=NULL)
			{
				currentMenu=currentMenu[current_index].subMenu;
				first_index=0;
				current_index=0;
				if(currentMenu->menuSize!=0)
				currentMenuItemsBufferSize=currentMenu->menuSize;
			}
			keystatebuffer[Keys_KEY_1].event_handle=Event_None;
		}
		if(keystatebuffer[Keys_WKUP].event_handle==Event_SingleClick) //Back键
		{
			if(currentMenu[current_index].parentMenu!=NULL)
			{
				int temp_offset=currentMenu[current_index].parentMenu->menuLocation; //得出返回的父菜单相对于其菜单数组首地址的偏移值
				currentMenu=currentMenu[current_index].parentMenu-temp_offset;//使得currentMenu地址为父菜的菜单数组的首地址
				first_index=0;
				current_index=temp_offset; //使得返回时光标处于父菜单位置
				if(currentMenu->menuSize!=0)
				currentMenuItemsBufferSize=currentMenu->menuSize;	
			}
			keystatebuffer[Keys_WKUP].event_handle=Event_None;
		}
//		if(Key_Pressed(Keys_SW_UP)==0) //上键
//		{
//			current_index--;
//			current_index=constrain(0,currentMenuItemsBufferSize-1,current_index);
//			while(Key_Pressed(Keys_SW_UP)==0);
//		}
//		if(Key_Pressed(Keys_SW_DOWN)==0) //下键
//		{
//			current_index++;
//			current_index=constrain(0,currentMenuItemsBufferSize-1,current_index);
//			while(Key_Pressed(Keys_SW_DOWN)==0);
//		}
//		
//		first_index=current_index-3;
//		if(current_index<3)
//			first_index=0;
//		if(Key_Pressed(Keys_KEY_1)==0) //Enter键
//		{
//			if(currentMenu[current_index].subMenu!=NULL)
//			{
//				currentMenu=currentMenu[current_index].subMenu;
//				first_index=0;
//				current_index=0;
//				if(currentMenu->menuSize!=0)
//				currentMenuItemsBufferSize=currentMenu->menuSize;
//			}
//		}
//		if(Key_Pressed(Keys_WKUP)==0) //Back键
//		{
//			if(currentMenu[current_index].parentMenu!=NULL)
//			{
//				
//				currentMenu=currentMenu[current_index].parentMenu;
//				first_index=0;
//				current_index=0;
//				if(currentMenu->menuSize!=0)
//				currentMenuItemsBufferSize=currentMenu->menuSize;
//				
//			}
//		}
		MenuDisplay(first_index,current_index);
		OLED_ShowFrame();
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
 // ==============================================================================
// 外部中断服务函数 (PB11)
// ==============================================================================
void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(NRF_EXTI_LINE) != RESET) // 检查是否是PB11上的中断
    {
//        uint8_t status = NRF24L01_Read_Reg(STATUS); // 读取状态寄存器

//        if (status & (1<<TX_DS)) // 发送完成中断
//        {
////			if(status&(1<<RX_DR)) //收到接收端应答
//            nrf_tx_complete_flag = 1;
//        }
//        else if (status & (1<<MAX_RT)) // 最大重发中断
//        {
//            nrf_tx_complete_flag = 2;
//			NRF24L01_Write_Reg(FLUSH_TX , 0xFFFF);
//        }
//        // 清除NRF24L01内部的中断标志
//        NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, status);
		
        EXTI_ClearITPendingBit(NRF_EXTI_LINE); // 清除MCU外部中断标志
    }
}