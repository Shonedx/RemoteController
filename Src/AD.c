#include "stm32f10x.h"                  // Device header
#include "AD.h"
#define CHANNEL_COUNTS 5 //通道数量，也可以用来表示dma传输次数
uint16_t AD_Value[5];//存放Right_X,Right_Y,Left_X,Left_Y,Bat_Value
uint32_t ALL_Sample_Buffer[4] = {0} ;	// 采样总计
// ALL_Sample_Buffer[0] ---- ALL_R_X_Sample 右摇杆x采样总计
// ALL_Sample_Buffer[1] ---- ALL_R_Y_Sample 右摇杆y采样总计
// ALL_Sample_Buffer[2] ---- ALL_L_X_Sample 左摇杆x采样总计
// ALL_Sample_Buffer[3] ---- ALL_L_Y_Sample 左摇杆y采样总计
uint16_t AV_Sample_Buffer[4] = {2053,2019,2053,2019} ;	//平均采样
// AV_Sample_Buffer[0] ---- AV_R_X_Sample ADC_CH3平均值
// AV_Sample_Buffer[1] ---- AV_R_Y_Sample ADC_CH6平均值
// AV_Sample_Buffer[2] ---- AV_L_X_Sample ADC_CH1平均值
// AV_Sample_Buffer[3] ---- AV_L_Y_Sample ADC_CH2平均值

void AD_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);		//开启ADC1内部时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//开启GPIOA内部时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//开启DMA1时钟

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	//12Mz, 配置ADC分频（*2、*4、6、8）
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入，ADC专属
	GPIO_InitStructure.GPIO_Pin = Rocker_Left_X_Pin | Rocker_Left_Y_Pin | Bat_Pin | Rocker_Right_X_Pin | Rocker_Right_Y_Pin ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,1,ADC_SampleTime_55Cycles5);//ADC规则组通道设置，ADC1，通道，序列，采样时间
	ADC_RegularChannelConfig(ADC1,ADC_Channel_6,2,ADC_SampleTime_55Cycles5);//通过参数ADC_Channel改变通道，实现多个IO口的ADC获取
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,3,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,4,ADC_SampleTime_55Cycles5);//通过参数ADC_Channel改变通道，实现多个IO口的ADC获取
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,5,ADC_SampleTime_55Cycles5);
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		//工作模式，独立
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;		//数据对齐，右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;		//外部触发源选择，此处为不使用外部触发,使用软件触发
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		//连续或单次模式，ENABLE为连续
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;		//扫描模式，自动扫描每个通道	
	ADC_InitStructure.ADC_NbrOfChannel = CHANNEL_COUNTS;		//0——16，通道数目
	ADC_Init(ADC1, &ADC_InitStructure);	//初始化ADC
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//起始地址参数传值
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//数据宽度16位半字
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//地址不自增，始终转运同一个位置的数据
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;//存储器起始地址
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//数据宽度半字
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//地址自增
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//外设地址作为源，传输方向由外设到存储器
	DMA_InitStructure.DMA_BufferSize = CHANNEL_COUNTS;//传输次数
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//正常模式
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//硬件触发，触发源为ADC1
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);//DMA初始化
	
	DMA_Cmd(DMA1_Channel1,ENABLE);//使能
	ADC_DMACmd(ADC1,ENABLE);//ADC1开启DMA输出信号

	ADC_Cmd(ADC1, ENABLE);		//开启ADC电源
	
	ADC_ResetCalibration(ADC1);		//复位校准
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);	//等待校准完成
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) == SET);
}

void AD_GetValue(void)
{
	DMA_Cmd(DMA1_Channel1,DISABLE);//失能
	DMA_SetCurrDataCounter(DMA1_Channel1,CHANNEL_COUNTS);//给传输计数器写数据
	DMA_Cmd(DMA1_Channel1,ENABLE);//使能
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//软件触发
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);//等待传输完成
	DMA_ClearFlag(DMA1_FLAG_TC1);//清除标志位
	
}
// 获取ADC通道的平均值
void ADC1_Value_average(void)
{
	uint16_t i;
	for(i=0;i<10;i++)
	{
		AD_GetValue();
		for(int j=0;j<4;j++)
		{
			ALL_Sample_Buffer[j] += AD_Value[j];//10次ADC值累加
		}
	}
	for(int j=0;j<4;j++)
	{
		AV_Sample_Buffer[j] = ALL_Sample_Buffer[j]/10;//求平均值
		ALL_Sample_Buffer[j]=0;
	}

}