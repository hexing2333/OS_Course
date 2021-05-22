#pragma once
#include<string>
#include<vector>
using namespace std;

class FCB
{
public:
	FCB(char newtype, string newname, int newsize, FCB* newparent, FCB* newchild, FCB* newsibling);

	char type;//1为文件j夹，2为文件
	string name;
	FCB* parent;
	FCB* child;
	FCB* sibling;
	int size;//数据块占用几个block，文件夹的size就是0
	vector<int> location;//文件的存储位置
	//int accessRight;//可读可写之类的
	//int ownerId;
};

typedef FCB TreeNode;
typedef FCB* Tree;



