#include <windows.h>
#include "global.h"
#include "state.h"
#include "FileSystem.h"
#include "FCB.h"
#include "DB.h"
#include "DeviceControl.h"


using namespace std;
int pidSum;//当前进程总数
int Timer;//时钟
int memory;//内存总大小
bool CPUbusy;//CPU是否运算中+
bool KeyBoardBusy;//键盘是否busy
bool printBusy;//打印机是否busy
int policy;
list<string> ins;


//进程同步 读写文件时候可以使用信号量！！！！！！！！

/*中断向量表 中断向量表储存中断类型与对应的中断程序的入口*/
map<int, int>interruptTable;
map<string, struct mutexInfo>fileMutex;
CPU cpu;
FileSystem fs;


list<PCB> PCBList;//总进程队列
list<PCB> readyList;//就绪队列
list<PCB> waitForMemoryList;//等待内存队列
list<PCB> waitForKeyBoardList;//等待键盘队列
list<PCB> waitForPrintList;//等待打印机队列
list<PCB> blockList;//阻塞队列
list<PCB> deviceList;//设备队列
list<PCB> externalPCBList;//挂起进程队列


void init() {
	pidSum = 0;
	Timer = 0;
	memory = 10;
	CPUbusy = false;
	KeyBoardBusy = false;
	printBusy = false;
	policy = SCHED_PRO;
	//中断向量表初始化
	interruptTable[NORMAL_SWITCH] = PC_NORMAL_SWITCH;
	interruptTable[PREEMPTION_SWITCH] = PC_PREEMPTION_SWITCH;
	interruptTable[MIDTERM_SWITCH_IN] = PC_MIDTERM_SWITCH_IN;
	interruptTable[MIDTERM_SWITCH_OUT] = PC_MIDTERM_SWITCH_OUT;
	interruptTable[KEYBOARD_SWITCH_USE] = PC_KEYBOARD_SWITCH_USE;
	interruptTable[KEYBOARD_SWITCH_UNUSE] = PC_KEYBOARD_SWITCH_UNUSE;
	interruptTable[PRINT_SWITCH_USE] = PC_PRINT_SWITCH_USE;
	interruptTable[PRINT_SWITCH_UNUSE] = PC_PRINT_SWITCH_UNUSE;

	//文件临界区资源初始化，这里需要对已经存在的文件 将其的mutex设置为false
	//需要文件那边给我一个数组或vector，内容为目前文件系统中的所有文件的路径(注意是 文件 的路径！)
	vector<string>fileExist;
	fileExist = fs.printAllFilesPath();
	for (int i = 0; i < fileExist.size(); i++) {
		fileMutex[fileExist[i]].isBusy = false;//这个步骤的目的是为了实现 所有文件初始化都没有人访问即临界区false
	}

	cpu.pid = -1;
	cpu.PC = 0;
	initMemory();
	//内存

};
string state[7] = { "Creating",   "Ready",    "Running", "Block",
				   "Suspend", "Suspend", "End" };
string policyStr[5] = { "","PRO","RR","FCFS" };

PCB create(int blocks, int prority, list<string> temp) {
	PCB p;
	p.uid = (short)1;
	p.pid = pidSum + 1;
	p.processer = 1;
	p.state = TASK_CREATING;
	p.cpuState = USERMODE;
	p.prority = prority;
	p.utime = -1;
	p.mm = (struct mm_struct*)malloc(sizeof(mm_struct));
	p.type = NOTBLOCK;
	p.mm->task_size = blocks;
	p.mm->is_apply = false;
	p.program = temp;
	p.wtime.keyboardStartTime = -1;
	pidSum++;
	p.start_time = (u64)Timer;
	cout << "****创建进程pid=" << p.pid << "中****" << endl;
	return p;
};

void ready(PCB& p) {  //进入就绪状态
	p.state = TASK_READY;
	readyList.push_back(p);
	changePCBList(p, TASK_READY);
	cout << "Timer=" << Timer << " 进程pid=" << p.pid << "进入就绪队列" << endl;
};
void block(PCB& p) {
	p.state = TASK_BLOCK;
	blockList.push_back(p);
	changePCBList(p, TASK_BLOCK);
	cout << "Timer=" << Timer << " 进程pid=" << p.pid << "进入阻塞队列" << endl;
};
void stop(PCB& p) {
	cout << "Timer=" << Timer << " 进程pid=" << p.pid << "结束" << endl;
	changePCBList(p, TASK_DEAD);
	releaseMemory(p);
	//memory += p.mm->task_size;
	//readyList.pop_front();
};

void waitForFile(string filePath) {
	//cout << "p1.list" << fileMutex[p.fs].waitForFileList.back().fsState << endl;
	cout << "filePath" << filePath;
	if (fileMutex[filePath].isBusy == false) {//如果当前的临界区没有进程访问
		if (!fileMutex[filePath].waitForFileList.empty()) {
			cout << "Timer=" << Timer << " 进程pid=" << fileMutex[filePath].waitForFileList.front().pid << "开始I/O文件:" << filePath << endl;
			fileMutex[filePath].isBusy = true;
			if (fileMutex[filePath].waitForFileList.front().fsState == "W") {
				fs.edit(filePath, "this is a test text\n");
			}
			else if (fileMutex[filePath].waitForFileList.front().fsState == "R") {
				//fs.read(p.fs);
				cout << "read file" << endl;
				string s = "read file:" + fs.read(filePath);
				ins.push_front(s);
			}
			else{}

			fileMutex[filePath].waitForFileList.front().wtime.writeStartTime = Timer;  //将第一个进程开始占用keyboard
			for (list<PCB>::iterator it = blockList.begin();
				it != blockList.end(); ++it) {
				if (it->pid == fileMutex[filePath].waitForFileList.front().pid) {
					it->wtime.writeStartTime = Timer;
				}
			}

		}
	}
}

void waitForPrint() {
	//检查当前的wait队列 看是否有进程正在等待或占用键盘
	if (DeviceControl('q', 'p', 'o') != -1) {
		if (!waitForPrintList.empty()) {
			//cout << "Timer=" << Timer << " 进程pid=" << waitForPrintList.front().pid << "开始占用打印机" << endl;
			//printBusy = true;
			DeviceControl('r', 'p', 'o', waitForPrintList.front().pid);
			waitForPrintList.front().wtime.printStartTime = Timer;  //将第一个进程开始占用keyboard
			for (list<PCB>::iterator it = blockList.begin();
				it != blockList.end(); ++it) {
				if (it->pid == waitForPrintList.front().pid) {
					it->wtime.printStartTime = Timer;
				}
			}
		}
	}
};

void waitForKeyBoard() {
	//检查当前的wait队列 看是否有进程正在等待或占用键盘
	if (DeviceControl('q', 'k', 'i') != -1) {
		if (!waitForKeyBoardList.empty()) {
			cout << "Timer=" << Timer << " 进程pid=" << waitForKeyBoardList.front().pid << "开始占用键盘" << endl;
			//KeyBoardBusy = true;
			DeviceControl('r', 'k', 'i', waitForKeyBoardList.front().pid);
			waitForKeyBoardList.front().wtime.keyboardStartTime = Timer;  //将第一个进程开始占用keyboard
			for (list<PCB>::iterator it = blockList.begin();
				it != blockList.end(); ++it) {
				if (it->pid == waitForKeyBoardList.front().pid) {
					it->wtime.keyboardStartTime = Timer;
				}
			}
		}
	}
};

void checkState() {
	ofstream f("./OS_UI/commute/process.txt");
	ofstream log("log.txt", ios::app);
	ofstream timer("./OS_UI/commute/pc.txt");
	ofstream insstate("./OS_UI/commute/state.txt");

	

	//f << "hello" << endl;
	//f.close();
	cout << endl << "当前CPU时间：" << Timer << endl;
	cout << "当前操作系统的进程共" << PCBList.size() << "个:" << endl;
	cout << "--------------------------------------------------" << endl;
	cout << "pid  " << "state  " << "占用内存  " << "创建时间  " << "优先级  " << "调度策略" << endl;
	log << endl << "当前CPU时间： " << Timer << endl;
	log << "pid\t" << "state\t" << "占用内存\t" << "优先级\t" << "调度策略" << endl;
	timer << Timer << endl;
	for (list<PCB>::iterator it = PCBList.begin(); it != PCBList.end(); ++it) {
		if (it->mm->is_apply) {
			cout << it->pid << "    " << state[it->state] << "    "
				<< it->mm->task_size << "          " << it->start_time << "       " << it->prority << "      " << policyStr[policy] << endl;
			f << it->pid << " " << state[it->state] << " "
				<< it->mm->task_size << " " << it->prority << " " << policyStr[policy] << endl;
			log << it->pid << "\t" << state[it->state] << "\t"
				<< it->mm->task_size << "\t" << it->prority << "\t" << policyStr[policy] << endl;
			string s;
			s.append("process");
			s.append(to_string(it->pid));
			s.append(": ");
			s.append(it->program.front());
			ins.push_front(s);
		}
		else {
			cout << it->pid << "    " << state[it->state] << "    "
				<< 0 << "          " << it->start_time << "       " << it->prority << "      " << policyStr[policy] << endl;
			f << it->pid << " " << state[it->state] << " "
				<< 0 << " " << it->prority << " " << policyStr[policy] << endl;
			log << it->pid << "\t" << state[it->state] << "\t"
				<< 0 << "\t" << it->prority << "\t" << policyStr[policy] << endl;
		}
		
	}
	ins.push_front("-----------------------");
	for (list<string>::iterator it = ins.begin(); it != ins.end(); ++it) {
		insstate << *it << endl;
	}
	log << endl << endl;
	f.close();
	log.close();
	timer.close();
	insstate.close();
	printMemoryInfo();
	fs.sendExternalStorageStatusToUI();
	devicePrint();
	cout << "---------------------------------------------------" << endl
		<< endl;
	cout << "当前处于就绪状态的进程共" << readyList.size() << "个:" << endl;
	cout << "--------------------------------------------------" << endl;
	cout << "pid" << "  " << "state" << "  " << "占用内存块数" << "  " << "创建时间" << " " << "优先级" << endl;
	for (list<PCB>::iterator it = readyList.begin(); it != readyList.end();
		++it) {
		cout << it->pid << "    " << state[it->state] << "    "
			<< it->mm->task_size << "          " << it->start_time << "    " << it->prority << endl;
	}

	cout << "---------------------------------------------------" << endl
		<< endl;
	cout << "当前处于阻塞状态的进程共" << blockList.size() << "个:" << endl;
	cout << "--------------------------------------------------" << endl;
	cout << "pid" << "  " << "state" << "  " << "占用内存块数" << "  " << "创建时间" << " " << "优先级" << endl;
	for (list<PCB>::iterator it = blockList.begin(); it != blockList.end();
		++it) {
		cout << it->pid << "    " << state[it->state] << "    "
			<< it->mm->task_size << "          " << it->start_time << "    " << it->prority << endl;
	}

	cout << "---------------------------------------------------" << endl
		<< endl;
	cout << "当前处于等待内存状态的进程共" << waitForMemoryList.size()
		<< "个:" << endl;
	cout << "--------------------------------------------------" << endl;
	cout << "pid" << "  " << "state" << "  " << "占用内存块数" << "  " << "创建时间" << " " << "优先级" << endl;
	for (list<PCB>::iterator it = waitForMemoryList.begin();
		it != waitForMemoryList.end(); ++it) {
		cout << it->pid << "    " << state[it->state] << "    "
			<< it->mm->task_size << "          " << it->start_time << "    " << it->prority << endl;
	}
	cout << "---------------------------------------------------" << endl
		<< endl;

	cout << "因中期调度调出内存的进程共" << externalPCBList.size()
		<< "个:" << endl;
	cout << "--------------------------------------------------" << endl;
	cout << "pid" << "  " << "state" << "  " << "释放内存块数" << "  " << "创建时间" << " " << "优先级" << endl;
	for (list<PCB>::iterator it = externalPCBList.begin();
		it != externalPCBList.end(); ++it) {
		cout << it->pid << "    " << state[it->state] << "    "
			<< 0 << "          " << it->start_time << "    " << it->prority << endl;
	}
	cout << "---------------------------------------------------" << endl
		<< endl;
};

void blockdelete(PCB& p) {
	for (list<PCB>::iterator i = blockList.begin(); i != blockList.end();) {
		if (i->pid == p.pid) {
			blockList.erase(i++);
		}
		else {
			i++;
		}
	}
};

void Kdelete(PCB& p) {
	for (list<PCB>::iterator i = waitForKeyBoardList.begin(); i != waitForKeyBoardList.end();) {
		if (i->pid == p.pid) {
			waitForKeyBoardList.erase(i++);
		}
		else {
			i++;
		}
	}
	//KeyBoardBusy = false;
	DeviceControl('c', 'k', 'i', p.pid);
};

void Pdelete(PCB& p) {
	for (list<PCB>::iterator i = waitForPrintList.begin(); i != waitForPrintList.end();) {
		if (i->pid == p.pid) {
			waitForPrintList.erase(i++);
		}
		else {
			i++;
		}
	}
	DeviceControl('c', 'p', 'o', p.pid);
};

void externaldelete(PCB& p) {
	for (list<PCB>::iterator i = externalPCBList.begin(); i != externalPCBList.end();) {
		if (i->pid == p.pid) {
			externalPCBList.erase(i++);
		}
		else {
			i++;
		}
	}
};

void readydelete(PCB& p) {
	for (list<PCB>::iterator i = readyList.begin(); i != readyList.end();) {
		if (i->pid == p.pid) {
			readyList.erase(i++);
		}
		else {
			i++;
		}
	}
};


/*进程是否处在占用键盘或等待键盘事件中*/
bool isInK(PCB& p) {
	for (list<PCB>::iterator i = waitForKeyBoardList.begin(); i != waitForKeyBoardList.end(); ++i) {
		if (i->pid == p.pid) {
			return true;
		}
	}
	return false;
};

/*进程是否处在占用键盘或等待键盘事件中*/
bool isInP(PCB& p) {
	for (list<PCB>::iterator i = waitForPrintList.begin(); i != waitForPrintList.end(); ++i) {
		if (i->pid == p.pid) {
			return true;
		}
	}
	return false;
};

void MidTermScheduler(int inOrOut) {
	if (inOrOut == OUT) {
		int size = 0;
		for (int i = 0; i < blockList.size(); ++i) {
			pair<int, int> pcbPair = { INT_MAX, -1 };
			for (list<PCB>::iterator it = blockList.begin(); it != blockList.end(); ++it) {
				if (it->prority < pcbPair.first && it->mm->is_apply) {//优先调出优先级低的进程
					pcbPair.first = it->prority;
					pcbPair.second = it->pid;
				}
			}
			if (pcbPair.second != -1) {
				for (list<PCB>::iterator it = blockList.begin(); it != blockList.end(); ++it) {
					if (it->pid == pcbPair.second) {
						size += it->mm->task_size;
						memory += it->mm->task_size;
						it->mm->is_apply = false;

						Interrupt(*it, MIDTERM_SWITCH_OUT);

						break;
					}
				}
			}
			if (size > PHYSICAL_SIZE / 3) {
				break;
			}
		}
	}
	else if (inOrOut == IN) {
		pair<int, int> pcbPair = { INT_MIN, -1 };
		for (list<PCB>::iterator it = externalPCBList.begin(); it != externalPCBList.end(); ++it) {
			if (it->prority > pcbPair.first && it->mm->task_size < memory) {
				pcbPair.first = it->prority;
				pcbPair.second = it->pid;
			}
		}
		if (pcbPair.second != -1) {
			for (list<PCB>::iterator it = externalPCBList.begin(); it != externalPCBList.end(); ++it) {
				if (it->pid == pcbPair.second) {
					Interrupt(*it, MIDTERM_SWITCH_IN);
					break;
				}
			}
		}
	}
};

void updateTaskState() {

	//每经过一个节拍更新进程状态,遍历整个进程列表
	for (list<PCB>::iterator it = waitForMemoryList.begin();
		it != waitForMemoryList.end();) {
		//每次都要检测一下 将因为内存不够而阻塞的进程唤醒
		if (applyForMemory(*it)) {
			//申请成功 转入就绪状态
			changePCBList(*it, TASK_READY);
			it->state = TASK_READY;
			it->mm->is_apply = true;
			readyList.push_back(*it);
			/*将blockList中的进程删除*/
			blockdelete(*it);
			waitForMemoryList.erase(it++);
		}
		else {
			it++;
		}
	}

	/*遍历等待键盘队列*/
	if (!waitForKeyBoardList.empty()) {
		PCB& keyP = waitForKeyBoardList.front();
		string s = keyP.program.front();
		int keyTime = atoi(s.substr(2, s.size()).c_str());
		//cout<<keyP.wtime.keyboardStartTime<<"  "<<keyTime<< endl;
		cout << "pid=" << keyP.pid << " " << "time:" << Timer - keyP.wtime.keyboardStartTime << endl;
		if ((Timer - keyP.wtime.keyboardStartTime) == keyTime) {


			Interrupt(keyP, KEYBOARD_SWITCH_UNUSE);
		}
	}

	/*遍历等待打印机队列*/
	if (!waitForPrintList.empty()) {
		PCB& printP = waitForPrintList.front();
		string s = printP.program.front();
		int printTime = atoi(s.substr(2, s.size()).c_str());
		//cout<<keyP.wtime.keyboardStartTime<<"  "<<keyTime<< endl;
		cout << "pid=" << printP.pid << " " << "time:" << Timer - printP.wtime.printStartTime << endl;
		if ((Timer - printP.wtime.printStartTime) == printTime) {
			Interrupt(printP, PRINT_SWITCH_UNUSE);
		}
	}
	/*遍历等待fileMutex的map*/
	map<string, struct mutexInfo>::iterator it;
	for (it = fileMutex.begin(); it != fileMutex.end(); it++) {
		if (it->second.isBusy == true) {
			//如果是busy状态 那么就要检查头进程是否读写完成了
			PCB& writeP = it->second.waitForFileList.front();
			string s = writeP.program.front();
			string writeTime = s.substr(2, s.size()).c_str();
			string path, wtime;
			stringstream input(writeTime);
			input >> path >> wtime;
			cout << wtime << endl;
			//cout<<keyP.wtime.keyboardStartTime<<"  "<<keyTime<< endl;
			cout << "pid=" << writeP.pid << " " << "wtime:" << Timer - writeP.wtime.writeStartTime << endl;
			if ((Timer - writeP.wtime.writeStartTime) == atoi(wtime.c_str())) {
				Interrupt(writeP, FILE_SWITCH_UNUSE);
			}
		}


	}

	/*遍历执行中进程*/
	for (list<PCB>::iterator it = PCBList.begin(); it != PCBList.end(); ++it) {
		if (it->state == TASK_RUNNING) {
			it->utime++;
			string s = it->program.front();
			int utime = it->utime;
			int alltime = atoi(s.substr(2, s.size()).c_str());
			cout << "utime:" << utime << endl;
			cout << "alltime" << alltime << endl;
			if (utime == alltime) {
				/*执行上下文切换 将被切换进程的状态保存在PCB中*/
				Interrupt(*it, NORMAL_SWITCH);

			}
		}
		else if (it->state == TASK_BLOCK) {

		}
	}
	if (getSize() > 5 && !externalPCBList.empty()) {
		MidTermScheduler(IN);
	}
};



void applyForResource(PCB& p) {
	//暂时只考虑到内存资源
	if (applyForMemory(p)) {  //代表内存分配成功，进入就绪队列

		AllocateMsg s = allocateSuccess(p);


		ready(p);           //将进程p由创建状态转入就绪状态
		p.mm->is_apply = true;//成功分配内存
	}
	else {
		//此时内存分配失败，开始执行检查阻塞队列判断是否进行中期调度
		cout << "内存分配失败，进程pid=" << p.pid << "转入等待内存队列" << endl;
		block(p);
		waitForMemoryList.push_back(p);
		if (blockList.size() > MAX_BLOCK) {
			MidTermScheduler(OUT);  //中期调度将处于阻塞状态的任务选择部分换出释放内存空间
		}
		else {
			//不执行中期调度，等待
		}
	}
};

void LongTermScheduler(string filename) {
	//长期调度，消费者将外存中的程序调度内存并创建进程
	ifstream fp;
	fp.open(filename.c_str(), ios::in);
	if (fp) {
		list<string> temp;
		string line;
		int blocks;  //占用的内存块数
		int prority;
		while (getline(fp, line))  // line中不包括每行的换行符
		{
			//处理各种在创建进程时相关的指令
			if (line[0] == 'M') {  //如果是占用内存相关指令 则计算出需要占用的内存块数
				string s = line.substr(2, line.size());
				blocks = atoi(s.c_str());
			}
			else if (line[0] == 'Y') {  //如果是优先级相关指令 则计算出优先级
				string s = line.substr(2, line.size());
				prority = atoi(s.c_str());
			}
			else {
				cout << line << endl;
				temp.push_back(line);  //否则当做程序指令 暂时存放在temp中
			}
		}
		cout << "将程序" << filename << "调入内存" << endl;
		PCB p = create(blocks, prority, temp);
		PCBList.push_back(p);
		applyForResource(p);
	}
};


void Interrupt(PCB& p, int reason) {
	p.cpuState = KERNELMODE;//中断状态下 进程处于内核模式
	cpu.PC = interruptTable[reason];//查询中断向量表 找到中断程序的入口程序(模拟)
	//进程正常结束或遇到I/O阻塞等等进程进行切换时 需要保存上下文环境 将旧进程的状态保存在PCB中 新进程调入CPU
	//如果是正常的进程占用时间片结束
	if (reason == NORMAL_SWITCH) {
		p.utime = -1;
		p.program.pop_front();
		cpu.pid = -1;
		CPUbusy = false;
		ready(p);
	}
	//抢占式进程切换 需要保存上一个进程已占用CPU时间(或者在总时间上减去已占用时间)
	if (reason == PREEMPTION_SWITCH) {
		for (list<PCB>::iterator it = PCBList.begin(); it != PCBList.end(); ++it) {
			if (it->pid == p.pid) {
				if (p.utime != -1) {
					string s = it->program.front();
					int alltime = atoi(s.substr(2, s.size()).c_str());
					alltime -= it->utime + 1;
					it->program.pop_front();
					if (alltime != 0) {
						it->program.push_front("C " + to_string(alltime));
					}
					it->utime = -1;
				}
				it->cpuState = USERMODE;
				changePCBList(*it, TASK_READY);
				readyList.push_back(*it);
				break;
			}
		}
	}
	/*中期调度中断 保存占用设备时间等信息*/
	if (reason == MIDTERM_SWITCH_OUT) {
		cout << "Timer=" << Timer << " 进程pid=" << p.pid << "调出内存" << endl;
		medium_swap_out(p.pid);
		if (isInK(p)) {
			Kdelete(p);
			string s = p.program.front();
			cout << "pid=" << p.pid << " wTime " << p.wtime.keyboardStartTime << endl;
			if (p.wtime.keyboardStartTime != -1) {
				int keyTime = atoi(s.substr(2, s.size()).c_str());
				keyTime -= (Timer - p.wtime.keyboardStartTime);
				cout << "keyTime" << keyTime << endl;
				p.program.pop_front();
				if (keyTime != 0) {
					p.program.push_front("K " + to_string(keyTime));
				}
			}
		}
		if (isInP(p)) {
			Pdelete(p);
			p.type = PRINT;
			string s = p.program.front();
			cout << "pid=" << p.pid << " wTime " << p.wtime.keyboardStartTime << endl;
			if (p.wtime.printStartTime != -1) {
				int printTime = atoi(s.substr(2, s.size()).c_str());
				printTime -= (Timer - p.wtime.printStartTime);
				p.program.pop_front();
				if (printTime != 0) {
					p.program.push_front("P " + to_string(printTime));
				}
			}
		}
		externalPCBList.push_back(p);
		changePCBList(p, TASK_SUSPEND_BLOCK);
		blockdelete(p);
	}
	if (reason == MIDTERM_SWITCH_IN) {
		medium_swap_in(p);
		if (p.type == KEYBOARD) { //键盘阻塞
			memory -= p.mm->task_size;
			p.mm->is_apply = true;
			p.type = NOTBLOCK;
			cout << "Timer=" << Timer << " 进程pid=" << p.pid << "调入内存" << endl;
			readyList.push_back(p);
			changePCBList(p, TASK_READY);
		}
		else if (p.type == PRINT) {
			memory -= p.mm->task_size;
			p.mm->is_apply = true;
			p.type = NOTBLOCK;
			cout << "Timer=" << Timer << " 进程pid=" << p.pid << "调入内存" << endl;
			readyList.push_back(p);
			p.cpuState = USERMODE;
			changePCBList(p, TASK_READY);
		}
		externaldelete(p);
	}
	/*以下内容为IO中断*/
	if (reason == KEYBOARD_SWITCH_USE) {
		//查找中断向量表 执行中断程序 保存中断状态
		block(p);
		waitForKeyBoardList.push_back(p);
		waitForKeyBoard();  //执行键盘输入函数
	}
	if (reason == KEYBOARD_SWITCH_UNUSE) {
		p.cpuState = USERMODE;
		p.program.pop_front();
		ready(p);
		blockdelete(p);
		waitForKeyBoardList.pop_front();
		for (list<PCB>::iterator it = PCBList.begin(); it != PCBList.end(); ++it) {
			if (it->pid == p.pid) {
				it->program.pop_front();
			}
		}
		DeviceControl('c', 'k', 'i', p.pid);
		waitForKeyBoard();
	}
	if (reason == PRINT_SWITCH_USE) {
		//查找中断向量表 执行中断程序 保存中断状态
		block(p);
		waitForPrintList.push_back(p);
		waitForPrint();  //执行键盘输入函数
	}
	if (reason == PRINT_SWITCH_UNUSE) {
		p.cpuState = USERMODE;
		p.program.pop_front();
		ready(p);
		blockdelete(p);
		waitForPrintList.pop_front();
		for (list<PCB>::iterator it = PCBList.begin(); it != PCBList.end(); ++it) {
			if (it->pid == p.pid) {
				it->program.pop_front();
			}
		}
		DeviceControl('c', 'p', 'o', p.pid);
		waitForPrint();
	}
	//开始写文件
	if (reason == FILE_SWITCH_USE) {
		block(p);
		fileMutex[p.fs].waitForFileList.push_back(p);
		cout << "push:" << fileMutex[p.fs].waitForFileList.back().fsState << endl;
		waitForFile(p.fs);  //执行键盘输入函数
	}
	//结束写文件
	if (reason == FILE_SWITCH_UNUSE) {
		cout << p.fsState << endl;
		p.program.pop_front();
		PCB &p1 = p;
		ready(p);
		blockdelete(p);
		cout << p1.fs << endl;
		string ff = p1.fs;
		map<string, struct mutexInfo>::iterator it;
		for (it = fileMutex.begin(); it != fileMutex.end(); it++) {
			cout << it->first << endl;
			if (it->first == p.fs && it->second.isBusy == true) {
				it->second.waitForFileList.pop_front();
				it->second.isBusy = false;
				break;
			}
		}
		for (list<PCB>::iterator it = PCBList.begin(); it != PCBList.end(); ++it) {
			if (it->pid == p1.pid) {
				it->program.pop_front();
			}
		}
		waitForFile(ff);
	}
	if (reason == PAGE_FAULT) {
		cout << "缺页中断" << endl;
	}
};



void changePCBList(PCB& p, int state) {
	for (list<PCB>::iterator it = PCBList.begin(); it != PCBList.end(); ++it) {
		if (state == TASK_RUNNING) {
			cpu.pid = p.pid;
		}
		if (it->pid == p.pid) {
			if (state == TASK_DEAD) {
				it->program.pop_front();
				it->program.push_front("NOP");
			}
			it->state = state;
			it->cpuState = p.cpuState;
		}
	}
};



void CPUScheduler(PCB& p) {
	//短期调度将从就绪队列中选择进程调度CPU中执行
	cout << "Timer=" << Timer << " 进程pid=" << p.pid << "从就绪态转入运行态" << endl;
	changePCBList(p, TASK_RUNNING);
	CPUbusy = true;
};

void addPrority() {
	/*给readyList中的进程优先级增加*/
	for (list<PCB>::iterator it = readyList.begin(); it != readyList.end(); ++it) {
		if (it->prority <= 100) {
			it->prority++;
		}
	}
	for (list<PCB>::iterator it = PCBList.begin(); it != PCBList.end(); ++it) {
		if (it->state == TASK_READY) {
			if (it->prority <= 100) {
				it->prority++;
			}
		}
	}//不使用链表的一个小弊端就是不能同时更新哈哈哈哈 手动更新
}

/*chensure 二次修改 代码现在很有逻辑性*/
void Execute() {
	int max = INT_MIN;
	for (list<PCB>::iterator it = readyList.begin(); it != readyList.end(); ++it) {
		if (it->prority > max) {
			max = it->prority;
		}
	}//寻找readylist中的最高优先级
	//执行就绪队列中排队的进程
	for (list<PCB>::iterator it = readyList.begin(); it != readyList.end();) {
		string s = it->program.front().substr(0, 1);
		cout << "当前指令:" << s << endl;
		if (s == "C") {  //如果指令是C，说明需要占用CPU
			if (!CPUbusy) {  //如果当前CPU中并无进程在运行,则安排firstP
				CPUScheduler(*it);  //第一个是CPU计算
				readyList.erase(it++);
			}else {
				//如果有进程在运行，则需要根据调度算法来进行进程的替换
				//这里写各种调度算法运行的结果
				//抢占式动态优先级，SJF，每执行一次优先级下降3，等待一次优先级上升1
				if (policy == SCHED_PRO) {
					for (list<PCB>::iterator it1 = PCBList.begin(); it1 != PCBList.end(); ++it1) {
						if (it1->state == TASK_RUNNING) {
							if (it1->prority >= max) {
								it++;
								break;
							}else {
								if (it->prority == max) {
									/*抢占CPU 需要中断*/
									Interrupt(*it1, PREEMPTION_SWITCH);
									CPUScheduler(*it);
									readyList.erase(it++);
								}else {
									it++;
								}
							}
							break;
						}
					}
				}else if (policy == SCHED_RR) {
					for (list<PCB>::iterator it1 = PCBList.begin(); it1 != PCBList.end(); ++it1) {
						if (it1->state == TASK_RUNNING) {
							cout << "running" << endl;
							Interrupt(*it1, PREEMPTION_SWITCH);
							CPUScheduler(*it);
							readyList.erase(it++);
							break;
						}
					}
					break;
				}else {
					it++;
				}
			}
		}
		/*除了C之外的所有命令都执行中断，进入阻塞队列，进行相应的操作*/
		else if (s == "K") {  //执行键盘输入
			Interrupt(*it, KEYBOARD_SWITCH_USE);
			readyList.erase(it++);
		}
		else if (s == "P") {  //执行打印机输出
			Interrupt(*it, PRINT_SWITCH_USE);
			readyList.erase(it++);
		}
		else if (s == "R") {  //执行文件读取
			string filePath = it->program.front().substr(2, it->program.front().size() - 1);
			string path, rtime;
			stringstream input(filePath);
			input >> path >> rtime;
			cout << "filePath:" << path << endl;
			if (fs.matchPath(path) == NULL) {
				cout << "目录不存在，跳过该指令" << endl;
				it->program.pop_front();
			}else {
				it->fs = path;//此时进程访问文件的路径
				it->fsState = "R";
				Interrupt(*it, FILE_SWITCH_USE);
				readyList.erase(it++);
			}
		}
		else if (s == "W") {  //执行文件写入
			string filePath = it->program.front().substr(2, it->program.front().size() - 1);
			string path, wtime;
			cout << filePath << endl;
			stringstream input(filePath);
			input >> path >> wtime;
			cout << "filePath:" << path << endl;
			//此时path就是真实的路径 第一步要检测这个文件是否存在
			if (fs.matchPath(path) == NULL) {
				cout << "目录不存在，跳过该指令" << endl;
				it->program.pop_front();
			}else {//如果在文件系统中找到了这个文件，那么第一步 看这个文件的互斥信号量是否为false（代表这个文件目前没有进程访问）
			 //查询map,看临界区是否有进程正在访问
				it->fs = path;//此时进程访问文件的路径
				it->fsState = "W";
				Interrupt(*it, FILE_SWITCH_USE);
				readyList.erase(it++);
			}
		}
		else if (s == "Q") {  //进程正常结束退出
			stop(*it);
			readyList.erase(it++);

		}
	}
}

void checkPrority() {
	for (list<PCB>::iterator it = PCBList.begin(); it != PCBList.end(); ++it) {
		if (it->state == TASK_RUNNING) {
			//cout<<"prority"<<endl;
			if (it->prority >= 3) {
				it->prority -= 3;//进程优先级在每次PC运行后会减少
				// cout << it->prority << endl;
			}
			else {
				it->prority = 0;
			}
		}
	}
	addPrority();
};
void handleMemory() {
	int runingPid = cpu.pid;
	cout << "当前运行进程:" << runingPid << endl;
	if (runingPid == -1) {
	}
	else {
		for (int i = 0; i < PROCESS_NUM; ++i) {
			if (runingPid == process_list[i].pid) {
				bool pageFault = handle_process(process_list[i]);
				PCB p;
				if (pageFault) {
					Interrupt(p, PAGE_FAULT);
				}
				//handle_process(process_list[i]);
				break;
			}
		}
	}
}
void getOption() {
	ifstream addDevice("./OS_UI/commute/add_device.txt");
	ifstream deleteDevice("./OS_UI/commute/delete_device.txt");
	ifstream policyControl("./OS_UI/commute/al.txt");
	if (addDevice) {
		string line;
		while (getline(addDevice, line))  // line中不包括每行的换行符
		{
			char deviceName = line[0];
			DeviceControl('a', deviceName, 'i');
		}
	}
	if (deleteDevice) {
		string line;
		while (getline(deleteDevice, line))  // line中不包括每行的换行符
		{
			char deviceName = line[0];
			DeviceControl('d', deviceName, 'i');
		}
	}
	if (policyControl) {
		string line;
		while (getline(policyControl, line))  // line中不包括每行的换行符
		{
			char deviceName = line[0];
			policy = deviceName - '0';
		}
	}
	policyControl.close();
	addDevice.close();
	deleteDevice.close();
	ofstream a("./OS_UI/commute/add_device.txt", ios::out);
	ofstream b("./OS_UI/commute/delete_device.txt", ios::out);

};
void testForProcess() {
	vector<string> file;
	file.push_back("1.txt");
	file.push_back("2.txt");
	file.push_back("3.txt");
	while (Timer != 100) {
		getOption();
		Execute();
		//短期调度
		Sleep(1000);
		/*if (Timer == 8) {
			policy = SCHED_RR;
		}
		if (Timer == 20) {
			policy = SCHED_FCFS;
		}*/
		if (!file.empty()) {
			for (int i = 0; i < file.size(); i++) {
				LongTermScheduler(file[i]);
			}
			file.clear();
		}
		handleMemory();
		updateTaskState();
		checkState();
		if (policy == SCHED_PRO) {
			checkPrority();
		}
		Timer++;
	}
}

void testForMidTermSchedule() {
	vector<string> file;
	file.push_back("4.txt");
	file.push_back("5.txt");
	while (Timer != 100) {
		getOption();
		Execute();
		//短期调度
		Sleep(1000);
		if (Timer == 8) {
			file.push_back("6.txt");
		}
		if (!file.empty()) {
			for (int i = 0; i < file.size(); i++) {
				LongTermScheduler(file[i]);
			}
			file.clear();
		}
		handleMemory();
		updateTaskState();
		checkState();
		if (policy == SCHED_PRO) {
			checkPrority();
		}
		Timer++;
	}
}

void testForFile() {
	//fs.test();
	vector<string> file;
	file.push_back("7.txt");
	file.push_back("8.txt");
	while (Timer != 50) {
		getOption();
		Execute();
		//短期调度
		Sleep(1000);
		if (!file.empty()) {
			for (int i = 0; i < file.size(); i++) {
				LongTermScheduler(file[i]);
			}
			file.clear();
		}
		handleMemory();
		updateTaskState();
		checkState();
		if (policy == SCHED_PRO) {
			checkPrority();
		}
		fs.printDir();
		fs.sendExternalStorageStatusToUI();
		Timer++;
	}
}
void testForDevice() {
	vector<string> file;
	file.push_back("9.txt");
	file.push_back("10.txt");
	while (Timer != 50) {
		getOption();
		Execute();
		//短期调度
		Sleep(1000);
		if (!file.empty()) {
			for (int i = 0; i < file.size(); i++) {
				LongTermScheduler(file[i]);
			}
			file.clear();
		}
		handleMemory();
		updateTaskState();
		checkState();
		if (policy == SCHED_PRO) {
			checkPrority();
		}
		fs.printDir();
		fs.sendExternalStorageStatusToUI();
		Timer++;
	}



}





int main(void) {
	init();
	initDC();
	
	//testForProcess();
	//testForMidTermSchedule();
	//testForFile();
	testForDevice();
}

/*
-李志毅
1.长期调度，检测file向量，如果不为空的话，需要对每个file(这里即硬盘中的作业)创建进程，每次时间片更新都要检测一下
是否能创建新进程，新进程创建会申请内存块，若申请内存失败则需要调用中期调度进行相关的移进和移出

2.创建进程并资源分配完成并转到就绪队列在一个时间片完成，若就绪进程被执行(转入执行状态)，时间需从下个时间片开始计算，即就绪进程最早下个时间片开始运行

3.因为使用的不是链表和指针，因此更新各个队列中的状态需要遍历并查找相应的pid进行更新

4.默认调度方式都是FIFO，设定不同的CPU调度目前暂未实现

5.若执行命令遇到打印机输出、键盘输入等事件，则都将进程的状态设置为阻塞状态，加入阻塞队列同时加入对应队列的等待队列

6.每个时间片更新时在Update函数中，通过比对当前时间PC-PCB中标识的占用键盘开始时间(占用打印机开始时间)来判断是否已执行完成，并进行相应的队列移进和移出操作

7.目前待做的：中期调度函数MidTermScheduler()，CPU调度中的其他策略(优先级)，各种除了计算事件外的别的事件的处理(我写了一个键盘输入的例子)

- 陈硕
新增：
1.PCB项：是否分配内存，分配前阻塞类型，用于类比保存现场；一些结点删除函数delet()；checkprority()等吧

2.中期调度，当阻塞进程大于某值之后进行中期调度，当内存剩余空间较大且足够容纳调出的进程时便进行中期调度，优先调度优先级高的

3.中期调度目前只有键盘，在调出前保存剩余键盘时间，调入后恢复

4.短期调度是动态优先级，运行一次优先级-3，就绪一次优先级+1

5.注释掉的代码是有bug的屎，但还没去删

6.如果有奇奇怪怪的输出内容，可能是有的调试输出忘记注释掉了

7.少任务跑起来没啥bug，但感觉挺乱的

8.之前的代码因为任务少没发现bug，害得我找了半天（恼

9.因为乱七八糟的逻辑写了一堆无用代码，又删了不少；但实际上代码不多555
*/


/*
- 李志毅
目前待补充内容与bug修改
1.为了实现与前端交互 我们采用txt文件为中介 在每个PC写入txt结果，前端读取txt文件并展示 后端在每次PC开始时检查命令.txt进行操作
2.为了体现中断的思想，我们需要有一个running->waiting或者block的中断设计(现在的中断其实是融入在了整个代码中 如果能单独拎出一个中断函数 会更直观)
3.修改了部分bug 将中期调度出的进程状态设置成TASK_SUSPEND_BLOCK,同时中期调度从外存调入内存时 进程应该处于就绪状态 而不是直接阻塞
4.体现上下文切换(包含当前命令中执行的指令位置要保存 已计算或占用设备时间要保存)
5.完成除了键盘外的各个设备
*/

/* 2021.5.4更新
- 陈 李
和内存、文件联调，目前进展顺利
*/

/* 2021.5.20更新
- 陈 李
  +新增打印机
  +新增进程同步(自定义信号量，使用是否busy来控制)
*/