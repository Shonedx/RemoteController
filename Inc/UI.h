#ifndef _UI_H
#define _UI_H
// 定义菜单项结构体
#include "stm32f10x.h"                  // Device header
#include "stdint.h"

typedef enum {
    MENU_ITEM_TYPE_NORMAL,      // 普通菜单项
    MENU_ITEM_TYPE_SUBMENU,     // 子菜单项
    MENU_ITEM_TYPE_ACTION,      // 带有动作的菜单项（如执行某个功能）
	MENU_ITEM_TYPE_SUB_ACT,     //子菜单和动作项目的结合，即选中时进入子菜单并执行对应函数
    MENU_ITEM_TYPE_TOGGLE,      // 开关型菜单项
    // ... 其他类型
} MenuItemType_t;

typedef struct SUB_ACT_ITEM {
    struct Menu *subMenu;   // 如果是子菜单类型，指向子菜单
	void (*actionFunc)(void); // 如果是动作类型，指向执行函数
} SUB_ACT_ITEM_t;

// 菜单项结构体
typedef struct MenuItem {
    const char *text;           // 菜单项显示的文本
    MenuItemType_t type;        // 菜单项类型
    union {
        struct Menu *subMenu;   // 如果是子菜单类型，指向子菜单
        void (*actionFunc)(void); // 如果是动作类型，指向执行函数
        uint8_t *toggleVar;        // 如果是开关类型，指向布尔变量
		struct SUB_ACT_ITEM *sub_act_item;
        // ... 其他类型的数据
    } data;
} MenuItem_t;

// 菜单结构体（表示一个菜单层级）
typedef struct Menu {
    const char *title;          // 菜单标题
    const MenuItem_t *items;    // 菜单项数组
    uint8_t itemCount;          // 菜单项数量
	uint8_t startIndex;
    uint8_t selectedIndex;      // 当前选中的菜单项索引
    struct Menu *parentMenu;    // 指向父菜单，用于返回上一级
} Menu_t;

typedef void (*switchMenu_t)(void);

typedef struct MenuFuctions{
	void(*drawMenu)(void);
	void(*enterMenu)(void);
	void(*backMenu)(void);
	switchMenu_t switchMenu[2];
}MenuFuctions_t;

extern MenuFuctions_t menu_functions;

void menu_ctrl(void);
void UI_Ctrl(void);

#endif