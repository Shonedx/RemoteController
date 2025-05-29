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

// 假设的按键状态定义
#define PRESSED  0 // 根据Key_Pressed 函数返回值确定
#define RELEASED 1 // 根据Key_Pressed 函数返回值确定

// 假设有10个按键
#define NUM_KEYS 10

//双击间隔时间决定单击按键的响应速度
// 假设的去抖时间、长按时间、双击间隔时间 (单位取决于 GetNowTime())
#define DEBOUNCE_TIME    10  // ms 
#define LONG_PRESS       1500 // ms 
#define DOUBLE_INTERVAL  100  // ms 


// 假设的按键状态机状态
typedef enum {
    KEY_IDLE = 0,          // 按键空闲状态
    KEY_PRESSED_DEBOUNCE,  // 检测到按下，等待去抖
    KEY_PRESSED,           // 按键稳定按下状态
    KEY_WAIT_INTERVAL      // 第一次短按释放后，等待双击时间窗口
} KeyState_t;

// 假设的事件类型
typedef enum {
    Event_None = 0,
    Event_SingleClick,
    Event_DoubleClick,
    Event_LongPress,
    // Event_LongPressRepeat, // 如果需要长按连发可以添加
} KeyEvent_t;

// 假设的按键状态信息结构体
typedef struct {
    KeyState_t key_state;         // 当前按键状态
    uint32_t pressed_start_time;  // 按下时间戳
    uint32_t release_time;        // 释放时间戳 (用于计算双击间隔)
    uint8_t click_count;          // 记录点击次数 (用于双击检测)
    KeyEvent_t event_handle;      // 待处理的事件
    uint32_t long_press_handled;  // 标志，确保长按事件只触发一次直到释放 (如果长按定义为按住N时间后触发一次)
    // 如果需要长按连发，可以加一个 long_press_repeat_time
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

