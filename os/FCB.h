#pragma once
#include<string>
#include<vector>
using namespace std;

class FCB
{
public:
	FCB(char newtype, string newname, int newsize, FCB* newparent, FCB* newchild, FCB* newsibling);

	char type;//1Ϊ�ļ�j�У�2Ϊ�ļ�
	string name;
	FCB* parent;
	FCB* child;
	FCB* sibling;
	int size;//���ݿ�ռ�ü���block���ļ��е�size����0
	vector<int> location;//�ļ��Ĵ洢λ��
	//int accessRight;//�ɶ���д֮���
	//int ownerId;
};

typedef FCB TreeNode;
typedef FCB* Tree;



