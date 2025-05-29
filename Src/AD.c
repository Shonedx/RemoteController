#include "stm32f10x.h"                  // Device header
#include "AD.h"
#define CHANNEL_COUNTS 5 //ͨ��������Ҳ����������ʾdma�������
uint16_t AD_Value[5];//���Right_X,Right_Y,Left_X,Left_Y,Bat_Value
uint32_t ALL_Sample_Buffer[4] = {0} ;	// �����ܼ�
// ALL_Sample_Buffer[0] ---- ALL_R_X_Sample ��ҡ��x�����ܼ�
// ALL_Sample_Buffer[1] ---- ALL_R_Y_Sample ��ҡ��y�����ܼ�
// ALL_Sample_Buffer[2] ---- ALL_L_X_Sample ��ҡ��x�����ܼ�
// ALL_Sample_Buffer[3] ---- ALL_L_Y_Sample ��ҡ��y�����ܼ�
uint16_t AV_Sample_Buffer[4] = {2053,2019,2053,2019} ;	//ƽ������
// AV_Sample_Buffer[0] ---- AV_R_X_Sample ADC_CH3ƽ��ֵ
// AV_Sample_Buffer[1] ---- AV_R_Y_Sample ADC_CH6ƽ��ֵ
// AV_Sample_Buffer[2] ---- AV_L_X_Sample ADC_CH1ƽ��ֵ
// AV_Sample_Buffer[3] ---- AV_L_Y_Sample ADC_CH2ƽ��ֵ

void AD_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);		//����ADC1�ڲ�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//����GPIOA�ڲ�ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//����DMA1ʱ��

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	//12Mz, ����ADC��Ƶ��*2��*4��6��8��
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ�����룬ADCר��
	GPIO_InitStructure.GPIO_Pin = Rocker_Left_X_Pin | Rocker_Left_Y_Pin | Bat_Pin | Rocker_Right_X_Pin | Rocker_Right_Y_Pin ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,1,ADC_SampleTime_55Cycles5);//ADC������ͨ�����ã�ADC1��ͨ�������У�����ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_6,2,ADC_SampleTime_55Cycles5);//ͨ������ADC_Channel�ı�ͨ����ʵ�ֶ��IO�ڵ�ADC��ȡ
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,3,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,4,ADC_SampleTime_55Cycles5);//ͨ������ADC_Channel�ı�ͨ����ʵ�ֶ��IO�ڵ�ADC��ȡ
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,5,ADC_SampleTime_55Cycles5);
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		//����ģʽ������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;		//���ݶ��룬�Ҷ���
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;		//�ⲿ����Դѡ�񣬴˴�Ϊ��ʹ���ⲿ����,ʹ���������
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		//�����򵥴�ģʽ��ENABLEΪ����
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;		//ɨ��ģʽ���Զ�ɨ��ÿ��ͨ��	
	ADC_InitStructure.ADC_NbrOfChannel = CHANNEL_COUNTS;		//0����16��ͨ����Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//��ʼ��ADC
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//��ʼ��ַ������ֵ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//���ݿ��16λ����
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//��ַ��������ʼ��ת��ͬһ��λ�õ�����
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;//�洢����ʼ��ַ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//���ݿ�Ȱ���
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//��ַ����
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//�����ַ��ΪԴ�����䷽�������赽�洢��
	DMA_InitStructure.DMA_BufferSize = CHANNEL_COUNTS;//�������
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//����ģʽ
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//Ӳ������������ԴΪADC1
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//�е����ȼ�
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);//DMA��ʼ��
	
	DMA_Cmd(DMA1_Channel1,ENABLE);//ʹ��
	ADC_DMACmd(ADC1,ENABLE);//ADC1����DMA����ź�

	ADC_Cmd(ADC1, ENABLE);		//����ADC��Դ
	
	ADC_ResetCalibration(ADC1);		//��λУ׼
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);	//�ȴ�У׼���
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) == SET);
}

void AD_GetValue(void)
{
	DMA_Cmd(DMA1_Channel1,DISABLE);//ʧ��
	DMA_SetCurrDataCounter(DMA1_Channel1,CHANNEL_COUNTS);//�����������д����
	DMA_Cmd(DMA1_Channel1,ENABLE);//ʹ��
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//�������
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);//�ȴ��������
	DMA_ClearFlag(DMA1_FLAG_TC1);//�����־λ
	
}
// ��ȡADCͨ����ƽ��ֵ
void ADC1_Value_average(void)
{
	uint16_t i;
	for(i=0;i<10;i++)
	{
		AD_GetValue();
		for(int j=0;j<4;j++)
		{
			ALL_Sample_Buffer[j] += AD_Value[j];//10��ADCֵ�ۼ�
		}
	}
	for(int j=0;j<4;j++)
	{
		AV_Sample_Buffer[j] = ALL_Sample_Buffer[j]/10;//��ƽ��ֵ
		ALL_Sample_Buffer[j]=0;
	}

}