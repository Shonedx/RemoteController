#include "UI.h"
#include "FONT.h"
#include "OLED.h"
#include "KEY.h"
#include "AD.h"
#include "RC.h"
#define MAXSTRINGLENGTH 20
#define STRINGNUM 4

// 注释右侧带有 * 的是 SUB_ACT_ITEM_t 菜单&函数类相关的

extern CtrlState_t ctrl_state;
extern TranslateState_t translate_state;
extern JumpState_t jump_state;
extern uint8_t angle; 
extern uint8_t height;
extern IdleState_t idle_state;
extern char bat_value_str_buffer[20];

static uint8_t constrain(uint8_t value, uint8_t min, uint8_t max) { //限幅函数
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

//各类菜单
//========================
//开始菜单
Menu_t startMenu;
//主菜单
Menu_t mainMenu;
//四足平移功能设置菜单
Menu_t transSettingsMenu;
//跳跃功能菜单
Menu_t jumpctrlSettingsMenu;
//跳跃准备功能菜单
Menu_t preJumpSettingsMenu;
//Idle or Stop Setting
Menu_t idleOrStopSettingsMenu;
//Settings
Menu_t settingsMenu;
//battery view
Menu_t batViewingMenu;
//各函数
//========================
//平移
void trans_enable_func(void);
void trans_disable_func(void);
//idle or stop
void set_idle_func(void);
void set_stop_func(void);
//俯身&倾斜
void bend_func(void);
void lean_func(void);
//跳跃准备触发函数 *
void jump_pre_func(void);
//跳跃执行
void jump_exe_func(void);
//跳跃触发函数 *
void jump_ctrl_func(void);
//高度控制
void height_func(void);
//主菜单函数
void main_func(void);
//退出
void exit_func(void);
//初始化
void init_func(void);
//显示电量
void show_bat_func(void);

//菜单&函数类
//跳跃控制
SUB_ACT_ITEM_t jumpPreCmdItems;
//跳跃准备
SUB_ACT_ITEM_t jumpCtrlcmdItems;

//current
Menu_t *currentMenu=(Menu_t *)&startMenu;
void (*curren_func)(void)=NULL;

//菜单项
//========================
//battery viewing 查看电池电量
 MenuItem_t batViewingItems[] = {
	{"Click This To Show:", MENU_ITEM_TYPE_ACTION, .data.actionFunc =show_bat_func},
};
// 定义“显示设置”子菜单本身
 Menu_t batViewingMenu = {
    .title = "BATTERY",
    .items =(MenuItem_t*) &batViewingItems,
    .itemCount = sizeof(batViewingItems) / sizeof(batViewingItems[0]),
	.startIndex= 0,
    .selectedIndex = 0, // 进入此菜单时默认选中的项
    .parentMenu = &mainMenu // 稍后由其父菜单链接
};
//setting
// 定义“显示设置”子菜单的菜单项
 MenuItem_t settingsItems[] = {
    {"Translation", MENU_ITEM_TYPE_SUBMENU, .data.subMenu =(Menu_t *)&transSettingsMenu},
    {"IDLE_or_STOP", MENU_ITEM_TYPE_SUBMENU, .data.subMenu =(Menu_t *)&idleOrStopSettingsMenu }, 
	{"Height", MENU_ITEM_TYPE_ACTION, .data.actionFunc =height_func },

};
// 定义“显示设置”子菜单本身
 Menu_t settingsMenu = {
    .title = "SETTINGS",
    .items =(MenuItem_t*) &settingsItems,
    .itemCount = sizeof(settingsItems) / sizeof(settingsItems[0]),
	.startIndex= 0,
    .selectedIndex = 0, // 进入此菜单时默认选中的项
    .parentMenu = &mainMenu // 稍后由其父菜单链接
};
 //Idle or Stop
// 定义“显示设置”子菜单的菜单项
 MenuItem_t idleOrStopSettingsItems[] = {
    {"IDLE", MENU_ITEM_TYPE_ACTION, .data.actionFunc = set_idle_func}, // 调用一个函数
    {"STOP", MENU_ITEM_TYPE_ACTION, .data.actionFunc = set_stop_func},       // 切换一个变量
};
// 定义“显示设置”子菜单本身
 Menu_t idleOrStopSettingsMenu = {
    .title = "IDLE OR STOP",
    .items =(MenuItem_t*) &idleOrStopSettingsItems,
    .itemCount = sizeof(idleOrStopSettingsItems) / sizeof(idleOrStopSettingsItems[0]),
	.startIndex= 0,
    .selectedIndex = 0, // 进入此菜单时默认选中的项
    .parentMenu = &settingsMenu // 稍后由其父菜单链接
};
//translation
// 定义“显示设置”子菜单的菜单项
 MenuItem_t transSettingsItems[] = {
    {"TRANS ENABLE", MENU_ITEM_TYPE_ACTION, .data.actionFunc = trans_enable_func}, // 调用一个函数
    {"TRANS DISABLE", MENU_ITEM_TYPE_ACTION, .data.actionFunc = trans_disable_func},       // 切换一个变量
};
// 定义“显示设置”子菜单本身
 Menu_t transSettingsMenu = {
    .title = "TRANS Setting",
    .items =(MenuItem_t*) &transSettingsItems,
    .itemCount = sizeof(transSettingsItems) / sizeof(transSettingsItems[0]),
	.startIndex= 0,
    .selectedIndex = 0, // 进入此菜单时默认选中的项
    .parentMenu = &settingsMenu // 稍后由其父菜单链接
};
//pre jump
// 定义“显示设置”子菜单的菜单项
 MenuItem_t preJumpSettingsItems[] = {
    {"BEND", MENU_ITEM_TYPE_ACTION, .data.actionFunc = bend_func}, // 调用一个函数
    {"LEAN", MENU_ITEM_TYPE_ACTION, .data.actionFunc = lean_func}, // 调用一个函数      
};
// 定义“显示设置”子菜单本身
 Menu_t preJumpSettingsMenu = {
    .title = "PRE JUMP",
    .items =(MenuItem_t*) &preJumpSettingsItems,
    .itemCount = sizeof(preJumpSettingsItems) / sizeof(preJumpSettingsItems[0]),
	.startIndex= 0,
    .selectedIndex = 0, // 进入此菜单时默认选中的项
    .parentMenu = &jumpctrlSettingsMenu // 稍后由其父菜单链接
};
 //jump ctrl
// 定义“显示设置”子菜单的菜单项
 MenuItem_t jumpctrlSettingsItems[] = {
    {"Prepare Jump", MENU_ITEM_TYPE_SUB_ACT, .data.sub_act_item= (SUB_ACT_ITEM_t *)&jumpPreCmdItems}, // 调用一个函数
    {"Execute Jump", MENU_ITEM_TYPE_ACTION, .data.actionFunc = jump_exe_func},       // 切换一个变量
};
// 定义“显示设置”子菜单本身
 Menu_t jumpctrlSettingsMenu = {
    .title = "JumpSetting",
    .items =(MenuItem_t*) &jumpctrlSettingsItems,
    .itemCount = sizeof(jumpctrlSettingsItems) / sizeof(jumpctrlSettingsItems[0]),
	.startIndex= 0,
    .selectedIndex = 0, // 进入此菜单时默认选中的项
    .parentMenu = &mainMenu // 稍后由其父菜单链接
};
 //main
// 定义“主设置”菜单的菜单项
 MenuItem_t mainMenuItems[] = {
    {"Main Ctrl", MENU_ITEM_TYPE_ACTION, .data.actionFunc = main_func}, // 链接到“显示设置”子菜单
	{"Jump Ctrl", MENU_ITEM_TYPE_SUB_ACT, .data.sub_act_item = (SUB_ACT_ITEM_t *)&jumpCtrlcmdItems},  
	{"Settings",MENU_ITEM_TYPE_SUBMENU, .data.subMenu = (Menu_t *)&settingsMenu},  
	{"Battery", MENU_ITEM_TYPE_SUBMENU, .data.subMenu =(Menu_t *)&batViewingMenu }
};
// 定义“主设置”菜单本身
 Menu_t mainMenu = {
    .title = "Main",
    .items = (MenuItem_t*) &mainMenuItems,
    .itemCount = sizeof(mainMenuItems) / sizeof(mainMenuItems[0]),
	.startIndex= 0,
    .selectedIndex = 0,
    .parentMenu = &startMenu // 稍后由其父菜单链接
};
 //start
// 定义“主菜单”的菜单项
 MenuItem_t startMenuItems[] = {
    {"Start", MENU_ITEM_TYPE_SUBMENU, .data.subMenu = (Menu_t *)&mainMenu},
    {"Init", MENU_ITEM_TYPE_ACTION, .data.actionFunc = init_func}, // 链接到“主设置”菜单
    {"Quit", MENU_ITEM_TYPE_ACTION, .data.actionFunc = exit_func},
};
// 定义“主菜单”本身
 Menu_t startMenu = {
    .title = "StartMenu",
    .items = (MenuItem_t*) &startMenuItems,
    .itemCount = sizeof(startMenuItems) / sizeof(startMenuItems[0]),
	.startIndex= 0,
    .selectedIndex = 0,
    .parentMenu = NULL // 这是顶级菜单，没有父菜单
};

//函数项
//===============================
//显示电量
void show_bat_func(void)
{
	OLED_PrintASCIIString(40,28,bat_value_str_buffer,&afont24x12,OLED_COLOR_REVERSED);
}
//平移类函数
void trans_enable_func(void)
{
	translate_state=TRANS_ENABLE;
};
void trans_disable_func(void)
{
	translate_state=TRANS_DISABLE;
};
//俯身&倾斜函数
void bend_func(void){
	jump_state=BEND;
};
void lean_func(void){
	jump_state=LEAN;
};
//跳跃准备触发函数 进入跳跃准备菜单执行的函数 *
void jump_pre_func(void){
	ctrl_state=CS_PRE_JUMP;
	OLED_PrintASCIIString(0,24+16,"pre_angle:",&afont12x6,OLED_COLOR_NORMAL);
	OLED_PrintASCIINum(80,24+16,angle,4,&afont12x6,OLED_COLOR_NORMAL);
	OLED_PrintASCIIString(0,36+16,"jump_state:",&afont12x6,OLED_COLOR_NORMAL);
	OLED_PrintASCIINum(80,36+16,jump_state,4,&afont12x6,OLED_COLOR_NORMAL);
	if(keystatebuffer[Keys_SW_UP].event_handle==Event_SingleClick) //上键 Keys_SW_UP是遥控器左边的
	{
		angle=constrain(++angle,0,30);
		keystatebuffer[Keys_SW_UP].event_handle=Event_None;
	}
	else if(keystatebuffer[Keys_SW_DOWN].event_handle==Event_SingleClick) //下键
	{
		angle=constrain(--angle,0,30);
		keystatebuffer[Keys_SW_DOWN].event_handle=Event_None;
	}
};
//跳跃执行
void jump_exe_func(void){
	ctrl_state=CS_EXE_JUMP;
	jump_state=EXE;
};
//跳跃控制 进入跳跃设置菜单执行的函数 *
void jump_ctrl_func(void)
{
	jump_state=IDLE;
	height=StandHeight;
}
//高度控制函数
void height_func(void){
	ctrl_state=CS_HEIGHT;
	if(keystatebuffer[Keys_SW_UP].event_handle==Event_SingleClick) //上键
	{
		height=constrain(++height,CrouchHeight,HigherHeight);
		keystatebuffer[Keys_SW_UP].event_handle=Event_None;
	}
	else if(keystatebuffer[Keys_SW_DOWN].event_handle==Event_SingleClick) //下键
	{
		height=constrain(--height,CrouchHeight,HigherHeight);
		keystatebuffer[Keys_SW_DOWN].event_handle=Event_None;
	}
	OLED_PrintASCIINum(110,40,height,2,&afont12x6,OLED_COLOR_NORMAL);
};
//主菜单函数
void main_func(void){
	ctrl_state=CS_MAIN;
	jump_state=IDLE;
};
//退出
void exit_func(void){
	ctrl_state=CS_QUIT;
};
//初始化
void init_func(void){
	ctrl_state=CS_INIT;
};
//设置原地踏步标志量
void set_idle_func(void)
{
	idle_state=NORMAL;
}
//设置停止标志量
void set_stop_func(void)
{
	idle_state=STOP;
}

//菜单&函数类项 *
//跳跃准备 
SUB_ACT_ITEM_t jumpPreCmdItems=
{
	.subMenu=(Menu_t *)&preJumpSettingsMenu,
	.actionFunc=jump_pre_func
};
//跳跃控制
SUB_ACT_ITEM_t jumpCtrlcmdItems=
{
	.subMenu=(Menu_t *)&jumpctrlSettingsMenu,
	.actionFunc=jump_ctrl_func
};

//菜单主控制函数
void menu_ctrl(void)
{
	if(keystatebuffer[Keys_SW_shang].event_handle==Event_SingleClick) //上键
	{
		menu_functions.switchMenu[0]();
		keystatebuffer[Keys_SW_shang].event_handle=Event_None;
	}
	if(keystatebuffer[Keys_SW_xia].event_handle==Event_SingleClick) //下键
	{
		menu_functions.switchMenu[1]();
		keystatebuffer[Keys_SW_xia].event_handle=Event_None;
	}
	if(keystatebuffer[Keys_WKUP].event_handle==Event_SingleClick) //Enter键
	{
		menu_functions.enterMenu();
		keystatebuffer[Keys_WKUP].event_handle=Event_None;
	}
	if(keystatebuffer[Keys_KEY_1].event_handle==Event_SingleClick) //Back键
	{
		menu_functions.backMenu();
		keystatebuffer[Keys_KEY_1].event_handle=Event_None;
	}
	menu_functions.drawMenu();
	if(curren_func!=NULL)
	{
		curren_func(); //func里面可能有draw相关的内容所以放在drawmenu后面以便可以覆盖menu
	}
}
//菜单操作函数数组
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

void menu_draw(void) //前面要配OLED_NewFrame和后面OLED_ShowFrame
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
		currentMenu->selectedIndex=currentMenu->itemCount-1; //循环回到菜单项最末尾
	currentMenu->startIndex=(currentMenu->selectedIndex-3)<0?0:(currentMenu->selectedIndex-3);
}
void menu_down(void)
{
	if(currentMenu->selectedIndex<currentMenu->itemCount-1)
		currentMenu->selectedIndex++;
	else
		currentMenu->selectedIndex=0; //循环回到菜单项最首位
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
		
		if(Menu->items[Menu->selectedIndex].data.sub_act_item->actionFunc!=NULL) //这里用menu是因为currentmenu已经改变，所以我们要用之前储存得值
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
//	//左摇杆居中UI
//		OLED_DrawImage(0+5,0,&rockerImg,OLED_COLOR_NORMAL);
//	//右摇杆居中UI
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