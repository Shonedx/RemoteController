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
//�Ӳ˵�
MenuItem _JumpCtrl_childrenMenuItems[] = { //JumpCtrl�Ӳ˵�
	{"Prepare_Jump", NULL, NULL, &_Main_childrenMenuItems[1], Selected,0,0},//׼����Ծ���˵���Ҫ�г���Ծ�ĸ��ֲ������������ȳ��ȣ����ȳ��ȣ���Ծʱ��ȣ����Ե���
    {"Execute_Jump", NULL, NULL, &_Main_childrenMenuItems[1], Unselected,1,0}, //ִ����Ծ
};
MenuItem _Main_childrenMenuItems[] = { //main�Ӳ˵�
	{"Main_Ctrl", NULL, NULL, &mainMenuItems[0], Selected,0,0}, //��������������ҳ�棬����ʾҡ��λ�ã��Լ�������ʵʱ״̬
    {"JumpCtrl", NULL, _JumpCtrl_childrenMenuItems, &mainMenuItems[0], Unselected,1,0}, 
    {"HeightAdjust", NULL ,NULL, &mainMenuItems[0] ,Unselected,2,0}, //�����ֱ���г������ȵ�ʵʱ����
	{"Dog_Init", NULL, NULL, &mainMenuItems[0], Unselected,3,0}, //�㿪��ȷ��֮��ִ���ȵĳ�ʼ�������Կ����ȵ��ת�ĽǶ�
};

//���˵�
MenuItem mainMenuItems[] = {
    {"Main", NULL, _Main_childrenMenuItems, NULL, Selected,0,0}, //Ĭ�ϵ�һ����ѡ��
    {"Settings", NULL, NULL, NULL, Unselected,1,0}, // �������ó�ʼʱgait����ĸ��ֲ���
};
Menu menus[]={
	{mainMenuItems,2},
	{_Main_childrenMenuItems,4},
	{_JumpCtrl_childrenMenuItems,2},
};

MenuItem *currentMenu = mainMenuItems;      // ��ǰ��ʾ�Ĳ˵�����
int currentMenuItemsBufferSize=sizeof(mainMenuItems)/sizeof(mainMenuItems[0]);
void MenuCountInit(void) //��ʼ��menu���������ã��������˵�mainMenu����5����
{
	int menus_counts=sizeof(menus)/sizeof(menus[0]);
	for(int i=0;i<menus_counts;i++)
	{
		for(int j=0;j<menus[i].menuSize;j++)
		menus[i].menuItem[j].menuSize=menus[i].menuSize; //��ʼ���˵���С
	}
}
void MenuDisplay(int first_item_index,int if_selected) //�˵���ʾ����ѡ�еĲ˵�Ϊ��ɫ
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