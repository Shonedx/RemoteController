#include "RC.h"
#include "OLED.h"
#include "key.h"


uint8_t height =StandHeight; //CrouchHeight<height<HigherHeight
uint8_t angle=15; //����ֵ��С 0-30
TX_CTX_t tx_context;
CtrlState_t ctrl_state=CS_NONE;
TranslateState_t translate_state=TRANS_DISABLE;
IdleState_t idle_state=STOP;
JumpState_t jump_state=IDLE;
extern uint8_t tx_done;
// �����첽���ͣ��������أ����ȴ���
uint8_t RC_TxPacket_Async(uint8_t *data) {
    if (tx_context.state != TX_STATE_IDLE) {
        return 0; // ��ǰæ���ܾ�������
    }
    
    // �������ݵ����ͻ�����
    memcpy(tx_context.tx_buf, data, NRF_PAYLOAD_LENGTH);
   
    tx_context.state = TX_STATE_PREPARE; // ����׼��״̬
    tx_done = 0; // ���÷��ͽ��
	
    return 1; // �����ɹ�
}


