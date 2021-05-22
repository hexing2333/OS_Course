#pragma once
#include<vector>
#include "FCB.h"
#include "DB.h"
const int STORAGE_SIZE = 128;

class FileSystem
{
public:
	FileSystem();
	~FileSystem();//new 了很多树节点

	void test();
	int create(string path, string name, int type,int size); //在path下创建文件或文件夹name，并分配数据块
	int del(string path);							//删除path及以下的所有节点，并收回数据块空间
	int rename(string path,string oldname, string newname);	//重命名文件或文件夹
	vector<string> search(string name);				//搜索文件或文件夹,返回所有名为name的文件或文件夹的路径

	string read(string path);						//返回路径为path的文件内容
	int show(string path);							//显示path下的文件filename的内容
	int edit(string path,string input);				//编辑path下的文件的内容,把input写到路径为path的文件里

	int swapToExternalStorage(int pid, int size);	//中期调度时把进程换到外存上,进程pid申请size块空间
	int swapToMemory(int pid, int size);			//中期调度时把进程换回内存上,进程pid释放size块空间

	int printDir();									//打印目录
	int sendExternalStorageStatusToUI();			//把当前外存状态给UI
	void printFreeSpaceList();						//打印空闲空间列表
	void printUtiliRate();							//打印外存使用率
	TreeNode*& matchPath(string path);				//路径匹配,在树 dirTree 上找一条path，返回末尾的节点
	vector<string> printAllFilesPath();				//返回并打印所有文件的路径

	
private:
	Tree dirTree;				//目录树
	DB storage[STORAGE_SIZE];	//外存空间
	vector<int> freeSpaceList;	//空闲空间列表
	string swapAreaPath;		//交换区路径
	float utiliRate;			//外存占用率

	vector<string> split(const string& str, const string& delim);		//字符串切片
	void postOrderDelSubTree(Tree& t);									//后序删除子树
	string getFullPath(TreeNode* t);									//获取文件节点 t 的完整路径
	void preOrderTraverseFormAllPath(const Tree& t, vector<string>& s); //前序遍历找到所有文件的路径
	void preOrderTraverse(const Tree& t, int depth, vector<string>& s); //前序遍历子树
	void preOrderSearch(TreeNode* t, string name, vector<TreeNode*>& resultList);//前序遍历搜索指定文件
	int applyForExternalStorage(FCB& fcb);								//给一个文件或文件夹分配数据块
	int releaseExternalStorage(FCB& fcb);								//给一个文件或文件夹释放数据块
	int checkDupName(TreeNode* t, string name);							//检查t的第一层子树是否存在叫name的文件或文件夹，返回1则有，返回0则没有
	TreeNode* getATreeNode( string path, string name);					//返回path下的name文件或文件夹
};

