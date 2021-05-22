#include <iostream>
#include<fstream>
#include<vector>
#include<stack>
#include "FileSystem.h"
#include "FCB.h"

FileSystem::FileSystem()
{
    utiliRate = 0;
    //���пռ��б�һ��ʼȫ������
    for (int i = 0; i < STORAGE_SIZE; i++)
    {
        freeSpaceList.push_back(i);
    }

    dirTree = new TreeNode(1, "root", 0, nullptr, nullptr, nullptr);//����һ����root���ļ���
    swapAreaPath = "root/temp";
    string temp = swapAreaPath.substr(5);


    cout << "**********************����һЩ��ʼ�ļ����ļ���********************" << endl;
    create("root", temp, 1, 0);  //temp�����ڴ������ڵ��Ƚ��̵��ļ���

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
    //��ӡ��ʼ�ļ�Ŀ¼�Ϳ��пռ��б�
    printDir();
    printFreeSpaceList();

    cout << "****************�����ļ�����***************" << endl;
    create("root/personal/hw", "hw2", 2, 5);  //�����ã�֤��������ͬһ�ļ����´���ͬ���ļ����ļ���
    create("root/personal/hw/hw3", "lololo", 2, 1000);//�����ã�֤����治��ʱ�����ļ�ʧ��
    create("root/personal/hw/hw1/yeyeye", "dupname", 2, 3); //�����ã�֤���������ļ��´����ļ�
    create("root/zhangwangting", "notfound", 2, 9);      //�����ã�֤�������ڲ����ڵ�·���´����ļ�

    cout << "\n****** **��ӡϵͳ�������ļ���·������**********" << endl;
    printAllFilesPath();

    cout << "\n****************��������********************" << endl;
    search("lalala");
    search("yeyeye");
    search("yyy");

    cout << "\n****************����������******************" << endl;
    rename("root/personal/hw/hw3", "lalala", "lololo");
    rename("root/personal/hw/hw3", "lalala", "yeyeye");
    rename("root/personal/hw/hw3", "yeyeye", "yeyeye");
    printDir();


    cout << "\n**************ɾ���ļ��в���****************" << endl;
    del("root/personal/hw/hw1");
    printDir();
    del("root/personal/hw");
    del("root/person");
    printDir();
    printFreeSpaceList();
    search("lalala");

    swapToExternalStorage(123, 70);
    printUtiliRate();

    cout << "\n************���ڴ��Ľ�������***************" << endl;
    swapToExternalStorage(123, 3);
    printDir();
    printFreeSpaceList();
    swapToMemory(123, 3);
    printDir();
    printFreeSpaceList();

    cout << "\n***************�ļ���д���ܲ���****************" << endl;
    show("root/code/1.txt");
    edit("root/code/1.txt", "���޸�������^_^");
    read("root/code/1.txt");
}


void FileSystem::postOrderDelSubTree(Tree &t)//�������ɾ���ļ�����
{
    if (t == nullptr)
        return;

    postOrderDelSubTree(t->child);
    postOrderDelSubTree(t->sibling);
    //��ɾ�����ݿ�
    releaseExternalStorage(*t);
    //��ɾ��Ŀ¼��
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
// ��ȡ�ļ��ڵ� t ������·��
//		������	t���ļ��ڵ�
//		���أ�	ָ���ļ��ڵ������·���ַ������ļ����� \ ��β��
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

// ����ָ���ļ���
//		������	t���ļ��ڵ�
//				s����Ҫ���������������� vector ����
//		���أ�	None
void FileSystem::preOrderTraverseFormAllPath(const Tree& t, vector<string>& s)
{
    string tmp = getFullPath(t);							// ��ȡָ���ļ��ڵ������·������ \ ��β�ı�ʾ�ļ���
    if (tmp[tmp.length() - 1] != '/')
    {
        s.push_back(tmp);	// ���ָ���ڵ����ļ��У�������������
    }

    if (t->child != nullptr)								// ��������
        preOrderTraverseFormAllPath(t->child, s);

    if (t->sibling != nullptr)								// �����ֵ�
        preOrderTraverseFormAllPath(t->sibling, s);
}


void FileSystem::preOrderTraverse(const Tree& t,int depth,vector<string> &s)//��������ļ�Ŀ¼��
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

    //�����
    for (int i = 0; i < locations.size(); i++)
    {
        //cout << sizeof(storage[i]);
        memset(&storage[locations[i]], 0, DB_SIZE);
    }
    
    double size = input.size();
    int blocks = ceil(size /( DB_SIZE-1));
    if (blocks > fcb->size)
    {
        cout << "�����ļ�������" << endl;
        return 1;
    }
    for (int i = 0; i < blocks; i++)
    {
        if (i == blocks - 1)//�����һ����
        {
            
           // printf("@@@@@@@@%s", input.substr(0, DB_SIZE-1).c_str());
            strcpy_s((char *)&storage[locations[i]], strlen(input.substr(0).c_str())+1,input.substr(0).c_str());
        }
        else
        {
           // printf("@@@@@@@@@%s", input.substr(0, DB_SIZE-1).c_str());
            strcpy_s((char *)&storage[locations[i]], strlen(input.substr(0, DB_SIZE-1).c_str())+1,input.substr(0, DB_SIZE-1).c_str());//Ҫ��һ��ԭ���ǵ���һ��λ�ø�'\0'
            input.erase(0, DB_SIZE);
        }
    }
    return 0;
}

int FileSystem::swapToExternalStorage(int pid, int size)
{
    int ans=create("root/temp", "pid" + to_string(pid), 2,size);//���ļ���root\temp�´���һ����pidxxx���ļ�
    if (ans == 0)
    {
        cout << "�ɹ�������" << pid << "��" << size << "�黻������ϣ�" << endl;
    }
    else
    {
        cout<<"û�ܽ�����" << pid << "��" << size << "�黻������ϣ�" << endl;
    }
    return ans;
}

//ͼͼ�����������ڵ��������֮ǰ�ñ�֤��size���С�Ŀ����ڴ���^_^
int FileSystem::swapToMemory(int pid, int size)
{
   //ɾ����ӦĿ¼�ڵ㣬�ͷ���Ӧ���
    string path = swapAreaPath+"/pid"+to_string(pid);
    int ans=del(path);
    return ans;
}

int FileSystem::printDir()//��ӡ�ļ�Ŀ¼�������Ѵ�ӡ��,��������ļ�tree.txt
{
    cout << "-------------------------------------------------------------------------------------------" << endl;
    cout << "�ļ�Ŀ¼���£�" << endl;
   vector<string> s;
    preOrderTraverse(dirTree,0,s);
    for (int i = 0; i < s.size(); i++)
    {
        cout << s[i] << endl;
    }
    cout << "-------------------------------------------------------------------------------------------" << endl;

    //������ļ�
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
    //-1Ϊ��
    string storageStatus[STORAGE_SIZE];
    for (int i = 0; i < STORAGE_SIZE; i++)
    {
        storageStatus[i] = to_string(i) + " 1";
    }
    for (int i = 0; i < freeSpaceList.size(); i++)
    {
        storageStatus[freeSpaceList[i]] = to_string(freeSpaceList[i]) + " -1";
    }

    //������ļ�
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
        cout << "ɾ���ڵ�ʧ�ܣ���·��������" << endl;
        return 1;
    }
    else
    {
        postOrderDelSubTree(pnode);
        cout << "ɾ�������ɹ���" << endl;
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
        cout << "��path�����ڻ��path�²�������Ϊ" << oldname << "���ļ����ļ���!" << endl;
        cout << "������ʧ�ܣ�" << endl;
        return 1;
    }
    else
    {
        int retu = checkDupName(aim->parent, newname);
            if (retu == 0)//û�������ļ�
            {
                aim->name = newname;
                cout << "�������ɹ���" << endl;
              
            }
            else
            {
                cout << "��path�´��������ļ���\n������ʧ�ܣ�" << endl;
                
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
        cout << "��Ϊ" << name << "���ļ����ļ��в����ڣ�" << endl;
    }
    else
    {

        cout << "����Ϊ������Ϊ" << name << "���ļ����ļ��е�·��:" << endl;
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


TreeNode*& FileSystem::matchPath(string path)//���� dirTree ����һ��path������ĩβ�Ľڵ�
{
    vector<string> res = split(path, "/");
    TreeNode* temp = dirTree;
    TreeNode* lastptr = nullptr;

    int i;
    for (i = 0; i < res.size(); i++)
    {
        while (temp != nullptr)
        {
            if (temp->name == res[i] /*&& temp->type == 1*/)//�ڵ�ǰ���ҵ���
            {
                if (i < res.size() - 1)
                {
                    lastptr = temp;
                    temp = temp->child;//������һ���ƥ��
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
        cout << "·��ƥ��ɹ���" << endl;

        if (lastptr == nullptr)
            return dirTree;
        else if (lastptr->child == temp)
            return lastptr->child;
        else
            return lastptr->sibling;
    }
    else
    {
        cout << "�����ҵ�·��������" << endl;
        return temp;
    }
}

int FileSystem::create(string path,  string name,  int type, int size)//��path�´����ļ����ļ���name,��Ϊ֮����size�����ݿ�ռ�
{
    //�ҵ�����Ŀ¼�ڵ��λ��
    TreeNode * pnode=matchPath(path);
    
    if (pnode == nullptr)
    {
        cout << "��ǰĿ¼�����ڣ������½ڵ�ʧ�ܣ�\n" << endl;
        return 1;
    }
    if (pnode->type == 2)
    {
        cout << "�����ڷ�Ŀ¼�´����ڵ㣬�����½ڵ�ʧ�ܣ�" << endl;
        return 1;
    }
    TreeNode** aim = nullptr;
    if (pnode->child == nullptr)//��Ŀ¼�»�û�ж���ʱ
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
                cout << "��ǰĿ¼�´���ͬ���ļ�/�ļ��У������½ڵ�ʧ�ܣ�\n" << endl;
                return 1;
            }
            lastptr = currentptr;
            currentptr = currentptr->sibling;
        }
        aim = &lastptr->sibling;
       /* lastptr->sibling= new TreeNode(type, name, size, pnode, nullptr, nullptr);
        applyForExternalStorage(*lastptr->sibling);*/
    }

    //����Ŀ¼�ڵ㲢�������
    *aim= new TreeNode(type, name, size, pnode, nullptr, nullptr);
    int ans = applyForExternalStorage(**aim);
    if (ans == 0)
    {
        
        cout << "�½ڵ㴴���ɹ���" << endl;
    }
    else
    {
        cout << "�½ڵ㴴��ʧ�ܣ�" << endl;
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
        cout << "�������ռ�ɹ�!" << endl;
       /* printFreeSpaceList();
        cout << "�����пռ��СΪ" << freeSpaceList.size() << endl;*/
        int size=freeSpaceList.size();
        utiliRate = 100*(STORAGE_SIZE - freeSpaceList.size()) / STORAGE_SIZE;
        return 0;
    }
    else
    {
        fcb.size = 0;
        cout << "�������ռ�ʧ�ܣ����ռ䲻�㣡" << endl;
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
        if (currentptr->name == name)//�������ļ�
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
    cout << "\n���пռ��б�Ϊ:" << endl;
    for (int i = 0; i < freeSpaceList.size(); i++)
    {
        cout << freeSpaceList[i] << ' ';
    }
    cout << endl;
}

void FileSystem::printUtiliRate()
{

    cout<<"��ǰ���ʹ����Ϊ:" << utiliRate << endl;
}

vector<string> FileSystem::split(const string& str, const string& delim)//�ַ�����Ƭ
{
    vector<string> res;
    if ("" == str) return res;
    //�Ƚ�Ҫ�и���ַ�����string����ת��Ϊchar*����  
    int len1 = str.length() + 1;//Ҫ����һ��\0
    char* strs = new char[len1]; 
    strcpy_s(strs,len1, str.c_str());

    int len2 = delim.length() + 1;
    char* d = new char[len2];
    strcpy_s(d, len2,delim.c_str());

    char* buf=NULL;
    char* p = strtok_s(strs, d,&buf);

    while (p)
    {
        string s = p; //�ָ�õ����ַ���ת��Ϊstring����  
        res.push_back(s); //����������  
        p = strtok_s(NULL, d,&buf);
    }

    delete [] strs;
    delete [] d;

    cout << "\n·��Ϊ��" << endl;
    for (int i = 0; i < res.size(); i++)
    {
        std::cout << res[i] << " ";
    }
    //cout << "�õ����ֶ�����" << res.size() << endl;
    return res;
}
