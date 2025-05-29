#ifndef _RC_H
#define _RC_H
typedef enum 
{
	PowerOn, //电机上电
	Dog_Init, //机器人初始化
	Normal_Execute, //正常运行状态，这里执行机器人主要运行逻辑
	Jump_Ctrl, //进入跳跃控制
	Height_Adjust, //进入高度控制
	PowerOff   //断电
}Dog_State; //控制类状态机


#endif