#ifndef _RC_H
#define _RC_H
#include "nrf24l01.h"
#include "stdint.h"
#include "string.h"
#define StandHeight 20 
#define CrouchHeight 14
#define HigherHeight 35
typedef enum CtrlState
{
	CS_NONE,
	CS_INIT,
	CS_MAIN,
	CS_PRE_JUMP,
	CS_EXE_JUMP,
	CS_HEIGHT,
	CS_QUIT,
}CtrlState_t; //控制类状态机

typedef enum TranslateState
{
	TRANS_DISABLE,
	TRANS_ENABLE,
}TranslateState_t; //控制类状态机

typedef enum JumpState
{
	IDLE, 
	BEND,//俯身 step1
	LEAN,//倾斜 step2
	EXE, //执行 step3
}JumpState_t; //控制类状态机

typedef enum IdleState
{
	STOP,
	NORMAL,
}IdleState_t; //控制类状态机

// 发送状态机
typedef enum {
    TX_STATE_IDLE,      // 空闲状态
    TX_STATE_PREPARE,   // 准备数据
    TX_STATE_SEND,      // 启动发送
    TX_STATE_WAIT       // 等待发送完成
} TX_State_t;


typedef struct TX_CTX {
	TX_State_t state;   // 当前状态
	uint8_t    tx_buf[NRF_PAYLOAD_LENGTH]; // 发送缓冲区
} TX_CTX_t; //发送上下文

void RC_Ctrl(void);
uint8_t RC_TxPacket_Async(uint8_t *data);

#endif