#ifndef _UI_H
#define _UI_H
// 定义菜单项结构体
#include "stm32f10x.h"                  // Device header
#include "stdint.h"
typedef enum {
     Unselected,
     Selected,
} ItemState;
typedef struct MenuItem {
     char *text;         // 菜单显示的文本
    void (*action)(void);     // 选中该菜单项时执行的函数指针
    struct MenuItem *subMenu; // 指向子菜单的指针，NULL表示没有子菜单
    struct MenuItem *parentMenu; // 指向父菜单的指针，用于返回上一级
	ItemState SelectedState; //菜单选项是否被选中
	uint8_t menuLocation; //该菜单的位号
	uint8_t menuSize; //该菜单栏目数量
} MenuItem;
typedef struct Menu
{
	MenuItem *menuItem;
	const int menuSize;
}Menu;
extern MenuItem mainMenuItems[];

extern int currentMenuItemsBufferSize;
extern MenuItem *currentMenu ;      // 当前显示的菜单数组

extern Menu menus[];

void MenuCountInit(void);
void MenuDisplay(int first_item_index,int if_selected);
void UI_Ctrl(void);
int constrain(int min,int max,int input);
#endif