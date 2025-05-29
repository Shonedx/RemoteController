#ifndef  _KEY_H
#define  _KEY_H

#include "stm32f10x.h"
typedef enum 
{
	Keys_WKUP, //0
	Keys_KEY_1,
	Keys_SW_shang,
	Keys_SW_xia,
	Keys_SW_zuo,
	Keys_SW_you,
	Keys_SW_UP,
	Keys_SW_DOWN,
	Keys_SW_LEFT,
	Keys_SW_RIGHT //9
}Keys;

#define  WKUP  GPIO_Pin_0  /**PA**/
#define  KEY_1 GPIO_Pin_14 /**PC**/

#define  SW_shang   GPIO_Pin_11 /******/
#define  SW_xia     GPIO_Pin_12 /**PA**/
#define  SW_zuo     GPIO_Pin_15 /******/
#define  SW_you     GPIO_Pin_3 /**PB**/

#define  SW_UP      GPIO_Pin_6 /******/
#define  SW_DOWN    GPIO_Pin_7 /**PB**/
#define  SW_LEFT    GPIO_Pin_8 /**PB**/
#define  SW_RIGHT   GPIO_Pin_9 /******/

// ����İ���״̬����
#define PRESSED  0 // ����Key_Pressed ��������ֵȷ��
#define RELEASED 1 // ����Key_Pressed ��������ֵȷ��

// ������10������
#define NUM_KEYS 10

//˫�����ʱ�����������������Ӧ�ٶ�
// �����ȥ��ʱ�䡢����ʱ�䡢˫�����ʱ�� (��λȡ���� GetNowTime())
#define DEBOUNCE_TIME    10  // ms 
#define LONG_PRESS       1500 // ms 
#define DOUBLE_INTERVAL  100  // ms 


// ����İ���״̬��״̬
typedef enum {
    KEY_IDLE = 0,          // ��������״̬
    KEY_PRESSED_DEBOUNCE,  // ��⵽���£��ȴ�ȥ��
    KEY_PRESSED,           // �����ȶ�����״̬
    KEY_WAIT_INTERVAL      // ��һ�ζ̰��ͷź󣬵ȴ�˫��ʱ�䴰��
} KeyState_t;

// ������¼�����
typedef enum {
    Event_None = 0,
    Event_SingleClick,
    Event_DoubleClick,
    Event_LongPress,
    // Event_LongPressRepeat, // �����Ҫ���������������
} KeyEvent_t;

// ����İ���״̬��Ϣ�ṹ��
typedef struct {
    KeyState_t key_state;         // ��ǰ����״̬
    uint32_t pressed_start_time;  // ����ʱ���
    uint32_t release_time;        // �ͷ�ʱ��� (���ڼ���˫�����)
    uint8_t click_count;          // ��¼������� (����˫�����)
    KeyEvent_t event_handle;      // ��������¼�
    uint32_t long_press_handled;  // ��־��ȷ�������¼�ֻ����һ��ֱ���ͷ� (�����������Ϊ��סNʱ��󴥷�һ��)
    // �����Ҫ�������������Լ�һ�� long_press_repeat_time
} KeyStateInfo_t;

//extern
extern KeyStateInfo_t keystatebuffer[NUM_KEYS];
extern uint8_t key_event_buffer[10];

//Fuctions
int Key_Pressed(int key);

void Key_Init(void);

void Key_InitState(void);

uint32_t GetNowTime(void) ;

//void Key_Event_Scan(KeyStateInfo_t *key_state_buffer);
#endif

