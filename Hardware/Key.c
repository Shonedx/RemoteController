#include "key.h"
#include "delay.h"
#include "led_driver.h"
#include "buzzer.h"

/*���δ�Ÿ�����״̬�Ľṹ������
keystatebuffer[0]---WKUP ����Ϊ1������Ϊ0
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
void Key_Init(void) //������ʼ������
{
	GPIO_InitTypeDef  GPIO_Initstructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);		// JTAGʧ�� //ʹ��PA15������ΪGPIO��ʹ��
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);
	//������WKUP--PA0
	GPIO_Initstructure.GPIO_Pin = WKUP;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPD; // ��������
	GPIO_Init(GPIOA,&GPIO_Initstructure);

	GPIO_Initstructure.GPIO_Pin =   SW_shang | SW_xia | SW_zuo;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU; // ��������
	GPIO_Init(GPIOA,&GPIO_Initstructure);

	GPIO_Initstructure.GPIO_Pin = SW_UP | SW_DOWN | SW_LEFT | SW_RIGHT | SW_you;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU; // ��������
	GPIO_Init(GPIOB,&GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Pin = KEY_1 ;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU; // ��������
	GPIO_Init(GPIOC,&GPIO_Initstructure);
}

int Key_Pressed(int key) //�������·���0
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
// ��ʼ������״̬�ĺ��� (��ϵͳ��ʼ��ʱ����)
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

const uint32_t dt=1; //����ʱ���õĻ���
static uint32_t now_time=0; //��ǰʱ��
 uint32_t GetNowTime(void) //���ص�ǰʱ��
{
	return now_time;
}
static void UpdateTime(void) //����ʱ���׼������ʶ�𳤰���״̬�Ļ���
{
	now_time+=dt;
	if(now_time>1000000)
		now_time=0;
}
// ==========================================================
// **** ��ʱ�� TIM2 �жϷ�����****
// ==========================================================
void TIM2_IRQHandler(void) //ִ�а���ɨ�蹦�ܣ�����ʶ�𳤰���˫��������
{
	
    // ����Ƿ���TIM2�����жϣ��������־λ����ֹ�ظ�����
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
       TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // ��������жϱ�־λ

        UpdateTime(); // ����ʱ���׼������ GetNowTime() �����ڴ�

        for(int i = 0; i < NUM_KEYS; i++)
        {
			
			int curr_state = Key_Pressed(i); // ��ȡ��ǰ����״̬
			switch(keystatebuffer[i].key_state)
            {
                case KEY_IDLE:
                    if(curr_state == PRESSED) // ��⵽����
                    {
                        keystatebuffer[i].pressed_start_time = GetNowTime();
                        keystatebuffer[i].key_state = KEY_PRESSED_DEBOUNCE; // ���밴�¶���״̬
                        keystatebuffer[i].long_press_handled = 0; // ���ó��������־
                    }
                    // else: �����ͷ�����IDLE״̬���������κβ�����������ȷ�Ŀ���״̬��Ϊ��
                    break;

                case KEY_PRESSED_DEBOUNCE: // ����ȥ�˶�״̬
                    if (curr_state == RELEASED)
                    { // ��ȥ��ʱ�����ͷţ���Ϊ�Ƕ���������
                         keystatebuffer[i].key_state = KEY_IDLE; // �ص�����
                    }
                    else
                    { // ��Ȼ���£�����Ƿ�������ȥ��ʱ��
                         if (GetNowTime() - keystatebuffer[i].pressed_start_time >= DEBOUNCE_TIME)
                         {
                             keystatebuffer[i].key_state = KEY_PRESSED; // �ȶ����£����� KEY_PRESSED ״̬
                         }
                    }
                    break;


                case KEY_PRESSED: // �����ȶ�����״̬ (��ȥ��)
                    if(curr_state == RELEASED) // ��⵽�ͷ��¼�
                    {
                        uint32_t press_duration = GetNowTime() - keystatebuffer[i].pressed_start_time; // ���㰴��ʱ��
                        keystatebuffer[i].release_time = GetNowTime(); // ��¼�ͷ�ʱ�� (���ڼ���˫�����)

                        if(press_duration >= LONG_PRESS) // ������ֵ�ж�
                        {
                            // �������Ϊ��ס����Nʱ����ͷŴ���һ�γ����¼�
                            if (keystatebuffer[i].long_press_handled == 0) // ��鳤���¼��Ƿ����ڰ��¹����д���
                            {
                                // ��������£������¼������������� 'else' ��֧�д������ñ�־
                                // �����������Ϊ�ͷ�ʱ�����������������ñ�־��
                                keystatebuffer[i].event_handle = Event_LongPress;
							
                            }
                             // ������̰��ͷź󣬶���Ҫ�ص� IDLE ��ȴ�˫��״̬
                        }
                        else // �̰��ͷ� (����ʱ��С�� LONG_PRESS������ͨ��ȥ��)
                        {
                            keystatebuffer[i].click_count++; // ��Ч�̰��ͷż�����һ

                            if (keystatebuffer[i].click_count == 2) // ��������ﵽ2����⵽˫��
                            {
                                keystatebuffer[i].event_handle = Event_DoubleClick; // *** ��������˫���¼����� ***
                                keystatebuffer[i].click_count = 0; // ˫��������ϣ��������
                                keystatebuffer[i].key_state = KEY_IDLE; // �ص�����״̬
                            }
                            else // ���������1 (��һ����Ч�̰��ͷ�)
                            {
                                keystatebuffer[i].key_state = KEY_WAIT_INTERVAL; // ����ȴ�˫�����״̬
                                // release_time �Ѿ��������¼
                            }
                        }

                        // �����ǳ����ͷŻ��Ƕ̰��ͷŵ��µ�״̬�仯 (���˽��� KEY_WAIT_INTERVAL)
                        if (keystatebuffer[i].key_state != KEY_WAIT_INTERVAL) {
                            keystatebuffer[i].click_count = 0; // ���������� (����ǳ�����ȥ��ʧ�ܻص�IDLE) 
                            keystatebuffer[i].key_state = KEY_IDLE; // �ص�����״̬
                        }
                        keystatebuffer[i].long_press_handled = 0; // �ͷź����ó�����־
                    }
                    else // ��Ȼ���ְ���״̬ (curr_state == PRESSED)
                    {
                         // �ڰ��¹����г�����ⳤ��ʱ��
                         if (GetNowTime() - keystatebuffer[i].pressed_start_time >= LONG_PRESS && keystatebuffer[i].long_press_handled == 0)
                         {
                             keystatebuffer[i].event_handle = Event_LongPress; // *** �������ǳ����¼����� (��ס�ﵽʱ�䴥��һ��) ***
                             keystatebuffer[i].long_press_handled = 1; // ���ñ�־�����ⳤ�������ظ��������¼�
                             // �����Ҫ�����������߼�������ӣ���Ҫ�����ʱ���жϺ����û���
                         }
                    }
                    break;

                case KEY_WAIT_INTERVAL: // �ȴ�˫�����״̬ (�ڵ�һ�ζ̰��ͷź�)
                    if(curr_state == PRESSED) // *** ��⵽�ڶ��ΰ��� ***
                    {
                        // ��ʱ��⵽�ڶ��ΰ����ˣ���ʼ����ڶ��ε����ȥ���Ͱ��¹���
                        keystatebuffer[i].pressed_start_time = GetNowTime(); // ��¼�ڶ��ΰ��µ�ʱ��
                        keystatebuffer[i].key_state = KEY_PRESSED_DEBOUNCE; // ���밴�¶���״̬����ڶ��ΰ���
                        keystatebuffer[i].long_press_handled = 0; // ���ó�����־ (���ΰ�ѹ��Ӧ������֮ǰ����������) ȷ������ʱ���ᱻʶ��Ϊ����
                    }
                    else if(GetNowTime() - keystatebuffer[i].release_time > DOUBLE_INTERVAL) // ˫�������ʱ
                    {
                        if(keystatebuffer[i].click_count == 1) // ����ڼ����û�м�⵽�ڶ��ΰ��£���ֻ��һ�ε������
                        {
                            keystatebuffer[i].event_handle = Event_SingleClick; // *** ���Ϊ�����¼� ***
                        }
                        // ���۵��������1��������ֵ�������ϳ�ʱʱֻ��1��������bug������������󶼻ص�����
                        keystatebuffer[i].click_count = 0; // ����������
                        keystatebuffer[i].key_state = KEY_IDLE; // �ص�����״̬
                    }
                    // else: ��Ȼ���ͷ�״̬����δ��ʱ�������� KEY_WAIT_INTERVAL �ȴ�
                    break;
            }
			key_event_buffer[i]= keystatebuffer[i].event_handle;	//�Ѱ���״̬���ݸ������¼���������
        }
		
        // TIM_ClearITPendingBit �Ѿ����жϷ��������ڴ��������������ͨ���Ǹ���ȫ��������
        // ȷ����ִ���ж��߼�ʱ��־λ�Ѿ������������⺯��Ҫ�������������밴��Ҫ�����λ�á�
		
    }
	 
}


