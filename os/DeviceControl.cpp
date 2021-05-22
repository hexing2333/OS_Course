#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include "DeviceControl.h"
using namespace std;
//����
sdt* s[20];      //�豸
dct* d[20];      //�豸����
coct* co[20];    //������
chct* ch1, *ch2; //ͨ��1,ͨ��2
block* b;
int e = 4; //�豸��,��ʼΪ4
int c = 3; //��������,��ʼΪ3
char controller = 'a';
void devicePrint();

//��ʼ��
void initDC() //��ʼ��
{
	for (int i = 0; i < 4; i++) {
		s[i] = new (sdt);
		d[i] = new (dct);
		d[i]->state = 0;
		d[i]->sdt = s[i];
		d[i]->next = new (block);
		d[i]->next->next = NULL;
	}
	s[0]->name = 'k';
	s[1]->name = 'm';
	s[2]->name = 't';
	s[3]->name = 'p';
	s[0]->type = 'i';
	s[1]->type = 'i';
	s[2]->type = 'o';
	s[3]->type = 'o';

	for (int i = 1; i < 4; i++) {
		co[i] = new (coct);
		co[i]->state = 0;
		co[i]->next = new (block);
		co[i]->next->next = NULL;
	}
	co[1]->name = '1';
	co[2]->name = '2';
	co[3]->name = '3';

	ch1 = new (chct);
	ch2 = new (chct);
	ch1->name = '1';
	ch1->state = 0;
	ch1->next = new (block);
	ch1->next->next = NULL;
	ch2->name = '1';
	ch2->state = 0;
	ch2->next = new (block);
	ch2->next->next = NULL;

	co[1]->chct = ch1;
	co[2]->chct = ch2;
	co[3]->chct = ch2;

	d[0]->coct = co[1];
	d[1]->coct = co[1];
	d[2]->coct = co[2];
	d[3]->coct = co[3];

	DeviceControl('d', 'm', 'i');
	DeviceControl('d', 't', 'i');
}

//�����豸
int add(char name, char type) {
	int i;
	char a;
	char b;
	cout << "�豸���ƣ�" << endl;
	a = name;
	cout << "�豸���ͣ�" << endl;
	b = type;
	for (i = 0; i < e; i++)
		if (d[i]->sdt->name == a)
			cout << "�豸�Ѵ���!" << endl;
	if (i == e) {
		s[e] = new (sdt);
		d[e] = new (dct);
		s[e]->name = a;
		s[e]->type = b;
		d[e]->sdt = s[e];
		d[e]->state = 0;
		d[e]->next = new (block);
		d[e]->next->next = NULL;
		e++;
		cout << "�Ƿ��½�������?(y/n)" << endl;
		a = 'y';
		if (a == 'y') {
			char g;
			int flag = 0;
			cout << "��������������������:" << endl;
			g = controller++;
			if (flag == 0) {
				for (int k = 1; k <= c; k++)
					if (co[k]->name == g) {
						flag = 1;
						cout << "�ÿ������Ѵ���,����������!" << endl;
						cin >> g;
					}
			}
			co[c + 1] = new (coct);
			co[c + 1]->name = g;
			co[c + 1]->state = 0;
			co[c + 1]->next = new (block);
			co[c + 1]->next->next = NULL;
			d[e - 1]->coct = co[c + 1];
			c++;
			cout << "��ѡ��ͨ��(1/2)" << endl;
			char f;
			f = '1';
			if (f == '1')
				co[c]->chct = ch1;
			if (f == '2')
				co[c]->chct = ch2;

			cout << "�豸��ӳɹ���" << endl;
			return 1;
		}
		else if (a == 'n') {
			cout << "��ǰ�Ѵ��ڵĿ�������:" << endl;
			for (int i = 1; i <= c; i++)
				cout << co[i]->name << endl;
			cout << "����ѡ��Ŀ���������:" << endl;
			char cz;
			cin >> cz;
			for (int j = 1; j <= c; j++) {
				if (cz == co[j]->name) {
					cout << "�豸��ӳɹ�!" << endl;
					d[e - 1]->coct = co[j];
				}
			}
		}
	}
}

//ɾ���豸
int del(char name) {
	int b;
	block* p;
	cout << "�豸����:" << endl;
	char c;
	c = name;
	for (int i = 0; i < e; i++) {
		if (d[i]->sdt->name == c) {
			if (d[i]->state == 1) {
				cout << "�û���������ʹ���豸,�޷�ɾ��!" << endl;
				return -1;
			}

			else if (d[i]->next->next != NULL) //�豸�������в���
			{
				p = d[i]->next->next;
				for (; p->ename != c && p->next != NULL;)
					p = p->next;
				cout << "�û�����" << p->pid << "�ȴ�ʹ���豸,�޷�ɾ��!" << endl;
				return -1;
			}
			else if (d[i]->coct->next->next != NULL) //�������������в���
			{
				p = d[i]->coct->next->next;
				for (; p->ename != c && p->next != NULL;)
					p = p->next;
				cout << "�û�����" << p->pid << "�ȴ�ʹ���豸,�޷�ɾ��!" << endl;
				return -1;
			}
			else if (d[i]->coct->chct->next->next != NULL) //ͨ���������в���
			{
				p = d[i]->coct->chct->next->next;
				for (; p->ename != c && p->next != NULL;)
					p = p->next;
				cout << "�û�����" << p->pid << "�ȴ�ʹ���豸,�޷�ɾ��!" << endl;
				return -1;
			}
			else {
				cout << "�豸ɾ���ɹ�!" << endl;

				char coc = d[i]->coct->name;
				for (int j = i; j < e; j++)
					d[j] = d[j + 1];
				e--;
				cout << "ɾ��������(y/n)?" << endl;
				char a;
				a = 'n';
				if (a == 'y') {
					for (b = 0; b < e; b++)
						if (d[b]->coct->name == coc)
							cout << "������" << coc << "����ʹ��,�޷�ɾ��!" << endl;
					if (b == e - 1) {
						int j = 0;
						while (co[j]->name != coc)
							j++;
						for (j; j <= c; j++)
							co[j] = co[j + 1];
						c--;
						cout << "������ɾ���ɹ�!" << endl;
					}
					break;
				}
				else if (a == 'n')
					return 1;
				break;
			}
		}
	}
}

//�����豸
int require(char name, int pid) {
	block* p;
	char ename;
	cout << "�豸����:" << endl;
	ename = name;
	cout << "��������:" << endl;
	//pname = process;
	int fff = 0;
	for (int i = 0; i < e; i++) {
		if (d[i]->sdt->name == ename && d[i]->state == 0 && d[i]->coct->state == 0) {
			d[i]->state = 1;
			d[i]->pid = pid;
			d[i]->coct->state = 1;
			d[i]->coct->chct->state = 1;
			cout << "����ɹ�!" << endl;
			fff = 1;
			return 1;
		}
		else if (d[i]->sdt->name == ename && d[i]->state == 1) {
			cout << "�豸" << d[i]->sdt->name << "��ռ��,��������!" << endl;
			b = new (block);
			b->pid = pid;
			b->ename = ename;
			p = d[i]->next;
			while (p->next != NULL)
				p = p->next;
			p->next = b;
			b->next = NULL;
			fff = 1;
			return -1;
		}
		else if (d[i]->sdt->name == ename && d[i]->state == 0 && d[i]->coct->state == 1) {
			cout << "������" << d[i]->coct->name << "��ռ��,��������!" << endl;
			b = new (block);
			b->pid = pid;
			b->ename = ename;
			p = d[i]->coct->next;
			while (p->next != NULL)
				p = p->next;
			p->next = b;
			b->next = NULL;
			fff = 1;
			return -1;
		}
		else if (d[i]->sdt->name == ename && d[i]->state == 0 && d[i]->coct->state == 0 && d[i]->coct->chct->state == 1) {
			cout << "ͨ��" << d[i]->coct->chct->name << "��ռ��,��������!" << endl;
			b = new (block);
			b->pid = pid;
			b->ename = ename;
			p = d[i]->coct->chct->next;
			while (p->next != NULL)
				p = p->next;
			p->next = b;
			b->next = NULL;
			fff = 1;
			return -1;
		}
	}
	if (fff == 0) {
		cout << "�����豸�����ڣ�\n";
		return -1;
	}
}

int query(char name) {
	char ename;
	ename = name;
	//pname = process;
	int fff = 0;
	for (int i = 0; i < e; i++) {
		if (d[i]->sdt->name == ename && d[i]->state == 0 && d[i]->coct->state == 0) {
			return 1;
		}
		else if (d[i]->sdt->name == ename && d[i]->state == 1) {
			return -1;
		}
		else if (d[i]->sdt->name == ename && d[i]->state == 0 && d[i]->coct->state == 1) {
			return -1;
		}
		else if (d[i]->sdt->name == ename && d[i]->state == 0 && d[i]->coct->state == 0 && d[i]->coct->chct->state == 1) {
			return -1;
		}
	}
	if (fff == 0) {
		cout << "�����豸�����ڣ�\n";
		return -1;
	}
}

//�����豸
int callback(char name) {
	int i = 0, j;
	char n;
	int b;
	cout << "�豸����:" << endl;
	n = name;
	for (int i = 0; i < e; i++) {
		if (d[i]->sdt->name == n) {
			if (d[i]->state == 1) {
				cout << "�豸" << d[i]->sdt->name << "���ճɹ�!" << endl;
				d[i]->coct->state = 0;
				d[i]->coct->chct->state = 0;
				d[i]->state = 0;
				return 1;
				// if (d[i]->coct->chct->next->next != NULL)
				// {
				//     cout << "����" << d[i]->coct->chct->next->next->pname << "�ѳɹ�ʹ��" << d[i]->coct->chct->next->next->ename << "�豸!" << endl;
				//     for (b = 0; b < e; b++)
				//     {
				//         if (d[i]->coct->chct->next->next->ename == d[b]->sdt->name)
				//         {
				//             d[b]->state = 1;
				//             break;
				//         }
				//     }
				//     d[b]->coct->state = 1;
				//     d[i]->coct->chct->state = 1;
				//     block *p;
				//     p = d[i]->coct->chct->next;
				//     while (p->next->pname != d[i]->coct->chct->next->next->pname)
				//         p = p->next;
				//     p->next = p->next->next;
				//     break;
				// }
				// if (d[i]->coct->next->next != NULL)
				// {

				//     cout << "����" << d[i]->coct->next->next->pname << "�ѳɹ�ʹ��" << d[i]->coct->next->next->ename << "�豸!" << endl;
				//     for (b = 0; b < e; b++)
				//     {
				//         if (d[i]->coct->next->next->ename == d[b]->sdt->name)
				//             d[b]->state = 1;
				//     }
				//     d[i]->coct->state = 1;
				//     d[i]->coct->chct->state = 1;
				//     block *q;
				//     q = d[i]->coct->next;
				//     while (q->next->pname != d[i]->coct->next->next->pname)
				//         q = q->next;
				//     q->next = q->next->next;
				//     break;
				// }
				// if (d[i]->next->next != NULL)
				// {
				//     cout << "����" << d[i]->next->next->pname << "�ѳɹ�ʹ��" << d[i]->next->next->ename << "�豸!" << endl;
				//     for (int b = 0; b < e; b++)
				//     {
				//         if (d[i]->next->next->ename == d[b]->sdt->name)
				//             d[b]->state = 1;
				//     }

				//     d[i]->coct->state = 1;
				//     d[i]->coct->chct->state = 1;
				//     block *r;
				//     r = d[i]->next;
				//     while (r->next->pname != d[i]->next->next->pname)
				//         r = r->next;
				//     r->next = r->next->next;
				//     break;
				// }
				// else
				// {
				//     for (int j = 0; j < e; j++)
				//     {
				//         if (d[j]->coct->chct->next->next != NULL)
				//         {
				//             cout << "����" << d[j]->coct->chct->next->next->pname << "�ѳɹ�ʹ��" << d[j]->coct->chct->next->next->ename << "�豸!" << endl;
				//             d[j]->coct->state = 1;
				//             d[j]->coct->chct->state = 1;
				//             block *p;
				//             p = d[j]->coct->chct->next;
				//             while (p->next->pname != d[j]->coct->chct->next->pname)
				//                 p = p->next;
				//             p->next = p->next->next;
				//             break;
				//         }
				//         if (d[j]->coct->next->next != NULL)
				//         {
				//             cout << "����" << d[j]->coct->next->next->pname << "�ѳɹ�ʹ��" << d[j]->coct->next->next->ename << "�豸!" << endl;
				//             d[j]->state = 1;
				//             d[j]->coct->state = 1;
				//             d[j]->coct->chct->state = 1;
				//             block *q;
				//             q = d[j]->coct->next;
				//             while (q->next->pname != d[j]->coct->next->next->pname)
				//                 q = q->next;
				//             q->next = q->next->next;
				//             break;
				//         }
				//         if (d[j]->next->next != NULL)
				//         {
				//             cout << "����" << d[j]->next->next->pname << "�ѳɹ�ʹ��" << d[j]->next->next->ename << "�豸!" << endl;
				//             d[j]->state = 1;
				//             d[j]->coct->state = 1;
				//             d[j]->coct->chct->state = 1;
				//             block *r;
				//             r = d[j]->next;
				//             while (r->next->pname != d[j]->next->next->pname)
				//                 r = r->next;
				//             r->next = r->next->next;
				//             break;
				//         }
				//     }
				//     if (j == e)
				//         cout << "����������" << endl;
				//     break;
				// }
			}
			else {
				cout << "�豸����,�������!" << endl;
				return 0;
				break;
			}
		}
	}
	if (i == e)
		cout << "�����ڸ��豸!" << endl;
	return 0;
}

//���˵���ӡ
void menu() {
	cout << "--------------------�豸����---------------------" << endl;
	cout << "                 a  ����豸" << endl;
	cout << "                 d  ɾ���豸" << endl;
	cout << "                 r  �����豸" << endl;
	cout << "                 c  �����豸" << endl;
	cout << "                 s  �豸״̬�鿴" << endl;
	cout << "                 o  �˳�����" << endl;
	cout << "-------------------------------------------------" << endl;
}

//�豸״̬�鿴
void play() {
	cout << "    "
		<< "�豸����"
		<< "   "
		<< "����"
		<< "    "
		<< "������"
		<< "    "
		<< "ͨ��" << endl;
	for (int i = 0; i < e; i++)
		cout << "       " << d[i]->sdt->name << "        " << d[i]->sdt->type << "        " << d[i]->coct->name << "       " << d[i]->coct->chct->name << endl;
}

//c����������ͣ�
//a:����豸 d:ɾ���豸 r:����ʹ���豸 c:����ʹ���豸 s:�鿴Ŀǰʹ���豸״�� q:��ѯռ��
int DeviceControl(char c, char name, char type, int pid) {
	int fanhuizhi = 1;

	switch (c) {
	case 'a':
		fanhuizhi = add(name, type);
		break;
	case 'd':
		fanhuizhi = del(name);
		break;
	case 'r':
		fanhuizhi = require(name, pid);
		break;
	case 'c':
		fanhuizhi = callback(name);
		break;
	case 's':
		play();
		break;
	case 'q':
		fanhuizhi = query(name);
		break;
	default:
		cout << "�������! ";
		return -1;
		break;
	}
	return fanhuizhi;
}

// //������
// int main()
// {
//     init();
//     DeviceControl('d', 'k', 'z', 's');
//     DeviceControl('d', 'm', 'z', 's');
//     DeviceControl('d', 't', 'z', 's');
//     DeviceControl('d', 'p', 'z', 's');
//     while (1)
//     {
//         menu();
//         cout << "��ѡ��:" << endl;
//         char c, z, v, p;
//         cin >> c;
//         cout << "�������豸����:" << endl;
//         cin >> z;
//         cout << "�������豸����:" << endl;
//         cin >> v;
//         cout << "������ʹ�ý���:" << endl;
//         cin >> p;
//         DeviceControl(c, z, v, p);
//         devicePrint();
//     }
// }

void devicePrint() {
	ofstream outfile("./OS_UI/commute/device.txt", ios::trunc);
	for (int i = 0; i < e; i++) {
		string str = "";
		str = str + d[i]->sdt->name + ' ';
		if (d[i]->state == 1) {
			str = str + "Busy ";
			str = str + to_string(d[i]->pid);
		}
		else {
			str = str + "Free NOP";
		}
		outfile << str << endl;
	}
	outfile.close();
}