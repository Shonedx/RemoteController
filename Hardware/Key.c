#include "key.h"
#include "delay.h"
#include "led_driver.h"
#include "buzzer.h"

/*依次存放各按键状态的结构体数组
keystatebuffer[0]---WKUP 按下为1，其余为0
keystatebuffer[1]---KEY_1
keystatebuffer[2]---SW_shang
keystatebuffer[3]---SW_xia
keystatebuffer[4]---SW_zuo
keystatebuffer[5]---SW_you
keystatebuffer[6]---SW_UP
keystatebuffer[7]---SW_DOWN
keystatebuffer[8]---SW_LEFT
keystatebuffer[9]---SW_RIGHT
*/
KeyStateInfo_t keystatebuffer[NUM_KEYS]={0};

uint8_t key_event_buffer[10]={0};
void Key_Init(void) //按键初始化函数
{
	GPIO_InitTypeDef  GPIO_Initstructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);		// JTAG失能 //使得PA15可以作为GPIO口使用
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);
	//按键：WKUP--PA0
	GPIO_Initstructure.GPIO_Pin = WKUP;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPD; // 下拉输入
	GPIO_Init(GPIOA,&GPIO_Initstructure);

	GPIO_Initstructure.GPIO_Pin =   SW_shang | SW_xia | SW_zuo;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
	GPIO_Init(GPIOA,&GPIO_Initstructure);

	GPIO_Initstructure.GPIO_Pin = SW_UP | SW_DOWN | SW_LEFT | SW_RIGHT | SW_you;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
	GPIO_Init(GPIOB,&GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Pin = KEY_1 ;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
	GPIO_Init(GPIOC,&GPIO_Initstructure);
}

int Key_Pressed(int key) //按键按下返回0
{
	int output=-1;
	switch(key)
	{
		case Keys_WKUP:
			output=!GPIO_ReadInputDataBit(GPIOA,WKUP);
			break;
		case Keys_KEY_1:
			output=GPIO_ReadInputDataBit(GPIOC,KEY_1);
			break;
		case Keys_SW_DOWN:
			output=GPIO_ReadInputDataBit(GPIOB,SW_DOWN);
			break;
		case Keys_SW_UP:
			output=GPIO_ReadInputDataBit(GPIOB,SW_UP);
			break;
		case Keys_SW_shang:
			output=GPIO_ReadInputDataBit(GPIOA,SW_shang);
			break;
		case Keys_SW_xia:
			output=GPIO_ReadInputDataBit(GPIOA,SW_xia);
			break;
		case Keys_SW_zuo:
			output=GPIO_ReadInputDataBit(GPIOA,SW_zuo);
			break;
		case Keys_SW_you:
			output=GPIO_ReadInputDataBit(GPIOA,SW_you);
			break;
		case Keys_SW_LEFT:
			output=GPIO_ReadInputDataBit(GPIOB,SW_LEFT);
			break;
		case Keys_SW_RIGHT:
			output=GPIO_ReadInputDataBit(GPIOB,SW_RIGHT);
			break;
		default:
			break;
	}
	return output;
}
//void Key_Event_Scan(KeyStateInfo_t *key_state_buffer)
//{
//	for(int i=0;i<10;i++)
//	{
//		key_event_buffer[i]= key_state_buffer[i].event_handle;	
//	}
//}
// 初始化按键状态的函数 (在系统初始化时调用)
void Key_InitState(void)
{
    for(int i = 0; i < NUM_KEYS; i++)
    {
        keystatebuffer[i].key_state = KEY_IDLE;
        keystatebuffer[i].pressed_start_time = 0;
        keystatebuffer[i].release_time = 0;
        keystatebuffer[i].click_count = 0;
        keystatebuffer[i].event_handle = Event_None;
        keystatebuffer[i].long_press_handled = 0;
    }
}

const uint32_t dt=1; //更新时间用的基数
static uint32_t now_time=0; //当前时间
 uint32_t GetNowTime(void) //返回当前时间
{
	return now_time;
}
static void UpdateTime(void) //更新时间基准，这是识别长按等状态的基础
{
	now_time+=dt;
	if(now_time>1000000)
		now_time=0;
}
// ==========================================================
// **** 定时器 TIM2 中断服务函数****
// ==========================================================
void TIM2_IRQHandler(void) //执行按键扫描功能，可以识别长按，双击，单击
{
	
    // 检查是否是TIM2更新中断，并清除标志位，防止重复进入
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
       TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // 立即清除中断标志位

        UpdateTime(); // 更新时间基准，假设 GetNowTime() 依赖于此

        for(int i = 0; i < NUM_KEYS; i++)
        {
			
			int curr_state = Key_Pressed(i); // 读取当前按键状态
			switch(keystatebuffer[i].key_state)
            {
                case KEY_IDLE:
                    if(curr_state == PRESSED) // 检测到按下
                    {
                        keystatebuffer[i].pressed_start_time = GetNowTime();
                        keystatebuffer[i].key_state = KEY_PRESSED_DEBOUNCE; // 进入按下抖动状态
                        keystatebuffer[i].long_press_handled = 0; // 重置长按处理标志
                    }
                    // else: 按键释放且在IDLE状态，不进行任何操作，这是正确的空闲状态行为。
                    break;

                case KEY_PRESSED_DEBOUNCE: // 按下去了抖状态
                    if (curr_state == RELEASED)
                    { // 在去抖时间内释放，认为是抖动，忽略
                         keystatebuffer[i].key_state = KEY_IDLE; // 回到空闲
                    }
                    else
                    { // 仍然按下，检查是否已满足去抖时间
                         if (GetNowTime() - keystatebuffer[i].pressed_start_time >= DEBOUNCE_TIME)
                         {
                             keystatebuffer[i].key_state = KEY_PRESSED; // 稳定按下，进入 KEY_PRESSED 状态
                         }
                    }
                    break;


                case KEY_PRESSED: // 按键稳定按下状态 (已去抖)
                    if(curr_state == RELEASED) // 检测到释放事件
                    {
                        uint32_t press_duration = GetNowTime() - keystatebuffer[i].pressed_start_time; // 计算按下时长
                        keystatebuffer[i].release_time = GetNowTime(); // 记录释放时间 (用于计算双击间隔)

                        if(press_duration >= LONG_PRESS) // 长按阈值判断
                        {
                            // 如果定义为按住超过N时间后释放触发一次长按事件
                            if (keystatebuffer[i].long_press_handled == 0) // 检查长按事件是否已在按下过程中处理
                            {
                                // 这种情况下，长按事件可能已在下面 'else' 分支中处理并设置标志
                                // 如果长按定义为释放时触发，则在这里设置标志：
                                keystatebuffer[i].event_handle = Event_LongPress;
							
                            }
                             // 长按或短按释放后，都需要回到 IDLE 或等待双击状态
                        }
                        else // 短按释放 (持续时间小于 LONG_PRESS，且已通过去抖)
                        {
                            keystatebuffer[i].click_count++; // 有效短按释放计数加一

                            if (keystatebuffer[i].click_count == 2) // 如果计数达到2，检测到双击
                            {
                                keystatebuffer[i].event_handle = Event_DoubleClick; // *** 在这里标记双击事件发生 ***
                                keystatebuffer[i].click_count = 0; // 双击处理完毕，清零计数
                                keystatebuffer[i].key_state = KEY_IDLE; // 回到空闲状态
                            }
                            else // 如果计数是1 (第一次有效短按释放)
                            {
                                keystatebuffer[i].key_state = KEY_WAIT_INTERVAL; // 进入等待双击间隔状态
                                // release_time 已经在上面记录
                            }
                        }

                        // 无论是长按释放还是短按释放导致的状态变化 (除了进入 KEY_WAIT_INTERVAL)
                        if (keystatebuffer[i].key_state != KEY_WAIT_INTERVAL) {
                            keystatebuffer[i].click_count = 0; // 清除点击计数 (如果是长按或去抖失败回到IDLE) 
                            keystatebuffer[i].key_state = KEY_IDLE; // 回到空闲状态
                        }
                        keystatebuffer[i].long_press_handled = 0; // 释放后重置长按标志
                    }
                    else // 仍然保持按下状态 (curr_state == PRESSED)
                    {
                         // 在按下过程中持续检测长按时间
                         if (GetNowTime() - keystatebuffer[i].pressed_start_time >= LONG_PRESS && keystatebuffer[i].long_press_handled == 0)
                         {
                             keystatebuffer[i].event_handle = Event_LongPress; // *** 在这里标记长按事件发生 (按住达到时间触发一次) ***
                             keystatebuffer[i].long_press_handled = 1; // 设置标志，避免长按不放重复触发此事件
                             // 如果需要长按连发，逻辑会更复杂，需要另外的时间判断和重置机制
                         }
                    }
                    break;

                case KEY_WAIT_INTERVAL: // 等待双击间隔状态 (在第一次短按释放后)
                    if(curr_state == PRESSED) // *** 检测到第二次按下 ***
                    {
                        // 此时检测到第二次按下了，开始处理第二次点击的去抖和按下过程
                        keystatebuffer[i].pressed_start_time = GetNowTime(); // 记录第二次按下的时间
                        keystatebuffer[i].key_state = KEY_PRESSED_DEBOUNCE; // 进入按下抖动状态处理第二次按下
                        keystatebuffer[i].long_press_handled = 0; // 重置长按标志 (本次按压不应该算作之前长按的延续) 确保按下时不会被识别为长按
                    }
                    else if(GetNowTime() - keystatebuffer[i].release_time > DOUBLE_INTERVAL) // 双击间隔超时
                    {
                        if(keystatebuffer[i].click_count == 1) // 如果在间隔内没有检测到第二次按下，且只有一次点击计数
                        {
                            keystatebuffer[i].event_handle = Event_SingleClick; // *** 标记为单击事件 ***
                        }
                        // 无论点击次数是1还是其他值（理论上超时时只有1，除非有bug），间隔结束后都回到空闲
                        keystatebuffer[i].click_count = 0; // 清除点击计数
                        keystatebuffer[i].key_state = KEY_IDLE; // 回到空闲状态
                    }
                    // else: 仍然是释放状态，且未超时，继续在 KEY_WAIT_INTERVAL 等待
                    break;
            }
			key_event_buffer[i]= keystatebuffer[i].event_handle;	//把按键状态传递给按键事件储存数组
        }
		
        // TIM_ClearITPendingBit 已经在中断服务程序入口处进行了清除，这通常是更安全的做法，
        // 确保在执行中断逻辑时标志位已经被清除。如果库函数要求在最后清除，请按库要求调整位置。
		
    }
	 
}


