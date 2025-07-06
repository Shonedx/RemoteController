#ifndef NRF24L01_H
#define NRF24L01_H

#include "stm32f10x.h"

// 定义STM32F1平台
#define STM32F1

#ifdef STM32F1
#include "stm32f10x.h"
#define NRF_SPI_INSTANCE    SPI2
#define NRF_GPIO_CLK_CMD    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE)
#define NRF_SPI_CLK_CMD     RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE)
#define NRF_GPIO_AF_MODE    GPIO_Mode_AF_PP
#define NRF_GPIO_IN_MODE    GPIO_Mode_IPU
#define NRF_EXTI_PORT_SRC   GPIO_PortSourceGPIOB
#define NRF_EXTI_PIN_SRC    GPIO_PinSource11
#define NRF_EXTI_LINE       EXTI_Line11
#define NRF_EXTI_IRQN       EXTI15_10_IRQn
#endif

// NRF24L01 寄存器定义
#define NRF_READ_REG        0x00
#define NRF_WRITE_REG       0x20
#define RD_RX_PLOAD         0x61
#define WR_TX_PLOAD         0xA0
#define FLUSH_TX            0xE1
#define FLUSH_RX            0xE2
#define REUSE_TX_PL         0xE3
#define NOP                 0xFF

// 寄存器地址
#define CONFIG          0x00
#define EN_AA           0x01
#define EN_RXADDR       0x02
#define SETUP_AW        0x03
#define SETUP_RETR      0x04
#define RF_CH           0x05
#define RF_SETUP        0x06
#define STATUS          0x07
#define OBSERVE_TX      0x08
#define CD              0x09
#define RX_ADDR_P0      0x0A
#define RX_ADDR_P1      0x0B
#define RX_ADDR_P2      0x0C
#define RX_ADDR_P3      0x0D
#define RX_ADDR_P4      0x0E
#define RX_ADDR_P5      0x0F
#define TX_ADDR         0x10
#define RX_PW_P0        0x11
#define RX_PW_P1        0x12
#define RX_PW_P2        0x13
#define RX_PW_P3        0x14
#define RX_PW_P4        0x15
#define RX_PW_P5        0x16
#define FIFO_STATUS     0x17
#define DYNPD           0x1C
#define FEATURE         0x1D

// 状态寄存器位
#define RX_DR   6
#define TX_DS   5
#define MAX_RT  4

// 配置寄存器位
#define PRIM_RX         0
#define PWR_UP          1

// RF_SETUP 寄存器位
#define RF_DR_LOW       5
#define RF_DR_HIGH      3

// 数据包长度
#define NRF_PAYLOAD_LENGTH  11

// 通信地址
extern const uint8_t NRF_COMMON_ADDR[5];
extern uint8_t tx_buffer[NRF_PAYLOAD_LENGTH];

// 引脚定义
extern GPIO_TypeDef* NRF_CE_Port;
extern uint16_t      NRF_CE_Pin;
extern GPIO_TypeDef* NRF_CSN_Port;
extern uint16_t      NRF_CSN_Pin;
extern GPIO_TypeDef* NRF_IRQ_Port;
extern uint16_t      NRF_IRQ_Pin;
extern GPIO_TypeDef* NRF_MOSI_Port;
extern uint16_t      NRF_MOSI_Pin;
extern GPIO_TypeDef* NRF_MISO_Port;
extern uint16_t      NRF_MISO_Pin;
extern GPIO_TypeDef* NRF_SCK_Port;
extern uint16_t      NRF_SCK_Pin;

// 函数声明
void NRF24L01_Init_Common(void);
uint8_t NRF24L01_Check(void);
void NRF24L01_Set_RX_Mode(void);
uint8_t NRF24L01_RxPacket_IRQ(uint8_t *rxbuf);
void NRF24L01_Set_TX_Mode(void);
uint8_t NRF24L01_TxPacket_IRQ(uint8_t *txbuf);
void NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len);
void NRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len);
uint8_t NRF24L01_Read_Reg(uint8_t reg);
uint8_t NRF24L01_Write_Reg(uint8_t reg, uint8_t value);
uint8_t NRF24L01_SpiRW(uint8_t dat);
void Load_Data_to_Tx_Buffer(void);
void NRF24_Process(void);

// NRF24L01 引脚定义 (CE, CSN, IRQ)
// 具体引脚定义将在各自的 main.c 或项目配置中完成
#define NRF_CE_H()      GPIO_SetBits(NRF_CE_Port, NRF_CE_Pin)
#define NRF_CE_L()      GPIO_ResetBits(NRF_CE_Port, NRF_CE_Pin)
#define NRF_CSN_H()     GPIO_SetBits(NRF_CSN_Port, NRF_CSN_Pin)
#define NRF_CSN_L()     GPIO_ResetBits(NRF_CSN_Port, NRF_CSN_Pin)
#define NRF_IRQ_READ()  GPIO_ReadInputDataBit(NRF_IRQ_Port, NRF_IRQ_Pin)




#endif /* __NRF24L01_H */
