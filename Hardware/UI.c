#include "UI.h"
#include "FONT.h"
#include "OLED.h"
#include "KEY.h"
#include "AD.h"
#define MAXSTRINGLENGTH 20
#define STRINGNUM 4


int constrain(int min,int max,int input)
{
	if(input<min)
	{
		input=min;
	}
	if(input>max)
	{
		input=max;
	}
		return input;
}

MenuItem _JumpCtrl_childrenMenuItems[2];
MenuItem _Main_childrenMenuItems[4];
MenuItem mainMenuItems[2];
//子菜单
MenuItem _JumpCtrl_childrenMenuItems[] = { //JumpCtrl子菜单
	{"Prepare_Jump", NULL, NULL, &_Main_childrenMenuItems[1], Selected,0,0},//准备跳跃，菜单需要列出跳跃的各种参数，比如伸腿长度，缩腿长度，跳跃时间等，可以调节
    {"Execute_Jump", NULL, NULL, &_Main_childrenMenuItems[1], Unselected,1,0}, //执行跳跃
};
MenuItem _Main_childrenMenuItems[] = { //main子菜单
	{"Main_Ctrl", NULL, NULL, &mainMenuItems[0], Selected,0,0}, //点击后进入主控制页面，能显示摇杆位置，以及机器狗实时状态
    {"JumpCtrl", NULL, _JumpCtrl_childrenMenuItems, &mainMenuItems[0], Unselected,1,0}, 
    {"HeightAdjust", NULL ,NULL, &mainMenuItems[0] ,Unselected,2,0}, //点击后直接列出各个腿的实时长度
	{"Dog_Init", NULL, NULL, &mainMenuItems[0], Unselected,3,0}, //点开按确定之后执行腿的初始化，可以看到腿电机转的角度
};

//主菜单
MenuItem mainMenuItems[] = {
    {"Main", NULL, _Main_childrenMenuItems, NULL, Selected,0,0}, //默认第一个被选中
    {"Settings", NULL, NULL, NULL, Unselected,1,0}, // 用来设置初始时gait数组的各种参数
};
Menu menus[]={
	{mainMenuItems,2},
	{_Main_childrenMenuItems,4},
	{_JumpCtrl_childrenMenuItems,2},
};

MenuItem *currentMenu = mainMenuItems;      // 当前显示的菜单数组
int currentMenuItemsBufferSize=sizeof(mainMenuItems)/sizeof(mainMenuItems[0]);
void MenuCountInit(void) //初始化menu的数量配置，比如主菜单mainMenu就有5个项
{
	int menus_counts=sizeof(menus)/sizeof(menus[0]);
	for(int i=0;i<menus_counts;i++)
	{
		for(int j=0;j<menus[i].menuSize;j++)
		menus[i].menuItem[j].menuSize=menus[i].menuSize; //初始化菜单大小
	}
}
void MenuDisplay(int first_item_index,int if_selected) //菜单显示，被选中的菜单为反色
{
	constrain(0,currentMenuItemsBufferSize,first_item_index);
	for(int i=first_item_index;i<currentMenuItemsBufferSize;i++)
	{
		if(i==if_selected)
		{
			currentMenu[i].SelectedState=Selected;
		}
		else
		{
			currentMenu[i].SelectedState=Unselected;
		}
		if(currentMenu[i].SelectedState==Selected)
		OLED_PrintASCIIString(0,16*(i-first_item_index),currentMenu[i].text,&afont16x8,OLED_COLOR_REVERSED);
		else
		OLED_PrintASCIIString(0,16*(i-first_item_index),currentMenu[i].text,&afont16x8,OLED_COLOR_NORMAL);
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