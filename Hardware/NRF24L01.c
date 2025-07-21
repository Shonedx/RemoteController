#include "nrf24l01.h"
#include "delay.h"
#include "RC.h"
#include "AD.h"
#include "KEY.h"
#include "HW504.h"
#include "OLED.h"
// ͨ�ŵ�ַ
const uint8_t NRF_COMMON_ADDR[5] = {'e','t','h','a','n'};

extern volatile uint8_t rx_flag; // ���ձ�־
extern volatile uint8_t tx_done; //�����־
extern TX_CTX_t tx_context;
extern CtrlState_t ctrl_state;
// ��ʼ��ͨ�ò���
void NRF24L01_Init_Common(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 1. ʹ��ʱ��
    NRF_GPIO_CLK_CMD;
    NRF_SPI_CLK_CMD;

    // 2. ����SPI����
    GPIO_InitStructure.GPIO_Pin = NRF_SCK_Pin | NRF_MOSI_Pin;
    GPIO_InitStructure.GPIO_Mode = NRF_GPIO_AF_MODE;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NRF_SCK_Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = NRF_MISO_Pin;
    GPIO_InitStructure.GPIO_Mode = NRF_GPIO_IN_MODE;
    GPIO_Init(NRF_MISO_Port, &GPIO_InitStructure);

    // 3. ����CE, CSN, IRQ����
    GPIO_InitStructure.GPIO_Pin = NRF_CE_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NRF_CE_Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = NRF_CSN_Pin;
    GPIO_Init(NRF_CSN_Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = NRF_IRQ_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(NRF_IRQ_Port, &GPIO_InitStructure);

    // 4. ����SPI
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // 9MHz (72MHz/8)
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(NRF_SPI_INSTANCE, &SPI_InitStructure);
    SPI_Cmd(NRF_SPI_INSTANCE, ENABLE);

    // 5. �����ⲿ�ж�
    GPIO_EXTILineConfig(NRF_EXTI_PORT_SRC, NRF_EXTI_PIN_SRC);
    EXTI_InitStructure.EXTI_Line = NRF_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 6. ����NVIC
    NVIC_InitStructure.NVIC_IRQChannel = NRF_EXTI_IRQN;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 7. ��ʼ��״̬
    NRF_CE_L();
    NRF_CSN_H();
    Delay_ms(100);

    // 8. ���ģ��
    if (!NRF24L01_Check()) {
        while (1); // ��ѭ����ʾ����
    }

    // ���FIFO
    NRF24L01_Write_Reg(FLUSH_TX, 0xFF);
    NRF24L01_Write_Reg(FLUSH_RX, 0xFF);
}

// SPI��дһ���ֽ�
uint8_t NRF24L01_SpiRW(uint8_t dat) {
    uint8_t retry = 0;
    // �ȴ����ͻ�������
    while (SPI_I2S_GetFlagStatus(NRF_SPI_INSTANCE, SPI_I2S_FLAG_TXE) == RESET) {
        if (++retry > 200) break;
    }
    SPI_I2S_SendData(NRF_SPI_INSTANCE, dat);
    retry = 0;
    // �ȴ��������
    while (SPI_I2S_GetFlagStatus(NRF_SPI_INSTANCE, SPI_I2S_FLAG_RXNE) == RESET) {
        if (++retry > 200) break;
    }
    return SPI_I2S_ReceiveData(NRF_SPI_INSTANCE);
}

// д�Ĵ���
uint8_t NRF24L01_Write_Reg(uint8_t reg, uint8_t value) {
    uint8_t status;
    NRF_CSN_L();
    status = NRF24L01_SpiRW(NRF_WRITE_REG | reg);
    NRF24L01_SpiRW(value);
    NRF_CSN_H();
    return status;
}

// ���Ĵ���
uint8_t NRF24L01_Read_Reg(uint8_t reg) {
    uint8_t reg_val;
    NRF_CSN_L();
    NRF24L01_SpiRW(NRF_READ_REG | reg);
    reg_val = NRF24L01_SpiRW(NOP);
    NRF_CSN_H();
    return reg_val;
}

// д������
void NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len) {
    uint8_t i;
    NRF_CSN_L();
    NRF24L01_SpiRW(reg);
    for (i = 0; i < len; i++) {
        NRF24L01_SpiRW(pBuf[i]);
    }
    NRF_CSN_H();
}

// ��������
void NRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len) {
    uint8_t i;
    NRF_CSN_L();
    NRF24L01_SpiRW(reg);
    for (i = 0; i < len; i++) {
        pBuf[i] = NRF24L01_SpiRW(NOP);
    }
    NRF_CSN_H();
}

// ���ģ���Ƿ����
uint8_t NRF24L01_Check(void) {
    uint8_t buf[5] = {0};
    const uint8_t test_addr[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
    
    // д����Ե�ַ
    NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, (uint8_t*)test_addr, 5);
    // ��ȡ���Ƚ�
    NRF24L01_Read_Buf(NRF_READ_REG + TX_ADDR, buf, 5);
    
    for (uint8_t i = 0; i < 5; i++) {
        if (buf[i] != test_addr[i]) {
            return 0;
        }
    }
    return 1;
}

void NRF24L01_Set_RX_Mode(void) {
	NRF_CE_L();
	
    // ���ý��յ�ַ��ͨ��0���ݳ���
    NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (uint8_t*)NRF_COMMON_ADDR, 5);
    NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P0, NRF_PAYLOAD_LENGTH);
    // ����ͨ��0�Զ�Ӧ��
    NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x01);
    // ����ͨ��0����ʹ��
    NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);
    // RF���ã�2Mbps, 0dBm
    NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0F);
    // �Զ��ط���500us, 15��
    NRF24L01_Write_Reg(NRF_WRITE_REG + SETUP_RETR, 0x1F);
    // ѡ��Ƶ��40 (2.440GHz)
    NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);
    // ����Ϊ����ģʽ���ϵ�
    NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0F); // PWR_UP=1, PRIM_RX=1
    // ���״̬�Ĵ���
    NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, 0x70); // ��������жϱ�־
    // ���FIFO
    NRF24L01_Write_Reg(FLUSH_RX, 0xFF);
//	NRF24L01_Write_Reg(CONFIG, 0x0B); // 0b00001011
    // �������ģʽ
    NRF_CE_H();

    Delay_us(150);
}

// ����Ϊ����ģʽ
void NRF24L01_Set_TX_Mode(void) {
    NRF_CE_L();
    // ���÷��͵�ַ
    NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, (uint8_t*)NRF_COMMON_ADDR, 5);
    // ���ý��յ�ַ0�����ڽ���ACK��
    NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (uint8_t*)NRF_COMMON_ADDR, 5);
    // ����ͨ��0���ݳ��ȣ�����ACK���ĳ��ȣ�
    NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P0, NRF_PAYLOAD_LENGTH);
    // ͨ��0�Զ�Ӧ��
    NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x01);
    // ͨ��0����ʹ��
    NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);
    // RF���ã�2Mbps, 0dBm
    NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0F);
    // �Զ��ط���500us, 15��
    NRF24L01_Write_Reg(NRF_WRITE_REG + SETUP_RETR, 0x1F);
    // ѡ��Ƶ��40
    NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);
    // ����Ϊ����ģʽ���ϵ�
    NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0E); // PWR_UP=1, PRIM_RX=0
    // ���״̬�Ĵ���
    NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, 0x70); // ��������жϱ�־
    // ���FIFO
    NRF24L01_Write_Reg(FLUSH_TX, 0xFF);
}

uint8_t NRF24L01_RxPacket_Polling(uint8_t *rxbuf) { //��ѯ��������
    uint8_t status = NRF24L01_Read_Reg(STATUS);
    
    if(status & (1 << RX_DR)) { // ��������ɱ�־
        NRF24L01_Read_Buf(RD_RX_PLOAD, rxbuf, NRF_PAYLOAD_LENGTH);
        NRF24L01_Write_Reg(STATUS, status); // ����жϱ�־
        NRF24L01_Write_Reg(FLUSH_RX, 0xFF);  // ���RX FIFO
        return 1;
    }
    return 0;
}

// �������ݰ����жϷ�ʽ��
uint8_t NRF24L01_TxPacket_IRQ(uint8_t *txbuf) {
    tx_done = 0;
    NRF24L01_Write_Buf(WR_TX_PLOAD, txbuf, NRF_PAYLOAD_LENGTH);
    NRF_CE_H();
    Delay_us(20);
    NRF_CE_L();
    return tx_done;
}

// ״̬������
void NRF24_Process(void) {
    switch (tx_context.state) {
        case TX_STATE_PREPARE:
            // ׼�����ݣ�������д��TX FIFO
            NRF24L01_Write_Buf(WR_TX_PLOAD, tx_context.tx_buf, NRF_PAYLOAD_LENGTH);
            tx_context.state = TX_STATE_SEND;
            break;
            
        case TX_STATE_SEND:
            // �������ͣ�CE�����壩
            NRF_CE_H();
            Delay_us(20); // ��������10us��������20us
            NRF_CE_L();
            tx_context.state = TX_STATE_WAIT; // �ȴ��ж�
            break;
            
        case TX_STATE_WAIT:

            break;
            
        case TX_STATE_IDLE:

			break;
        default:
            break;
    }
}
extern CtrlState_t ctrl_state;
extern TranslateState_t translate_state;
extern IdleState_t idle_state;
extern JumpState_t jump_state;
extern uint8_t height;
extern uint8_t angle;
void Load_Data_to_Tx_Buffer(void)
{
	tx_buffer[0]=0x00|(uint8_t)ctrl_state|((uint8_t)translate_state<<4)|((uint8_t)idle_state<<5)|((uint8_t)jump_state<<6);
	tx_buffer[1]=(uint8_t)height;
	tx_buffer[2]=AD_Value[Rocker_Right_X]&0xFF; //Rocker_Right_X_Low_Half
	tx_buffer[3]=AD_Value[Rocker_Right_X]>>8; 	//Rocker_Right_X_High_Half
	tx_buffer[4]=AD_Value[Rocker_Right_Y]&0xFF; //Rocker_Right_Y_Low_Half
	tx_buffer[5]=AD_Value[Rocker_Right_Y]>>8; 	//Rocker_Right_Y_High_Half
	tx_buffer[6]=AD_Value[Rocker_Left_Y]&0xFF; //Rocker_Left_X_Low_Half
	tx_buffer[7]=AD_Value[Rocker_Left_Y]>>8; 	//Rocker_Left_X_High_Half
	tx_buffer[8]=AD_Value[Rocker_Left_X]&0xFF; //Rocker_Left_Y_Low_Half
	tx_buffer[9]=AD_Value[Rocker_Left_X]>>8; 	//Rocker_Left_Y_High_Half
	tx_buffer[10]=(uint8_t)angle;
//	tx_buffer[18]=Rocker_Right_SW_Read();
//	tx_buffer[19]=Rocker_Left_SW_Read();
	
}
// �жϷ�����
void EXTI15_10_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
        uint8_t status = NRF24L01_Read_Reg(STATUS); // ��ȷ��ȡ״̬�Ĵ���
        
        // ����жϱ�־��д1�����
        NRF24L01_Write_Reg(STATUS, status); // ����ǰ״̬д�ؼ��������־
        
        if (status & (1 << TX_DS)) { // ���ͳɹ�
            // ���FIFO
            NRF24L01_Write_Reg(FLUSH_TX, 0xFF);
            tx_done = 1;
            tx_context.state = TX_STATE_IDLE;
        }
        else if (status & (1 << MAX_RT)) { // �ﵽ����ط�����
            NRF24L01_Write_Reg(FLUSH_TX, 0xFF);
            tx_done = 2;
            tx_context.state = TX_STATE_IDLE;
        }
		
		// ����ⲿ�жϱ�־
        EXTI_ClearITPendingBit(EXTI_Line11);
    }
}