#ifndef __IWDG_H
#define __IWDG_H
#include "stm32f10x.h"                  // Device header

#include <stdio.h>  // ���� sprintf ����
#include <string.h> // ���� strlen ���� (��ѡ�����ڼ���ַ�������)


void IWDG_Init(u8 prer,u16 rlr);//IWDG��ʼ��
void IWDG_Feed(void);  //ι������

#endif
