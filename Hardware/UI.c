#include "UI.h"
#include "FONT.h"
#include "OLED.h"
#include "KEY.h"
#include "AD.h"
#include "RC.h"
#define MAXSTRINGLENGTH 20
#define STRINGNUM 4

// ע���Ҳ���� * ���� SUB_ACT_ITEM_t �˵�&��������ص�

extern CtrlState_t ctrl_state;
extern TranslateState_t translate_state;
extern JumpState_t jump_state;
extern uint8_t angle; 
extern uint8_t height;
extern IdleState_t idle_state;
extern char bat_value_str_buffer[20];

static uint8_t constrain(uint8_t value, uint8_t min, uint8_t max) { //�޷�����
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

//����˵�
//========================
//��ʼ�˵�
Menu_t startMenu;
//���˵�
Menu_t mainMenu;
//����ƽ�ƹ������ò˵�
Menu_t transSettingsMenu;
//��Ծ���ܲ˵�
Menu_t jumpctrlSettingsMenu;
//��Ծ׼�����ܲ˵�
Menu_t preJumpSettingsMenu;
//Idle or Stop Setting
Menu_t idleOrStopSettingsMenu;
//Settings
Menu_t settingsMenu;
//battery view
Menu_t batViewingMenu;
//������
//========================
//ƽ��
void trans_enable_func(void);
void trans_disable_func(void);
//idle or stop
void set_idle_func(void);
void set_stop_func(void);
//����&��б
void bend_func(void);
void lean_func(void);
//��Ծ׼���������� *
void jump_pre_func(void);
//��Ծִ��
void jump_exe_func(void);
//��Ծ�������� *
void jump_ctrl_func(void);
//�߶ȿ���
void height_func(void);
//���˵�����
void main_func(void);
//�˳�
void exit_func(void);
//��ʼ��
void init_func(void);
//��ʾ����
void show_bat_func(void);

//�˵�&������
//��Ծ����
SUB_ACT_ITEM_t jumpPreCmdItems;
//��Ծ׼��
SUB_ACT_ITEM_t jumpCtrlcmdItems;

//current
Menu_t *currentMenu=(Menu_t *)&startMenu;
void (*curren_func)(void)=NULL;

//�˵���
//========================
//battery viewing �鿴��ص���
 MenuItem_t batViewingItems[] = {
	{"Click This To Show:", MENU_ITEM_TYPE_ACTION, .data.actionFunc =show_bat_func},
};
// ���塰��ʾ���á��Ӳ˵�����
 Menu_t batViewingMenu = {
    .title = "BATTERY",
    .items =(MenuItem_t*) &batViewingItems,
    .itemCount = sizeof(batViewingItems) / sizeof(batViewingItems[0]),
	.startIndex= 0,
    .selectedIndex = 0, // ����˲˵�ʱĬ��ѡ�е���
    .parentMenu = &mainMenu // �Ժ����丸�˵�����
};
//setting
// ���塰��ʾ���á��Ӳ˵��Ĳ˵���
 MenuItem_t settingsItems[] = {
    {"Translation", MENU_ITEM_TYPE_SUBMENU, .data.subMenu =(Menu_t *)&transSettingsMenu},
    {"IDLE_or_STOP", MENU_ITEM_TYPE_SUBMENU, .data.subMenu =(Menu_t *)&idleOrStopSettingsMenu }, 
	{"Height", MENU_ITEM_TYPE_ACTION, .data.actionFunc =height_func },

};
// ���塰��ʾ���á��Ӳ˵�����
 Menu_t settingsMenu = {
    .title = "SETTINGS",
    .items =(MenuItem_t*) &settingsItems,
    .itemCount = sizeof(settingsItems) / sizeof(settingsItems[0]),
	.startIndex= 0,
    .selectedIndex = 0, // ����˲˵�ʱĬ��ѡ�е���
    .parentMenu = &mainMenu // �Ժ����丸�˵�����
};
 //Idle or Stop
// ���塰��ʾ���á��Ӳ˵��Ĳ˵���
 MenuItem_t idleOrStopSettingsItems[] = {
    {"IDLE", MENU_ITEM_TYPE_ACTION, .data.actionFunc = set_idle_func}, // ����һ������
    {"STOP", MENU_ITEM_TYPE_ACTION, .data.actionFunc = set_stop_func},       // �л�һ������
};
// ���塰��ʾ���á��Ӳ˵�����
 Menu_t idleOrStopSettingsMenu = {
    .title = "IDLE OR STOP",
    .items =(MenuItem_t*) &idleOrStopSettingsItems,
    .itemCount = sizeof(idleOrStopSettingsItems) / sizeof(idleOrStopSettingsItems[0]),
	.startIndex= 0,
    .selectedIndex = 0, // ����˲˵�ʱĬ��ѡ�е���
    .parentMenu = &settingsMenu // �Ժ����丸�˵�����
};
//translation
// ���塰��ʾ���á��Ӳ˵��Ĳ˵���
 MenuItem_t transSettingsItems[] = {
    {"TRANS ENABLE", MENU_ITEM_TYPE_ACTION, .data.actionFunc = trans_enable_func}, // ����һ������
    {"TRANS DISABLE", MENU_ITEM_TYPE_ACTION, .data.actionFunc = trans_disable_func},       // �л�һ������
};
// ���塰��ʾ���á��Ӳ˵�����
 Menu_t transSettingsMenu = {
    .title = "TRANS Setting",
    .items =(MenuItem_t*) &transSettingsItems,
    .itemCount = sizeof(transSettingsItems) / sizeof(transSettingsItems[0]),
	.startIndex= 0,
    .selectedIndex = 0, // ����˲˵�ʱĬ��ѡ�е���
    .parentMenu = &settingsMenu // �Ժ����丸�˵�����
};
//pre jump
// ���塰��ʾ���á��Ӳ˵��Ĳ˵���
 MenuItem_t preJumpSettingsItems[] = {
    {"BEND", MENU_ITEM_TYPE_ACTION, .data.actionFunc = bend_func}, // ����һ������
    {"LEAN", MENU_ITEM_TYPE_ACTION, .data.actionFunc = lean_func}, // ����һ������      
};
// ���塰��ʾ���á��Ӳ˵�����
 Menu_t preJumpSettingsMenu = {
    .title = "PRE JUMP",
    .items =(MenuItem_t*) &preJumpSettingsItems,
    .itemCount = sizeof(preJumpSettingsItems) / sizeof(preJumpSettingsItems[0]),
	.startIndex= 0,
    .selectedIndex = 0, // ����˲˵�ʱĬ��ѡ�е���
    .parentMenu = &jumpctrlSettingsMenu // �Ժ����丸�˵�����
};
 //jump ctrl
// ���塰��ʾ���á��Ӳ˵��Ĳ˵���
 MenuItem_t jumpctrlSettingsItems[] = {
    {"Prepare Jump", MENU_ITEM_TYPE_SUB_ACT, .data.sub_act_item= (SUB_ACT_ITEM_t *)&jumpPreCmdItems}, // ����һ������
    {"Execute Jump", MENU_ITEM_TYPE_ACTION, .data.actionFunc = jump_exe_func},       // �л�һ������
};
// ���塰��ʾ���á��Ӳ˵�����
 Menu_t jumpctrlSettingsMenu = {
    .title = "JumpSetting",
    .items =(MenuItem_t*) &jumpctrlSettingsItems,
    .itemCount = sizeof(jumpctrlSettingsItems) / sizeof(jumpctrlSettingsItems[0]),
	.startIndex= 0,
    .selectedIndex = 0, // ����˲˵�ʱĬ��ѡ�е���
    .parentMenu = &mainMenu // �Ժ����丸�˵�����
};
 //main
// ���塰�����á��˵��Ĳ˵���
 MenuItem_t mainMenuItems[] = {
    {"Main Ctrl", MENU_ITEM_TYPE_ACTION, .data.actionFunc = main_func}, // ���ӵ�����ʾ���á��Ӳ˵�
	{"Jump Ctrl", MENU_ITEM_TYPE_SUB_ACT, .data.sub_act_item = (SUB_ACT_ITEM_t *)&jumpCtrlcmdItems},  
	{"Settings",MENU_ITEM_TYPE_SUBMENU, .data.subMenu = (Menu_t *)&settingsMenu},  
	{"Battery", MENU_ITEM_TYPE_SUBMENU, .data.subMenu =(Menu_t *)&batViewingMenu }
};
// ���塰�����á��˵�����
 Menu_t mainMenu = {
    .title = "Main",
    .items = (MenuItem_t*) &mainMenuItems,
    .itemCount = sizeof(mainMenuItems) / sizeof(mainMenuItems[0]),
	.startIndex= 0,
    .selectedIndex = 0,
    .parentMenu = &startMenu // �Ժ����丸�˵�����
};
 //start
// ���塰���˵����Ĳ˵���
 MenuItem_t startMenuItems[] = {
    {"Start", MENU_ITEM_TYPE_SUBMENU, .data.subMenu = (Menu_t *)&mainMenu},
    {"Init", MENU_ITEM_TYPE_ACTION, .data.actionFunc = init_func}, // ���ӵ��������á��˵�
    {"Quit", MENU_ITEM_TYPE_ACTION, .data.actionFunc = exit_func},
};
// ���塰���˵�������
 Menu_t startMenu = {
    .title = "StartMenu",
    .items = (MenuItem_t*) &startMenuItems,
    .itemCount = sizeof(startMenuItems) / sizeof(startMenuItems[0]),
	.startIndex= 0,
    .selectedIndex = 0,
    .parentMenu = NULL // ���Ƕ����˵���û�и��˵�
};

//������
//===============================
//��ʾ����
void show_bat_func(void)
{
	OLED_PrintASCIIString(40,28,bat_value_str_buffer,&afont24x12,OLED_COLOR_REVERSED);
}
//ƽ���ຯ��
void trans_enable_func(void)
{
	translate_state=TRANS_ENABLE;
};
void trans_disable_func(void)
{
	translate_state=TRANS_DISABLE;
};
//����&��б����
void bend_func(void){
	jump_state=BEND;
};
void lean_func(void){
	jump_state=LEAN;
};
//��Ծ׼���������� ������Ծ׼���˵�ִ�еĺ��� *
void jump_pre_func(void){
	ctrl_state=CS_PRE_JUMP;
	OLED_PrintASCIIString(0,24+16,"pre_angle:",&afont12x6,OLED_COLOR_NORMAL);
	OLED_PrintASCIINum(80,24+16,angle,4,&afont12x6,OLED_COLOR_NORMAL);
	OLED_PrintASCIIString(0,36+16,"jump_state:",&afont12x6,OLED_COLOR_NORMAL);
	OLED_PrintASCIINum(80,36+16,jump_state,4,&afont12x6,OLED_COLOR_NORMAL);
	if(keystatebuffer[Keys_SW_UP].event_handle==Event_SingleClick) //�ϼ� Keys_SW_UP��ң������ߵ�
	{
		angle=constrain(++angle,0,30);
		keystatebuffer[Keys_SW_UP].event_handle=Event_None;
	}
	else if(keystatebuffer[Keys_SW_DOWN].event_handle==Event_SingleClick) //�¼�
	{
		angle=constrain(--angle,0,30);
		keystatebuffer[Keys_SW_DOWN].event_handle=Event_None;
	}
};
//��Ծִ��
void jump_exe_func(void){
	ctrl_state=CS_EXE_JUMP;
	jump_state=EXE;
};
//��Ծ���� ������Ծ���ò˵�ִ�еĺ��� *
void jump_ctrl_func(void)
{
	jump_state=IDLE;
	height=StandHeight;
}
//�߶ȿ��ƺ���
void height_func(void){
	ctrl_state=CS_HEIGHT;
	if(keystatebuffer[Keys_SW_UP].event_handle==Event_SingleClick) //�ϼ�
	{
		height=constrain(++height,CrouchHeight,HigherHeight);
		keystatebuffer[Keys_SW_UP].event_handle=Event_None;
	}
	else if(keystatebuffer[Keys_SW_DOWN].event_handle==Event_SingleClick) //�¼�
	{
		height=constrain(--height,CrouchHeight,HigherHeight);
		keystatebuffer[Keys_SW_DOWN].event_handle=Event_None;
	}
	OLED_PrintASCIINum(110,40,height,2,&afont12x6,OLED_COLOR_NORMAL);
};
//���˵�����
void main_func(void){
	ctrl_state=CS_MAIN;
	jump_state=IDLE;
};
//�˳�
void exit_func(void){
	ctrl_state=CS_QUIT;
};
//��ʼ��
void init_func(void){
	ctrl_state=CS_INIT;
};
//����ԭ��̤����־��
void set_idle_func(void)
{
	idle_state=NORMAL;
}
//����ֹͣ��־��
void set_stop_func(void)
{
	idle_state=STOP;
}

//�˵�&�������� *
//��Ծ׼�� 
SUB_ACT_ITEM_t jumpPreCmdItems=
{
	.subMenu=(Menu_t *)&preJumpSettingsMenu,
	.actionFunc=jump_pre_func
};
//��Ծ����
SUB_ACT_ITEM_t jumpCtrlcmdItems=
{
	.subMenu=(Menu_t *)&jumpctrlSettingsMenu,
	.actionFunc=jump_ctrl_func
};

//�˵������ƺ���
void menu_ctrl(void)
{
	if(keystatebuffer[Keys_SW_shang].event_handle==Event_SingleClick) //�ϼ�
	{
		menu_functions.switchMenu[0]();
		keystatebuffer[Keys_SW_shang].event_handle=Event_None;
	}
	if(keystatebuffer[Keys_SW_xia].event_handle==Event_SingleClick) //�¼�
	{
		menu_functions.switchMenu[1]();
		keystatebuffer[Keys_SW_xia].event_handle=Event_None;
	}
	if(keystatebuffer[Keys_WKUP].event_handle==Event_SingleClick) //Enter��
	{
		menu_functions.enterMenu();
		keystatebuffer[Keys_WKUP].event_handle=Event_None;
	}
	if(keystatebuffer[Keys_KEY_1].event_handle==Event_SingleClick) //Back��
	{
		menu_functions.backMenu();
		keystatebuffer[Keys_KEY_1].event_handle=Event_None;
	}
	menu_functions.drawMenu();
	if(curren_func!=NULL)
	{
		curren_func(); //func���������draw��ص��������Է���drawmenu�����Ա���Ը���menu
	}
}
//�˵�������������
void menu_draw(void);
void menu_enter(void);
void menu_back(void);
void menu_up(void);
void menu_down(void);
MenuFuctions_t menu_functions=
{
	.drawMenu=menu_draw,
	.enterMenu=menu_enter,
	.backMenu=menu_back,
	.switchMenu[0]=menu_up,
	.switchMenu[1]=menu_down,
};

void menu_draw(void) //ǰ��Ҫ��OLED_NewFrame�ͺ���OLED_ShowFrame
{
	size_t titlelen = strlen(currentMenu->title);
	OLED_PrintASCIIString(64-titlelen*4,0,(char*)currentMenu->title,&afont16x8,OLED_COLOR_NORMAL); //title
	for(int i=currentMenu->startIndex;i<currentMenu->itemCount;i++)
	{
		if(i==currentMenu->selectedIndex)
			OLED_PrintASCIIString(0,16+i*12,(char*)currentMenu->items[i].text,&afont12x6,OLED_COLOR_REVERSED); 
		else
			OLED_PrintASCIIString(0,16+i*12,(char*)currentMenu->items[i].text,&afont12x6,OLED_COLOR_NORMAL); 
	}
}
void menu_up(void)
{
	if(currentMenu->selectedIndex>0)
		currentMenu->selectedIndex--;
	else
		currentMenu->selectedIndex=currentMenu->itemCount-1; //ѭ���ص��˵�����ĩβ
	currentMenu->startIndex=(currentMenu->selectedIndex-3)<0?0:(currentMenu->selectedIndex-3);
}
void menu_down(void)
{
	if(currentMenu->selectedIndex<currentMenu->itemCount-1)
		currentMenu->selectedIndex++;
	else
		currentMenu->selectedIndex=0; //ѭ���ص��˵�������λ
	currentMenu->startIndex=(currentMenu->selectedIndex-3)<0?0:(currentMenu->selectedIndex-3);
}
void menu_enter(void)
{
	Menu_t *Menu=currentMenu;
	if(currentMenu->items[currentMenu->selectedIndex].type==MENU_ITEM_TYPE_SUBMENU)
	{
		if(currentMenu->items[currentMenu->selectedIndex].data.subMenu!=NULL)
		currentMenu=currentMenu->items[currentMenu->selectedIndex].data.subMenu;
		curren_func=NULL;
	}
	else if (currentMenu->items[currentMenu->selectedIndex].type==MENU_ITEM_TYPE_ACTION)
	{
		if(currentMenu->items[currentMenu->selectedIndex].data.actionFunc!=NULL)
		curren_func=currentMenu->items[currentMenu->selectedIndex].data.actionFunc;
	}
	else if (currentMenu->items[currentMenu->selectedIndex].type==MENU_ITEM_TYPE_SUB_ACT)
	{
		
		if(currentMenu->items[currentMenu->selectedIndex].data.sub_act_item->subMenu!=NULL)
		currentMenu=currentMenu->items[currentMenu->selectedIndex].data.sub_act_item->subMenu;
		
		if(Menu->items[Menu->selectedIndex].data.sub_act_item->actionFunc!=NULL) //������menu����Ϊcurrentmenu�Ѿ��ı䣬��������Ҫ��֮ǰ�����ֵ
		curren_func=Menu->items[Menu->selectedIndex].data.sub_act_item->actionFunc;
	}
}
void menu_back(void)
{
	if(currentMenu->parentMenu!=NULL)
	{
		currentMenu->selectedIndex=0;
		currentMenu->startIndex=0;
		currentMenu=currentMenu->parentMenu;
		curren_func=NULL;
	}
	
}
//void UI_Ctrl(void)
//{
//	//��ҡ�˾���UI
//		OLED_DrawImage(0+5,0,&rockerImg,OLED_COLOR_NORMAL);
//	//��ҡ�˾���UI
//		OLED_DrawImage(128-24-5,0,&rockerImg,OLED_COLOR_NORMAL);
//		OLED_DrawImage(128-24+1-5+20*AV_R_X_Sample/4095,1+20*AV_R_Y_Sample/4095,&circleImg,OLED_COLOR_NORMAL);

//		// if(AV_R_Y_Sample<2019)
//		// {
//		// 	OLED_DrawImage(128-24+10-5,1+10*AV_R_Y_Sample/2019,&circleImg,OLED_COLOR_NORMAL);
//		// }
//		// else
//		// {
//		// 	OLED_DrawImage(128-24+10-5,1+10*(AV_R_Y_Sample-2019)/2076,&circleImg,OLED_COLOR_NORMAL);
//		// }
//		// if(AV_R_X_Sample<2053)
//		// {
//		// 	OLED_DrawImage(128-24+10-5,1+10*AV_R_Y_Sample/2053,&circleImg,OLED_COLOR_NORMAL);
//		// }
//		// else
//		// {
//		// 	OLED_DrawImage(128-24+10-5,1+10*(AV_R_Y_Sample-2053)/2042,&circleImg,OLED_COLOR_NORMAL);
//		// }
//		
//		OLED_DrawImage(32-3,32,&buttomImg,OLED_COLOR_NORMAL); //shang
//		OLED_DrawImage(32-3,32+16,&buttomImg,OLED_COLOR_NORMAL); //xia
//		OLED_DrawImage(32-3-10,32+8,&buttomImg,OLED_COLOR_NORMAL); //zuo
//		OLED_DrawImage(32-3+10,32+8,&buttomImg,OLED_COLOR_NORMAL); //you
//		if(F_KEY[2]==1)
//		OLED_DrawImage(32-3+1,32+1,&circleImg,OLED_COLOR_NORMAL); //shang
//		if(F_KEY[3]==1)
//		OLED_DrawImage(32-3+1,32+16+1,&circleImg,OLED_COLOR_NORMAL); //xia
//		if(F_KEY[4]==1)
//		OLED_DrawImage(32-3-10+1,32+8+1,&circleImg,OLED_COLOR_NORMAL); //zuo
//		if(F_KEY[5]==1)
//		OLED_DrawImage(32-3+10+1,32+8+1,&circleImg,OLED_COLOR_NORMAL); //you
//			
//		OLED_DrawImage(32-3+64,32,&buttomImg,OLED_COLOR_NORMAL); //UP
//		OLED_DrawImage(32-3+64,32+16,&buttomImg,OLED_COLOR_NORMAL); //DOWM
//		OLED_DrawImage(32-3-10+64,32+8,&buttomImg,OLED_COLOR_NORMAL); //LEFT
//		OLED_DrawImage(32-3+10+64,32+8,&buttomImg,OLED_COLOR_NORMAL); //RIGHT
//		if(F_KEY[6]==1)
//		OLED_DrawImage(32-3+64+1,32+1,&circleImg,OLED_COLOR_NORMAL); //UP
//		if(F_KEY[7]==1)
//		OLED_DrawImage(32-3+64+1,32+16+1,&circleImg,OLED_COLOR_NORMAL); //DOWM
//		if(F_KEY[8]==1)
//		OLED_DrawImage(32-3-10+64+1,32+8+1,&circleImg,OLED_COLOR_NORMAL); //LEFT
//		if(F_KEY[9]==1)
//		OLED_DrawImage(32-3+10+64+1,32+8+1,&circleImg,OLED_COLOR_NORMAL); //RIGHT
//			

//		OLED_DrawImage(64+8-3,32-3-8,&buttomImg,OLED_COLOR_NORMAL); //KEY1
//		OLED_DrawImage(64-8-3,32-3-8,&buttomImg,OLED_COLOR_NORMAL);  //WKUP
//		if(F_KEY[0]==1)
//		OLED_DrawImage(64+8-3+1,32-3-8+1,&circleImg,OLED_COLOR_NORMAL); //KEY1
//		if(F_KEY[1]==1)
//		OLED_DrawImage(64-8-3+1,32-3-8+1,&circleImg,OLED_COLOR_NORMAL);  //WKUP
//}