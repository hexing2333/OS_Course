#pragma once
#include<vector>
#include "FCB.h"
#include "DB.h"
const int STORAGE_SIZE = 128;

class FileSystem
{
public:
	FileSystem();
	~FileSystem();//new �˺ܶ����ڵ�

	void test();
	int create(string path, string name, int type,int size); //��path�´����ļ����ļ���name�����������ݿ�
	int del(string path);							//ɾ��path�����µ����нڵ㣬���ջ����ݿ�ռ�
	int rename(string path,string oldname, string newname);	//�������ļ����ļ���
	vector<string> search(string name);				//�����ļ����ļ���,����������Ϊname���ļ����ļ��е�·��

	string read(string path);						//����·��Ϊpath���ļ�����
	int show(string path);							//��ʾpath�µ��ļ�filename������
	int edit(string path,string input);				//�༭path�µ��ļ�������,��inputд��·��Ϊpath���ļ���

	int swapToExternalStorage(int pid, int size);	//���ڵ���ʱ�ѽ��̻��������,����pid����size��ռ�
	int swapToMemory(int pid, int size);			//���ڵ���ʱ�ѽ��̻����ڴ���,����pid�ͷ�size��ռ�

	int printDir();									//��ӡĿ¼
	int sendExternalStorageStatusToUI();			//�ѵ�ǰ���״̬��UI
	void printFreeSpaceList();						//��ӡ���пռ��б�
	void printUtiliRate();							//��ӡ���ʹ����
	TreeNode*& matchPath(string path);				//·��ƥ��,���� dirTree ����һ��path������ĩβ�Ľڵ�
	vector<string> printAllFilesPath();				//���ز���ӡ�����ļ���·��

	
private:
	Tree dirTree;				//Ŀ¼��
	DB storage[STORAGE_SIZE];	//���ռ�
	vector<int> freeSpaceList;	//���пռ��б�
	string swapAreaPath;		//������·��
	float utiliRate;			//���ռ����

	vector<string> split(const string& str, const string& delim);		//�ַ�����Ƭ
	void postOrderDelSubTree(Tree& t);									//����ɾ������
	string getFullPath(TreeNode* t);									//��ȡ�ļ��ڵ� t ������·��
	void preOrderTraverseFormAllPath(const Tree& t, vector<string>& s); //ǰ������ҵ������ļ���·��
	void preOrderTraverse(const Tree& t, int depth, vector<string>& s); //ǰ���������
	void preOrderSearch(TreeNode* t, string name, vector<TreeNode*>& resultList);//ǰ���������ָ���ļ�
	int applyForExternalStorage(FCB& fcb);								//��һ���ļ����ļ��з������ݿ�
	int releaseExternalStorage(FCB& fcb);								//��һ���ļ����ļ����ͷ����ݿ�
	int checkDupName(TreeNode* t, string name);							//���t�ĵ�һ�������Ƿ���ڽ�name���ļ����ļ��У�����1���У�����0��û��
	TreeNode* getATreeNode( string path, string name);					//����path�µ�name�ļ����ļ���
};

