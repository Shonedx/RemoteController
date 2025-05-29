#ifndef _UI_H
#define _UI_H
// ����˵���ṹ��
#include "stm32f10x.h"                  // Device header
#include "stdint.h"
typedef enum {
     Unselected,
     Selected,
} ItemState;
typedef struct MenuItem {
     char *text;         // �˵���ʾ���ı�
    void (*action)(void);     // ѡ�иò˵���ʱִ�еĺ���ָ��
    struct MenuItem *subMenu; // ָ���Ӳ˵���ָ�룬NULL��ʾû���Ӳ˵�
    struct MenuItem *parentMenu; // ָ�򸸲˵���ָ�룬���ڷ�����һ��
	ItemState SelectedState; //�˵�ѡ���Ƿ�ѡ��
	uint8_t menuLocation; //�ò˵���λ��
	uint8_t menuSize; //�ò˵���Ŀ����
} MenuItem;
typedef struct Menu
{
	MenuItem *menuItem;
	const int menuSize;
}Menu;
extern MenuItem mainMenuItems[];

extern int currentMenuItemsBufferSize;
extern MenuItem *currentMenu ;      // ��ǰ��ʾ�Ĳ˵�����

extern Menu menus[];

void MenuCountInit(void);
void MenuDisplay(int first_item_index,int if_selected);
void UI_Ctrl(void);
int constrain(int min,int max,int input);
#endif