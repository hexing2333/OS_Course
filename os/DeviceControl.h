#pragma once
#include <iostream>

struct sdt //系统设备表
{
	char name; //设备名称
	char type; //设备类型
	struct sdt* next;
};

struct block //阻塞设备
{
	//char pname; //申请设备进程名
	int pid;
	char ename; //设备名
	struct block* next;
};

struct chct //通道
{
	char name;          //通道名称
	int state;          //通道状态
	struct block* next; //通道被占用造成的阻塞队列
};

struct coct //控制器表
{
	char name;          //控制器名称
	int state;          //控制器状态
	struct chct* chct;  //设备通道
	struct block* next; //控制器被占用造成的阻塞队列
};

struct dct //设备控制表
{
	int state;          //设备状态
	//char pname;//使用进程的名称
	int pid;
	struct sdt* sdt;    //系统设备
	struct coct* coct;  //设备控制器
	struct block* next; //设备被占用造成的阻塞队列
};

int DeviceControl(char c, char name, char type, int pid = -1);
void devicePrint();
void initDC();