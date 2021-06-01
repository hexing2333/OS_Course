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
	unsigned long task_size;//ռ���ڴ����
	unsigned long total_vm, locked_vm, shared_vm, exec_vm;
	//�û�̬��ջ��ҳ����
	unsigned long stack_vm, reserved_vm, def_flags, nr_ptes;
	//ά������κ����ݶ�
	unsigned long start_code, end_code, start_data, end_data;
	//ά���Ѻ�ջ
	unsigned long start_brk, brk, start_stack;

	bool is_apply;//�Ƿ�������ڴ�
}mm_struct;


typedef struct wait_time {
	cputime_t keyboardStartTime;
	cputime_t printStartTime;
	cputime_t readStartTime;
	cputime_t writeStartTime;
}wait_time;

typedef struct task_struct {
	unsigned short uid;//�û���ʶ
	int pid;//���̱�ʶ
	int processer;//�ദ���������ʹ�õ�CPU��
	int state;//��ʶ���̵�״̬
	int cpuState;//���������������Ȩ״̬ �û�̬/�ں�̬
	int prority;//�������ȼ�
	//unsigned int policy;//���̵��Ȳ���
	cputime_t utime;//����ռ��CPU�����ʱ��
	//cputime_t prev_cputime;//��ǰ������ʱ��
	wait_time wtime;
	//unsigned long nvcsw,nivcsw;//��Ը/����Դ�������л�����
	u64 start_time;//���̴���ʱ��
	u64 real_start_time;//����˯��ʱ��

	blockType type;

	string fs;//�ļ�ϵͳ����Ϣ��ָ��
	string fsState;//����R����W״̬
	//struct files_struct *files;//���ļ�����Ϣָ��

	struct mm_struct *mm;//�����ڴ������Ϣ

	list<string> program;//�����˳��ִ������
}PCB;

typedef struct mutexInfo {
	bool isBusy;
	list<PCB> waitForFileList;//�ȴ��ļ�����
};

//��ǰCPU�ĸ���״̬
typedef struct CPU {
	int PC;//���������
	int pid;//ռ��CPU��PCB
}CPU;



PCB create(int blocks, int prority, list<string>temp);//����ԭ��
void applyForResource(PCB &p);//������Դ
bool applyForMemory(PCB &p);//�����ڴ�
void releaseMemory(PCB &p);//�ͷ��ڴ�
void ready(PCB &p);//����ԭ��
void block(PCB &p);//����ԭ��
void stop(PCB &p);//����ԭ��
void suspend(PCB &p);//����ԭ��
void checkState();//��������н���״̬
void LongTermScheduler(string filename);//���ڵ��ȳ���
void MidTermScheduler(int inOrOut);//���ڵ��ȳ���
void CPUScheduler(PCB &p);//���ڵ��ȳ���
void Execute();//ִ�к���
void updateTaskState();//����״̬���º���
void changePCBList(PCB &p, int state);
void blockdelete(PCB &p);
void waitForKeyBoard();//���̵ȴ�����
void waitForPrint();//��ӡ���ȴ�����
void Interrupt(PCB &p, int reason);//�жϺ���
void printMemoryInfo();
void initMemory();



/*
C  time  ����ָ�ʹ��CPU��ʱ��time
K  time   �������룬ʱ��time
P  time   ��ӡ�������ʱ��time
R  filename  time  ���ļ���ʱ��time
W  filename  time  size  д�ļ���ʱ�����ļ���Сsize��
M  block  ����ռ���ڴ�ռ�
Y  number  ���̵�������
Q  ��������



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
const int PHYSICAL_SIZE = 8;
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
	int size; //�ܹ�ҳ��Ŀ������棩
	struct table_unit page_table[TABLE_LEN];
	int sequence[SEQ_LEN];
	int current_pointer;                   //��������ָ��
	int occupied_frames;                   //�ǿ���frame����
	int frames_number[FRAMES_PER_PROCESS]; //���������frame���
	int lru[FRAMES_PER_PROCESS];
	int alive; //�Ƿ����ڴ��� 0 ������ 1 ����
};

struct AllocateMsg
{
	int pid;
	int frameNumber[FRAMES_PER_PROCESS];
};
struct AllocateMsg allocateSuccess(PCB &p);

extern int occupied_size; //�ڴ��б�ռ��֡����Ŀ
extern process_info process_list[PROCESS_NUM];
extern frame_unit frame[PHYSICAL_SIZE];
bool handle_process(process_info &p);
bool medium_swap_in(PCB &p);
void medium_swap_out(int pid_out);
int getSize();
//void initMemory();


