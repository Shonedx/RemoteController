#ifndef _UI_H
#define _UI_H
// ����˵���ṹ��
#include "stm32f10x.h"                  // Device header
#include "stdint.h"

typedef enum {
    MENU_ITEM_TYPE_NORMAL,      // ��ͨ�˵���
    MENU_ITEM_TYPE_SUBMENU,     // �Ӳ˵���
    MENU_ITEM_TYPE_ACTION,      // ���ж����Ĳ˵����ִ��ĳ�����ܣ�
	MENU_ITEM_TYPE_SUB_ACT,     //�Ӳ˵��Ͷ�����Ŀ�Ľ�ϣ���ѡ��ʱ�����Ӳ˵���ִ�ж�Ӧ����
    MENU_ITEM_TYPE_TOGGLE,      // �����Ͳ˵���
    // ... ��������
} MenuItemType_t;

typedef struct SUB_ACT_ITEM {
    struct Menu *subMenu;   // ������Ӳ˵����ͣ�ָ���Ӳ˵�
	void (*actionFunc)(void); // ����Ƕ������ͣ�ָ��ִ�к���
} SUB_ACT_ITEM_t;

// �˵���ṹ��
typedef struct MenuItem {
    const char *text;           // �˵�����ʾ���ı�
    MenuItemType_t type;        // �˵�������
    union {
        struct Menu *subMenu;   // ������Ӳ˵����ͣ�ָ���Ӳ˵�
        void (*actionFunc)(void); // ����Ƕ������ͣ�ָ��ִ�к���
        uint8_t *toggleVar;        // ����ǿ������ͣ�ָ�򲼶�����
		struct SUB_ACT_ITEM *sub_act_item;
        // ... �������͵�����
    } data;
} MenuItem_t;

// �˵��ṹ�壨��ʾһ���˵��㼶��
typedef struct Menu {
    const char *title;          // �˵�����
    const MenuItem_t *items;    // �˵�������
    uint8_t itemCount;          // �˵�������
	uint8_t startIndex;
    uint8_t selectedIndex;      // ��ǰѡ�еĲ˵�������
    struct Menu *parentMenu;    // ָ�򸸲˵������ڷ�����һ��
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