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

/*�ж�����*/
#define NORMAL_SWITCH 0 //�����Ľ����л� 
#define PREEMPTION_SWITCH 1 //���̱���ռʽ �л� 
#define MIDTERM_SWITCH_OUT 2 //���ڵ��Ƚ�������������
#define MIDTERM_SWITCH_IN 3 //���ڵ��Ƚ�������������
#define KEYBOARD_SWITCH_USE 4 //���̿�ʼʹ�� 
#define KEYBOARD_SWITCH_UNUSE 5 //���̽���ʹ��
#define PRINT_SWITCH_USE 6//��ӡ����ʼʹ��
#define PRINT_SWITCH_UNUSE 7//��ӡ������ʹ��
#define FILE_SWITCH_USE 8//�ļ�ռ�ÿ�ʼ
#define FILE_SWITCH_UNUSE 9//�ļ�ռ�ý���
#define PAGE_FAULT 12 //ȱҳ�ж�

/*�жϳ������ڵ�ַ(ģ��)*/
#define PC_NORMAL_SWITCH 1000
#define PC_PREEMPTION_SWITCH 1001
#define PC_MIDTERM_SWITCH_OUT 1002
#define PC_MIDTERM_SWITCH_IN 1003
#define PC_KEYBOARD_SWITCH_USE 1004
#define PC_KEYBOARD_SWITCH_UNUSE 1005
#define PC_PRINT_SWITCH_USE 1006
#define PC_PRINT_SWITCH_UNUSE 1007
#define PC_PAGE_FAULT 1008