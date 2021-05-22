#pragma once
/*cpu state*/
#define USERMODE 0
#define KERNELMODE 1


/*process state*/
#define TASK_CREATING 0
#define TASK_READY 1
#define TASK_RUNNING 2
#define TASK_BLOCK 3
#define TASK_SUSPEND_BLOCK 4
#define TASK_SUSPEND_READY 5
#define TASK_DEAD 6

/*process priority*/
#define MAX_USER_RT_PRIO    0
#define MAX_RT_PRIO MAX_USER_RT_PRIO

/*
* Scheduling policies
*/
#define SCHED_FCFS              3
#define SCHED_RR                2
#define SCHED_PRO               1
#define SCHED_MFQ               5

#define MAX_BLOCK               0

#define IN 0
#define OUT 1

#define NOTBLOCK -1
#define KEYBOARD 0
#define PRINT 1

/*中断类型*/
#define NORMAL_SWITCH 0 //正常的进程切换 
#define PREEMPTION_SWITCH 1 //进程被抢占式 切换 
#define MIDTERM_SWITCH_OUT 2 //中期调度将进程完整换出
#define MIDTERM_SWITCH_IN 3 //中期调度将进程完整换入
#define KEYBOARD_SWITCH_USE 4 //键盘开始使用 
#define KEYBOARD_SWITCH_UNUSE 5 //键盘结束使用
#define PRINT_SWITCH_USE 6//打印机开始使用
#define PRINT_SWITCH_UNUSE 7//打印机结束使用
#define FILE_SWITCH_USE 8//文件占用开始
#define FILE_SWITCH_UNUSE 9//文件占用结束
#define PAGE_FAULT 12 //缺页中断

/*中断程序的入口地址(模拟)*/
#define PC_NORMAL_SWITCH 1000
#define PC_PREEMPTION_SWITCH 1001
#define PC_MIDTERM_SWITCH_OUT 1002
#define PC_MIDTERM_SWITCH_IN 1003
#define PC_KEYBOARD_SWITCH_USE 1004
#define PC_KEYBOARD_SWITCH_UNUSE 1005
#define PC_PRINT_SWITCH_USE 1006
#define PC_PRINT_SWITCH_UNUSE 1007
#define PC_PAGE_FAULT 1008