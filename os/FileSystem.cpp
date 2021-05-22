#include <iostream>
#include<fstream>
#include<vector>
#include<stack>
#include "FileSystem.h"
#include "FCB.h"

FileSystem::FileSystem()
{
    utiliRate = 0;
    //空闲空间列表一开始全部空闲
    for (int i = 0; i < STORAGE_SIZE; i++)
    {
        freeSpaceList.push_back(i);
    }

    dirTree = new TreeNode(1, "root", 0, nullptr, nullptr, nullptr);//创建一个叫root的文件夹
    swapAreaPath = "root/temp";
    string temp = swapAreaPath.substr(5);


    cout << "**********************创建一些初始文件和文件夹********************" << endl;
    create("root", temp, 1, 0);  //temp是用于存在中期调度进程的文件夹

    create("root", "personal", 1, 0);
    create("root", "code", 1, 0);

    create("root/code", "1.txt", 2, 4);
    edit("root/code/1.txt", "M 2\nY 3\nC 3\nK 10\nC 2\nC 5\nC 3\nQ\n");

    create("root/code", "2.txt", 2, 4);
    edit("root/code/2.txt", "M 2\nY 2\nK 5\nC 10\nK 5\nC 2\nQ\n");

    create("root/code", "3.txt", 2, 4);
    edit("root/code/3.txt", "M 8\nY 3\nK 4\nC 3\nC 5\nK 6\nQ\n");

    create("root/code", "4.txt", 2, 4);
    edit("root/code/4.txt", "M 9\nY 3\n\nK 4\nC 3\nC 5\nK 6\nQ");

    create("root/personal", "hw", 1, 0);
    create("root/personal/hw", "hw1", 1, 0);
    create("root/personal/hw", "hw2", 1, 0);
    create("root/personal/hw", "hw3", 1, 0);
    create("root/personal/hw/hw1", "lalala", 2, 10);
    create("root/personal/hw/hw3", "lalala", 2, 5);
    create("root/personal/hw/hw1", "yeyeye", 2, 3);

    // show("root/code/1.txt");
    cout << "****************************************************************" << endl;
}

FileSystem::~FileSystem()
{
    postOrderDelSubTree(dirTree);
}

void FileSystem::test()
{
    //打印初始文件目录和空闲空间列表
    printDir();
    printFreeSpaceList();

    cout << "****************创建文件测试***************" << endl;
    create("root/personal/hw", "hw2", 2, 5);  //测试用，证明不能在同一文件夹下创建同名文件或文件夹
    create("root/personal/hw/hw3", "lololo", 2, 1000);//测试用，证明外存不够时创建文件失败
    create("root/personal/hw/hw1/yeyeye", "dupname", 2, 3); //测试用，证明不能在文件下创建文件
    create("root/zhangwangting", "notfound", 2, 9);      //测试用，证明不能在不存在的路径下创建文件

    cout << "\n****** **打印系统中所有文件的路径测试**********" << endl;
    printAllFilesPath();

    cout << "\n****************搜索测试********************" << endl;
    search("lalala");
    search("yeyeye");
    search("yyy");

    cout << "\n****************重命名测试******************" << endl;
    rename("root/personal/hw/hw3", "lalala", "lololo");
    rename("root/personal/hw/hw3", "lalala", "yeyeye");
    rename("root/personal/hw/hw3", "yeyeye", "yeyeye");
    printDir();


    cout << "\n**************删除文件夹测试****************" << endl;
    del("root/personal/hw/hw1");
    printDir();
    del("root/personal/hw");
    del("root/person");
    printDir();
    printFreeSpaceList();
    search("lalala");

    swapToExternalStorage(123, 70);
    printUtiliRate();

    cout << "\n************和内存间的交换测试***************" << endl;
    swapToExternalStorage(123, 3);
    printDir();
    printFreeSpaceList();
    swapToMemory(123, 3);
    printDir();
    printFreeSpaceList();

    cout << "\n***************文件读写功能测试****************" << endl;
    show("root/code/1.txt");
    edit("root/code/1.txt", "我修改完啦！^_^");
    read("root/code/1.txt");
}


void FileSystem::postOrderDelSubTree(Tree &t)//后序遍历删除文件子树
{
    if (t == nullptr)
        return;

    postOrderDelSubTree(t->child);
    postOrderDelSubTree(t->sibling);
    //先删除数据块
    releaseExternalStorage(*t);
    //再删除目录项
    delete t;
    t = nullptr;
}


vector<string> FileSystem::printAllFilesPath()
{
    vector<string> s;
    preOrderTraverseFormAllPath(dirTree, s);
    for (int i = 0; i < s.size(); i++)
    {
        cout << s[i] << endl;
    }
    return s;
}
// 获取文件节点 t 的完整路径
//		参数：	t：文件节点
//		返回：	指定文件节点的完整路径字符串（文件夹以 \ 结尾）
string FileSystem::getFullPath(TreeNode* t)
{
    string s;
    if (t->type == 1)	s = "/";

    do
    {
        s = "/" + t->name + s;
    } while (t = t->parent);

    return s;
}

// 遍历指定文件夹
//		参数：	t：文件节点
//				s：将要用来保存遍历结果的 vector 对象
//		返回：	None
void FileSystem::preOrderTraverseFormAllPath(const Tree& t, vector<string>& s)
{
    string tmp = getFullPath(t);							// 获取指定文件节点的完整路径。以 \ 结尾的表示文件夹
    if (tmp[tmp.length() - 1] != '/')
    {
        s.push_back(tmp);	// 如果指定节点是文件夹，不计入遍历结果
    }

    if (t->child != nullptr)								// 遍历孩子
        preOrderTraverseFormAllPath(t->child, s);

    if (t->sibling != nullptr)								// 遍历兄弟
        preOrderTraverseFormAllPath(t->sibling, s);
}


void FileSystem::preOrderTraverse(const Tree& t,int depth,vector<string> &s)//先序遍历文件目录树
{
    if (t != nullptr)
    {
        
        string temp;
        for (int i = 0; i < depth; i++)
        {
            if (i == depth - 1)
            {
                temp = temp + "|--";
            }
            else
            {
                temp = temp + "|    ";
            }
            
           
        }
        string seq=" ";
        for (int i = 0; i < t->size; i++)
        {
            seq = seq +" "+ to_string(t->location[i]);
        }
        s.push_back(temp + t->name);
        //s.push_back(temp + t->name + seq);
        preOrderTraverse(t->child,depth+1,s);
        preOrderTraverse(t->sibling,depth,s);
    }
}

string FileSystem::read(string path)
{
    FCB* fcb = matchPath(path);
    vector<int> locations = fcb->location;
    string result;
    for (int i = 0; i < locations.size(); i++)
    {
        //printf("%s", (char*)&storage[locations[i]]);
        result = result + string((char*)&storage[locations[i]]);
    }
    cout << result;
    return result;
}

int FileSystem::show(string path)
{
    FCB* fcb = matchPath(path);
    vector<int> locations = fcb->location;
    for (int i = 0; i < locations.size(); i++)
    {
        printf("%s", (char*)&storage[locations[i]]);
    }
    return 0;
}

int FileSystem::edit(string path,string input)
{
    FCB* fcb = matchPath(path);
    vector<int> locations = fcb->location;

    //先清空
    for (int i = 0; i < locations.size(); i++)
    {
        //cout << sizeof(storage[i]);
        memset(&storage[locations[i]], 0, DB_SIZE);
    }
    
    double size = input.size();
    int blocks = ceil(size /( DB_SIZE-1));
    if (blocks > fcb->size)
    {
        cout << "输入文件过长！" << endl;
        return 1;
    }
    for (int i = 0; i < blocks; i++)
    {
        if (i == blocks - 1)//到最后一块了
        {
            
           // printf("@@@@@@@@%s", input.substr(0, DB_SIZE-1).c_str());
            strcpy_s((char *)&storage[locations[i]], strlen(input.substr(0).c_str())+1,input.substr(0).c_str());
        }
        else
        {
           // printf("@@@@@@@@@%s", input.substr(0, DB_SIZE-1).c_str());
            strcpy_s((char *)&storage[locations[i]], strlen(input.substr(0, DB_SIZE-1).c_str())+1,input.substr(0, DB_SIZE-1).c_str());//要减一的原因是得留一个位置给'\0'
            input.erase(0, DB_SIZE);
        }
    }
    return 0;
}

int FileSystem::swapToExternalStorage(int pid, int size)
{
    int ans=create("root/temp", "pid" + to_string(pid), 2,size);//在文件树root\temp下创建一个叫pidxxx的文件
    if (ans == 0)
    {
        cout << "成功将进程" << pid << "的" << size << "块换到外存上！" << endl;
    }
    else
    {
        cout<<"没能将进程" << pid << "的" << size << "块换到外存上！" << endl;
    }
    return ans;
}

//图图、铭哥你们在调这个函数之前得保证有size块大小的空闲内存噢^_^
int FileSystem::swapToMemory(int pid, int size)
{
   //删除相应目录节点，释放相应外存
    string path = swapAreaPath+"/pid"+to_string(pid);
    int ans=del(path);
    return ans;
}

int FileSystem::printDir()//打印文件目录，即深搜打印树,并输出到文件tree.txt
{
    cout << "-------------------------------------------------------------------------------------------" << endl;
    cout << "文件目录如下：" << endl;
   vector<string> s;
    preOrderTraverse(dirTree,0,s);
    for (int i = 0; i < s.size(); i++)
    {
        cout << s[i] << endl;
    }
    cout << "-------------------------------------------------------------------------------------------" << endl;

    //输出到文件
    ofstream outfile("./OS_UI/commute/tree.txt", ios::out);

    if (!outfile)
    {
        cerr << "Failed to open the file!";
        return 1;
    }


    for (int i = 0; i < s.size(); i++)
    {
        outfile << s[i] + '\n';
    }
    outfile.close();
    return 0;
    
}

int FileSystem::sendExternalStorageStatusToUI()
{
    //-1为空
    string storageStatus[STORAGE_SIZE];
    for (int i = 0; i < STORAGE_SIZE; i++)
    {
        storageStatus[i] = to_string(i) + " 1";
    }
    for (int i = 0; i < freeSpaceList.size(); i++)
    {
        storageStatus[freeSpaceList[i]] = to_string(freeSpaceList[i]) + " -1";
    }

    //输出到文件
    ofstream outfile("./OS_UI/commute/storageStatus.txt", ios::out);

    if (!outfile)
    {
        cerr << "Failed to open the file!";
        return 1;
    }

    outfile << to_string(STORAGE_SIZE) << " " << utiliRate << "%" << endl;
    for (int i = 0; i < STORAGE_SIZE; i++)
    {
        outfile << storageStatus[i] + '\n';
    }
    outfile.close();
    return 0;

}

int FileSystem::del(string path)
{
    TreeNode*& pnode = matchPath(path);

    if (pnode == nullptr)
    {
        cout << "删除节点失败，该路径不存在" << endl;
        return 1;
    }
    else
    {
        postOrderDelSubTree(pnode);
        cout << "删除子树成功！" << endl;
        return 0;
    }
}

void FileSystem::preOrderSearch(TreeNode *t,string name,vector<TreeNode*> &resultList)
{
    if (t == nullptr)
    {
        return;
    }
    if (t->name == name)
    {
        resultList.push_back(t);
    }
    
    preOrderSearch(t->child, name,resultList);
    preOrderSearch(t->sibling, name, resultList);
}

int FileSystem::rename(string path, string oldname, string newname)
{
    TreeNode* aim = getATreeNode(path, oldname);

    if (aim == nullptr)
    {
        cout << "该path不存在或该path下不存在名为" << oldname << "的文件或文件夹!" << endl;
        cout << "重命名失败！" << endl;
        return 1;
    }
    else
    {
        int retu = checkDupName(aim->parent, newname);
            if (retu == 0)//没有重名文件
            {
                aim->name = newname;
                cout << "重命名成功！" << endl;
              
            }
            else
            {
                cout << "该path下存在重名文件。\n重命名失败！" << endl;
                
            }
            return retu;
    }
    
}

vector<string> FileSystem::search(string name)
{
    vector<string> ans;
    vector<TreeNode*>l;
    preOrderSearch(dirTree, name, l);
    if (l.size() == 0)
    {
        cout << "名为" << name << "的文件或文件夹不存在！" << endl;
    }
    else
    {

        cout << "以下为所有名为" << name << "的文件或文件夹的路径:" << endl;
        for (int i = 0; i < l.size(); i++)
        {
            TreeNode* currentptr = l[i];
            string res;
            while (currentptr != dirTree)
            {
                res = currentptr->name + "/" + res;
                currentptr = currentptr->parent;
            }
            res = currentptr->name + "/" + res;
            res.pop_back();
            ans.push_back(res);
            cout << res << endl;
        }
    }
    return ans;
}


TreeNode*& FileSystem::matchPath(string path)//在树 dirTree 上找一条path，返回末尾的节点
{
    vector<string> res = split(path, "/");
    TreeNode* temp = dirTree;
    TreeNode* lastptr = nullptr;

    int i;
    for (i = 0; i < res.size(); i++)
    {
        while (temp != nullptr)
        {
            if (temp->name == res[i] /*&& temp->type == 1*/)//在当前层找到了
            {
                if (i < res.size() - 1)
                {
                    lastptr = temp;
                    temp = temp->child;//进入下一层的匹配
                }
                break;
            }
            else
            {
                lastptr = temp;
                temp = temp->sibling;
            }
        }
        if (temp == nullptr)
            break;
    }

    if (i == res.size())
    {
        cout << "路径匹配成功！" << endl;

        if (lastptr == nullptr)
            return dirTree;
        else if (lastptr->child == temp)
            return lastptr->child;
        else
            return lastptr->sibling;
    }
    else
    {
        cout << "待查找的路径不存在" << endl;
        return temp;
    }
}

int FileSystem::create(string path,  string name,  int type, int size)//在path下创建文件或文件夹name,并为之分配size块数据块空间
{
    //找到创建目录节点的位置
    TreeNode * pnode=matchPath(path);
    
    if (pnode == nullptr)
    {
        cout << "当前目录不存在，创建新节点失败！\n" << endl;
        return 1;
    }
    if (pnode->type == 2)
    {
        cout << "不能在非目录下创建节点，创建新节点失败！" << endl;
        return 1;
    }
    TreeNode** aim = nullptr;
    if (pnode->child == nullptr)//该目录下还没有东西时
    {

       // pnode->child = new TreeNode(type, name, size, pnode, nullptr, nullptr);
        aim = &pnode->child;
       // applyForExternalStorage(*pnode->child);
    }
    else
    {
        TreeNode* currentptr = pnode->child;
        TreeNode* lastptr = nullptr;
        while (currentptr!=nullptr)
        {
            if (currentptr->name == name)
            {
                cout << "当前目录下存在同名文件/文件夹，创建新节点失败！\n" << endl;
                return 1;
            }
            lastptr = currentptr;
            currentptr = currentptr->sibling;
        }
        aim = &lastptr->sibling;
       /* lastptr->sibling= new TreeNode(type, name, size, pnode, nullptr, nullptr);
        applyForExternalStorage(*lastptr->sibling);*/
    }

    //创建目录节点并申请外存
    *aim= new TreeNode(type, name, size, pnode, nullptr, nullptr);
    int ans = applyForExternalStorage(**aim);
    if (ans == 0)
    {
        
        cout << "新节点创建成功！" << endl;
    }
    else
    {
        cout << "新节点创建失败！" << endl;
        delete *aim;
        *aim = nullptr;
    }
	return ans;
}

int FileSystem::applyForExternalStorage(FCB &fcb)
{
    int size = fcb.size;
    if (size < freeSpaceList.size())
    {
        fcb.location.insert(fcb.location.end(), freeSpaceList.begin(), freeSpaceList.begin() + size);
        freeSpaceList.erase(freeSpaceList.begin(), freeSpaceList.begin() + size);
        cout << "申请外存空间成功!" << endl;
       /* printFreeSpaceList();
        cout << "外存空闲空间大小为" << freeSpaceList.size() << endl;*/
        int size=freeSpaceList.size();
        utiliRate = 100*(STORAGE_SIZE - freeSpaceList.size()) / STORAGE_SIZE;
        return 0;
    }
    else
    {
        fcb.size = 0;
        cout << "申请外存空间失败，外存空间不足！" << endl;
        return 1;
    }
  
}

int FileSystem::releaseExternalStorage(FCB& fcb)
{
    int size = fcb.size;

    freeSpaceList.insert(freeSpaceList.end(),fcb.location.begin(),fcb.location.end());
  
    fcb.location.erase(fcb.location.begin(), fcb.location.end());
    utiliRate = 100*(STORAGE_SIZE - freeSpaceList.size()) / STORAGE_SIZE;
    return 0;
}

int FileSystem::checkDupName(TreeNode* t, string name)
{
    TreeNode* currentptr = t->child;
    while (currentptr!=NULL)
    {
        if (currentptr->name == name)//有重名文件
        {
            return 1;
        }
        currentptr = currentptr->sibling;
    }
     return 0;
}

TreeNode* FileSystem::getATreeNode(string path, string name)
{
    TreeNode* pnode= matchPath(path);
    if (pnode != nullptr)
    {
        TreeNode* currentptr = pnode->child;
        while (currentptr != NULL)
        {
            if (currentptr->name == name)
            {
                return currentptr;
            }
            currentptr = currentptr->sibling;
        }
        return nullptr;
    }
    else
    {
        return nullptr;
    }
}

void FileSystem::printFreeSpaceList()
{
    cout << "\n空闲空间列表为:" << endl;
    for (int i = 0; i < freeSpaceList.size(); i++)
    {
        cout << freeSpaceList[i] << ' ';
    }
    cout << endl;
}

void FileSystem::printUtiliRate()
{

    cout<<"当前外存使用率为:" << utiliRate << endl;
}

vector<string> FileSystem::split(const string& str, const string& delim)//字符串切片
{
    vector<string> res;
    if ("" == str) return res;
    //先将要切割的字符串从string类型转换为char*类型  
    int len1 = str.length() + 1;//要加上一个\0
    char* strs = new char[len1]; 
    strcpy_s(strs,len1, str.c_str());

    int len2 = delim.length() + 1;
    char* d = new char[len2];
    strcpy_s(d, len2,delim.c_str());

    char* buf=NULL;
    char* p = strtok_s(strs, d,&buf);

    while (p)
    {
        string s = p; //分割得到的字符串转换为string类型  
        res.push_back(s); //存入结果数组  
        p = strtok_s(NULL, d,&buf);
    }

    delete [] strs;
    delete [] d;

    cout << "\n路径为：" << endl;
    for (int i = 0; i < res.size(); i++)
    {
        std::cout << res[i] << " ";
    }
    //cout << "得到的字段数：" << res.size() << endl;
    return res;
}
