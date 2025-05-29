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
#include <stdio.h>  // ���� sprintf ����
#include <string.h> // ���� strlen ���� (��ѡ�����ڼ���ַ�������)
extern KeyStateInfo_t keystatebuffer[NUM_KEYS];
float BAT;//�����ص���
char bat_value_str_buffer[20]={0};

// NRF24L01 ����ӳ�䵽STM32F103��GPIO
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


// ������ɱ�־ (���ط�ʧ�ܱ�־)
volatile uint8_t nrf_tx_complete_flag = 0; // 0:����, 1:���ͳɹ�, 2:�ط�ʧ��

 int main (void)
 {

	TIM2_Init();
//	TIM3_Init();
	AD_Init();
	Rocker_SW_Init();//ҡ�˰�����ʼ��
	LED_Init();
	OLED_Init();
	 
	Key_Init(); //����GPIO���ó�ʼ��
	Key_InitState(); //
	MenuCountInit();
	 
	OLED_NewFrame();
	OLED_PrintASCIIString(10,20,"Init Done",&afont24x12,OLED_COLOR_NORMAL);
	
	Delay_ms(500);
	OLED_NewFrame();

	//============UI�˵��õ������������޶���ʾ��Χ================
	static int current_index=0;
	static int first_index=0;
	//============
	
	//============

	//============
	// SPI��GPIO��ʼ�� (�������ͨ�ó�ʼ������)
    NRF24L01_Init_Common();
    // ��ʼ����Ϊ����ģʽ
    NRF24L01_Set_TX_Mode();

	while(1)
	{	
//		AD_GetValue(); //��ȡADģ��ת����õ�����ֵ
//		BAT=(float)AD_Value[Bat_Value]/4095*3.3*3; //ң������ص���
//		ADC1_Value_average(); //��ȡƽ�����ֵ
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
//        // NRF24L01_TxPacket �����ڲ��������ѯ�����󲿷�ʱ���ǵȴ�IRQ
//        uint8_t tx_result = NRF24L01_TxPacket(tx_buffer);

//        if(tx_result == 1)
//        {
////			OLED_PrintASCIIString(0,16,(char*)tx_buffer,&afont16x8,OLED_COLOR_NORMAL);
//           OLED_PrintASCIIString(0,0,"Success!",&afont16x8,OLED_COLOR_NORMAL);
//        }
//        else if(tx_result == 2)
//        {
//           
//            // ������Կ��ǲ��ԣ����罵�ͷ������ʣ������³�ʼ��ģ��
//			OLED_PrintASCIIString(0,0,"Max Sent Counts!",&afont16x8,OLED_COLOR_NORMAL);
//        }
//        else // tx_result == 0 (��ʱ)
//        {
//			OLED_PrintASCIIString(0,0,"Timeout!",&afont16x8,OLED_COLOR_NORMAL);
//          
//        }


//		OLED_ShowFrame();

		//===================�༶�˵�����====================
		OLED_NewFrame();
//	
		if(keystatebuffer[Keys_SW_UP].event_handle==Event_SingleClick) //�ϼ�
		{
			current_index--;
			current_index=constrain(0,currentMenuItemsBufferSize-1,current_index);
//			while(keystatebuffer[Keys_SW_UP].event_handle==Event_SingleClick);
			keystatebuffer[Keys_SW_UP].event_handle=Event_None;
		}
		if(keystatebuffer[Keys_SW_DOWN].event_handle==Event_SingleClick) //�¼�
		{
			current_index++;
			current_index=constrain(0,currentMenuItemsBufferSize-1,current_index);
//			while(keystatebuffer[Keys_SW_DOWN].event_handle==Event_SingleClick);
			keystatebuffer[Keys_SW_DOWN].event_handle=Event_None;
		}
		
		first_index=current_index-3;
		if(current_index<3)
			first_index=0;
		if(keystatebuffer[Keys_KEY_1].event_handle==Event_SingleClick) //Enter��
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
		if(keystatebuffer[Keys_WKUP].event_handle==Event_SingleClick) //Back��
		{
			if(currentMenu[current_index].parentMenu!=NULL)
			{
				int temp_offset=currentMenu[current_index].parentMenu->menuLocation; //�ó����صĸ��˵��������˵������׵�ַ��ƫ��ֵ
				currentMenu=currentMenu[current_index].parentMenu-temp_offset;//ʹ��currentMenu��ַΪ���˵Ĳ˵�������׵�ַ
				first_index=0;
				current_index=temp_offset; //ʹ�÷���ʱ��괦�ڸ��˵�λ��
				if(currentMenu->menuSize!=0)
				currentMenuItemsBufferSize=currentMenu->menuSize;	
			}
			keystatebuffer[Keys_WKUP].event_handle=Event_None;
		}
//		if(Key_Pressed(Keys_SW_UP)==0) //�ϼ�
//		{
//			current_index--;
//			current_index=constrain(0,currentMenuItemsBufferSize-1,current_index);
//			while(Key_Pressed(Keys_SW_UP)==0);
//		}
//		if(Key_Pressed(Keys_SW_DOWN)==0) //�¼�
//		{
//			current_index++;
//			current_index=constrain(0,currentMenuItemsBufferSize-1,current_index);
//			while(Key_Pressed(Keys_SW_DOWN)==0);
//		}
//		
//		first_index=current_index-3;
//		if(current_index<3)
//			first_index=0;
//		if(Key_Pressed(Keys_KEY_1)==0) //Enter��
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
//		if(Key_Pressed(Keys_WKUP)==0) //Back��
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
////============�ϴ��룬�����ݴ����й�===========

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
// �ⲿ�жϷ����� (PB11)
// ==============================================================================
void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(NRF_EXTI_LINE) != RESET) // ����Ƿ���PB11�ϵ��ж�
    {
//        uint8_t status = NRF24L01_Read_Reg(STATUS); // ��ȡ״̬�Ĵ���

//        if (status & (1<<TX_DS)) // ��������ж�
//        {
////			if(status&(1<<RX_DR)) //�յ����ն�Ӧ��
//            nrf_tx_complete_flag = 1;
//        }
//        else if (status & (1<<MAX_RT)) // ����ط��ж�
//        {
//            nrf_tx_complete_flag = 2;
//			NRF24L01_Write_Reg(FLUSH_TX , 0xFFFF);
//        }
//        // ���NRF24L01�ڲ����жϱ�־
//        NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, status);
		
        EXTI_ClearITPendingBit(NRF_EXTI_LINE); // ���MCU�ⲿ�жϱ�־
    }
}