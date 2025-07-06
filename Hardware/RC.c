#include "RC.h"
#include "OLED.h"
#include "key.h"


uint8_t height =StandHeight; //CrouchHeight<height<HigherHeight
uint8_t angle=15; //绝对值大小 0-30
TX_CTX_t tx_context;
CtrlState_t ctrl_state=CS_NONE;
TranslateState_t translate_state=TRANS_DISABLE;
IdleState_t idle_state=STOP;
JumpState_t jump_state=IDLE;
extern uint8_t tx_done;
// 启动异步发送（立即返回，不等待）
uint8_t RC_TxPacket_Async(uint8_t *data) {
    if (tx_context.state != TX_STATE_IDLE) {
        return 0; // 当前忙，拒绝新请求
    }
    
    // 拷贝数据到发送缓冲区
    memcpy(tx_context.tx_buf, data, NRF_PAYLOAD_LENGTH);
   
    tx_context.state = TX_STATE_PREPARE; // 进入准备状态
    tx_done = 0; // 重置发送结果
	
    return 1; // 启动成功
}


