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
}CtrlState_t; //������״̬��

typedef enum TranslateState
{
	TRANS_DISABLE,
	TRANS_ENABLE,
}TranslateState_t; //������״̬��

typedef enum JumpState
{
	IDLE, 
	BEND,//���� step1
	LEAN,//��б step2
	EXE, //ִ�� step3
}JumpState_t; //������״̬��

typedef enum IdleState
{
	STOP,
	NORMAL,
}IdleState_t; //������״̬��

// ����״̬��
typedef enum {
    TX_STATE_IDLE,      // ����״̬
    TX_STATE_PREPARE,   // ׼������
    TX_STATE_SEND,      // ��������
    TX_STATE_WAIT       // �ȴ��������
} TX_State_t;


typedef struct TX_CTX {
	TX_State_t state;   // ��ǰ״̬
	uint8_t    tx_buf[NRF_PAYLOAD_LENGTH]; // ���ͻ�����
} TX_CTX_t; //����������

void RC_Ctrl(void);
uint8_t RC_TxPacket_Async(uint8_t *data);

#endif