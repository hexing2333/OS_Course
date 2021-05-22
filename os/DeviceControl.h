#pragma once
#include <iostream>

struct sdt //ϵͳ�豸��
{
	char name; //�豸����
	char type; //�豸����
	struct sdt* next;
};

struct block //�����豸
{
	//char pname; //�����豸������
	int pid;
	char ename; //�豸��
	struct block* next;
};

struct chct //ͨ��
{
	char name;          //ͨ������
	int state;          //ͨ��״̬
	struct block* next; //ͨ����ռ����ɵ���������
};

struct coct //��������
{
	char name;          //����������
	int state;          //������״̬
	struct chct* chct;  //�豸ͨ��
	struct block* next; //��������ռ����ɵ���������
};

struct dct //�豸���Ʊ�
{
	int state;          //�豸״̬
	//char pname;//ʹ�ý��̵�����
	int pid;
	struct sdt* sdt;    //ϵͳ�豸
	struct coct* coct;  //�豸������
	struct block* next; //�豸��ռ����ɵ���������
};

int DeviceControl(char c, char name, char type, int pid = -1);
void devicePrint();
void initDC();