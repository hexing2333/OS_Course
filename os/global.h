#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<math.h>
#include<algorithm>
#include<iostream>
#include<fstream>
#include<vector>
#include<queue>
#include<list>
#include<map>
#include<time.h>
#include<sstream>
using namespace std;

typedef int cputime_t;
typedef unsigned long long u64;
typedef int blockType;


typedef struct mm_struct {
	unsigned long task_size;
	unsigned long total_vm, locked_vm, shared_vm, exec_vm;
	//用户态堆栈的页数，
	unsigned long stack_vm, reserved_vm, def_flags, nr_ptes;
	//维护代码段和数据段
	unsigned long start_code, end_code, start_data, end_data;
	//维护堆和栈
	unsigned long start_brk, brk, start_stack;

	bool is_apply;//是否分配了内存
}mm_struct;


typedef struct wait_time {
	cputime_t keyboardStartTime;
	cputime_t printStartTime;
	cputime_t readStartTime;
	cputime_t writeStartTime;
}wait_time;

typedef struct task_struct {
	unsigned short uid;//用户标识
	int pid;//进程标识
	int processer;//多处理机进程正使用的CPU号
	int state;//标识进程的状态
	int cpuState;//定义进程所处的特权状态 用户态/内核态
	int prority;//进程优先级
	//unsigned int policy;//进程调度策略
	cputime_t utime;//进程占用CPU计算的时间
	//cputime_t prev_cputime;//先前的运行时间
	wait_time wtime;
	//unsigned long nvcsw,nivcsw;//自愿/非资源上下文切换计数
	u64 start_time;//进程创建时间
	u64 real_start_time;//进程睡眠时间

	blockType type;

	string fs;//文件系统的信息的指针
	string fsState;//处于R还是W状态
	//struct files_struct *files;//打开文件的信息指针

	struct mm_struct *mm;//进程内存管理信息

	list<string> program;//程序段顺序执行命令
}PCB;

typedef struct mutexInfo {
	bool isBusy;
	list<PCB> waitForFileList;
};

//当前CPU的各项状态
typedef struct CPU {
	int PC;//程序计数器
	int pid;//占用CPU的PCB
}CPU;



PCB create(int blocks, int prority, list<string>temp);
void applyForResource(PCB &p);
bool applyForMemory(PCB &p);


void releaseMemory(PCB &p);
void running();
void ready(PCB &p);
void block(PCB &p);
void stop(PCB &p);
void suspend();
void schedule();
void checkState();
void LongTermScheduler(string filename);
void MidTermScheduler(int inOrOut);
void CPUScheduler(PCB &p);
void Execute();
void updateTaskState();
void changePCBList(PCB &p, int state);
void blockdelete(PCB &p);
void waitForKeyBoard();
void waitForPrint();
void Interrupt(PCB &p, int reason);
void printMemoryInfo();
void initMemory();



/*
C  time  计算指令，使用CPU，时长time
K  time   键盘输入，时长time
P  time   打印机输出，时长time
R  filename  time  读文件，时长time
W  filename  time  size  写文件，时长，文件大小size块
M  block  进程占用内存空间
Y  number  进程的优先数
Q  结束运行



*/
/*
 * @Author: your name
 * @Date: 2021-05-05 13:52:52
 * @LastEditTime: 2021-05-13 19:51:07
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \process\memory.h
 */



 // #define PHYSICAL_SIZE 100
 // #define FRAMES_PER_PROCESS 3
 // #define TABLE_LEN 10
 // #define SEQ_LEN 100
 // #define PROCESS_NUM 33
const int PHYSICAL_SIZE = 9;
const int FRAMES_PER_PROCESS = 3;
const int TABLE_LEN = 1000;
const int SEQ_LEN = 100;
const int PROCESS_NUM = PHYSICAL_SIZE / FRAMES_PER_PROCESS;
struct table_unit
{
	int frame_no;
	int valid;
};
struct frame_unit
{
	int pid;
	int ifUse;
};

struct process_info
{
	int pid;
	int size; //总共页数目（内外存）
	struct table_unit page_table[TABLE_LEN];
	int sequence[SEQ_LEN];
	int current_pointer;                   //访问序列指针
	int occupied_frames;                   //非空闲frame数量
	int frames_number[FRAMES_PER_PROCESS]; //分配的所有frame编号
	int lru[FRAMES_PER_PROCESS];
	int alive; //是否还在内存中 0 不存在 1 存在
};

struct AllocateMsg
{
	int pid;
	int frameNumber[FRAMES_PER_PROCESS];
};
struct AllocateMsg allocateSuccess(PCB &p);

extern int occupied_size; //内存中被占用帧的数目
extern process_info process_list[PROCESS_NUM];
extern frame_unit frame[PHYSICAL_SIZE];
bool handle_process(process_info &p);
bool medium_swap_in(PCB &p);
void medium_swap_out(int pid_out);
int getSize();
//void initMemory();


