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
#include <stdio.h>  // ���� sprintf ����
#include <string.h> // ���� strlen ���� (��ѡ�����ڼ���ַ�������)
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

extern KeyStateInfo_t keystatebuffer[NUM_KEYS];
float BAT;//�����ص���
char bat_value_str_buffer[20]={0};

// �������ݻ�����
uint8_t tx_buffer[NRF_PAYLOAD_LENGTH]={0};

extern TX_CTX_t tx_context;
extern CtrlState_t ctrl_state;

volatile uint8_t rx_flag = 0; // ���ձ�־
volatile uint8_t tx_done = 0; // ������ɱ�־ ����������˷���ģʽ��

int main (void)
{

	IWDG_Init(IWDG_Prescaler_32, 1250); //1 s ��ʱ��ι��
	TIM2_Init();
	TIM3_Init();
	AD_Init();
	Rocker_SW_Init();//ҡ�˰�����ʼ��
	LED_Init();
	OLED_Init();
	 
	Key_Init(); //����GPIO���ó�ʼ��
	Key_InitState(); //����״̬����ʼ��

	 
	OLED_NewFrame();
	OLED_PrintASCIIString(10,20,"Init Done",&afont24x12,OLED_COLOR_NORMAL);
	OLED_ShowFrame();
	Delay_ms(500);
	OLED_NewFrame();

	// SPI��GPIO��ʼ�� (�������ͨ�ó�ʼ������)
    NRF24L01_Init_Common();
    // ��ʼ����Ϊ����ģʽ
    NRF24L01_Set_TX_Mode();

	 // ��ʼ������״̬��
    tx_context.state = TX_STATE_IDLE;


	while(1)
	{	
		if((BAT<3.29)|(BAT>=3.65))
		Buzzer_turn();			
		OLED_NewFrame();
		IWDG_Feed(); //ι��
		AD_GetValue(); //��ȡADģ��ת����õ�����ֵ
		BAT=(float)AD_Value[Bat_Value]/4095*3.3*3; //ң������ص���
		ADC1_Value_average(); //��ȡƽ�����ֵ
		
		Load_Data_to_Tx_Buffer();
		sprintf(bat_value_str_buffer, "%.2f", BAT); //�ѵ�ص���ת�����ַ���

		NRF24_Process(); //���Ͷ�״̬��

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
		
		//===================�༶�˵�����====================
//		OLED_NewFrame();	
//		menu_ctrl();
//		OLED_ShowFrame();
		


//==================================================
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
// TIM3 �жϷ�����
void TIM3_IRQHandler(void)
{
    // ����Ƿ��Ǹ����ж� (���������)
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
//		if(keystatebuffer[Keys_SW_UP].event_handle==Event_SingleClick) //�ϼ�
//		{
//			keystatebuffer[Keys_SW_UP].event_handle=Event_None;
//		}
		RC_TxPacket_Async(tx_buffer);
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

