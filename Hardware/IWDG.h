#ifndef __IWDG_H
#define __IWDG_H
#include "stm32f10x.h"                  // Device header

#include <stdio.h>  // 包含 sprintf 函数
#include <string.h> // 包含 strlen 函数 (可选，用于检查字符串长度)


void IWDG_Init(u8 prer,u16 rlr);//IWDG初始化
void IWDG_Feed(void);  //喂狗函数

#endif
