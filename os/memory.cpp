/*
 * @Author: your name
 * @Date: 2021-05-04 21:13:50
 * @LastEditTime: 2021-05-21 21:45:44
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \undefinedd:\L\Download\WeChat Files\wxid_g2c0iua6s6c222\FileStorage\File\2021-05\MEM Manage.cpp
 */

 //#include "memory.h"
#include "global.h"
#include "FileSystem.h"
int occupied_size; //内存中被占用帧的数目
process_info process_list[PROCESS_NUM];
frame_unit frame[PHYSICAL_SIZE];

void initMemory() //初始化内存，可以加在系统执行的init中
{
	srand(time(NULL));
	for (int i = 0; i < PROCESS_NUM; ++i)
	{
		process_list[i].pid = -1;
		process_list[i].alive = 0;
	}
	for (int i = 0; i < PHYSICAL_SIZE; ++i)
	{
		frame[i].pid = -1;
		frame[i].ifUse = 0;
	}
	occupied_size = 0;
}

void replacement(struct process_info &p) //可以在cmd输出调页的过程？
{
	if (p.occupied_frames < FRAMES_PER_PROCESS) //有空闲frame直接调入并更新lru数组
	{
		//swapToMemory()
		p.lru[p.occupied_frames] = -1;
		p.page_table[p.sequence[p.current_pointer]].frame_no = p.frames_number[p.occupied_frames];
		p.page_table[p.sequence[p.current_pointer]].valid = 1;
		for (int k = 0; k < p.occupied_frames; k++)
		{
			p.lru[k]++;
		}
		p.occupied_frames++;
	}
	else //没有空闲frame
	{
		int max = -9999;
		int loc_max;
		for (int k = 0; k < p.occupied_frames; k++) //找到当前lru数组中值最大的元素即最少使用
		{
			if (p.lru[k] > max)
			{
				max = p.lru[k];
				loc_max = k;
			}
		}
		for (int i = 0; i < p.size; i++)
		{
			if (p.page_table[i].frame_no == p.frames_number[loc_max] && p.page_table[i].valid == 1)
			{
				p.page_table[i].valid = -1;
			}
		}
		p.page_table[p.sequence[p.current_pointer]].frame_no = p.frames_number[loc_max];
		p.page_table[p.sequence[p.current_pointer]].valid = 1;
		p.lru[loc_max] = -1;
		for (int k = 0; k < p.occupied_frames; k++)
		{
			p.lru[k]++;
		}
	}
}

bool applyForMemory(PCB &p)				   //进程内存接口 传一个pcb过来,返回是否分配内存成功
{										   // pid 进程pid， size 进程所需页数
	if (occupied_size <= PHYSICAL_SIZE - 3) //如果有足够的的帧，分配内存
	{
		for (int i = 0; i < PROCESS_NUM; ++i) //是否达到进程上限
		{
			if (process_list[i].alive == 0) //初始化内存中进程信息表
			{
				//pid size 信息
				process_info process;
				process.pid = p.pid;
				process.size = p.mm->task_size;
				//初始化进程页表
				for (int j = 0; j < TABLE_LEN; ++j)
				{
					process.page_table[j].frame_no = -1;
					process.page_table[j].valid = -1;
				}
				// 初始化访问顺序
				process.sequence[0] = 0; //第一次访问必然是逻辑0页
				for (int j = 1; j < SEQ_LEN; ++j)
				{
					if (rand() % 10 < 2) //百分之二十的概率访问下一页
					{
						process.sequence[j] = (process.sequence[j - 1] + 1) % process.size;
					}
					else if (rand() % 10 > 8) //百分之十的概率跳转到某一页
					{
						process.sequence[j] = rand() % process.size;
					}
					else //百分之二十概率继续执行当前页
					{
						process.sequence[j] = process.sequence[j - 1];
					}
				}
				process.current_pointer = 0;
				// 初始化内存调度 和 生存信息
				process.occupied_frames = 0;
				process.alive = 1;
				for (int j = 0; j < PHYSICAL_SIZE; ++j)
				{
					if (frame[j].ifUse == 0)
					{
						for (int k = j, l = 0; k < j + FRAMES_PER_PROCESS; ++k, ++l)
						{
							frame[k].pid = process.pid;
							frame[k].ifUse = 1;
							process.frames_number[l] = k;
							cout << k << " ";
						}
						break;
					}
				}
				for (int j = 0; j < FRAMES_PER_PROCESS; ++j)
				{
					process.lru[j] = 0;
				}
				process_list[i] = process;
				occupied_size += FRAMES_PER_PROCESS;
				cout << "process_list:" << i << " framenumber:";
				for (int j = 0; j < FRAMES_PER_PROCESS; ++j)
				{
					cout << process_list[i].frames_number[j] << " ";
				}
				// handle_process(process); //直接处理调页过程
				return true;
			}
		}
		return false; //进程数目达到上限，分配失败
	}
	else
	{
		return false; //帧都正在被占用，分配失败
	}
}

struct AllocateMsg allocateSuccess(PCB &p) //内存分配成功，返回该进程所分配的内存号
{
	AllocateMsg temp;

	/*for (int i = 0; i < PHYSICAL_SIZE; ++i)
	{
		cout << "FRAME:";
		cout << frame[i].pid << " "<<endl;
	}
	cout << endl;*/
	for (int i = 0; i < PROCESS_NUM; ++i)
	{
		if (process_list[i].pid == p.pid)
		{
			temp.pid = p.pid;
			for (int j = 0; j < FRAMES_PER_PROCESS; ++j)
			{
				temp.frameNumber[j] = process_list[i].frames_number[j];
				cout << temp.frameNumber[j] << " ";
			}
			break;
		}
	}
	/*for (int i = 0; i < PHYSICAL_SIZE; ++i)
	{
		cout << p.pid << endl;
		if (frame[i].pid == p.pid)
		{
			cout << "fff:" << frame[i].pid << endl;
			temp.pid = p.pid;
			for (int l = 0, j = i; j < i + FRAMES_PER_PROCESS; ++j)
			{
				temp.frameNumber[l] = j;
			}
			break;
		}
	}*/
	return temp;
}

bool handle_process(process_info &p) //处理调页过程，每个PC执行一次,判断当前所处理进程的调页
{
	cout << "pid:";
	cout << p.pid << endl;
	cout << "page_table_frameNO:" << p.page_table[0].frame_no << endl;
	cout << "page_table_valid:" << p.page_table[0].valid << endl;
	bool pageFault = false;
	if (p.page_table[p.sequence[p.current_pointer]].valid == -1) //如果进程所需页不在frame中，调页
	{
		pageFault = true;
		replacement(p);
	}
	else
	{
		int tempNo = p.page_table[p.sequence[p.current_pointer]].frame_no;
		for (int i = 0; i < FRAMES_PER_PROCESS; ++i)
		{
			if (tempNo == p.frames_number[i])
			{
				p.lru[i] = -1;
			}
		}
	}
	p.current_pointer++;
	return pageFault;
}

void releaseMemory(PCB &p) // 进程内存接口传一个pcb过来，将该进程在内存中释放
{
	cout << "释放成功:pid = " << p.pid << endl;
	for (int i = 0; i < PROCESS_NUM; ++i)
	{
		if (process_list[i].pid == p.pid)
		{
			process_list[i].alive = 0;
			for (int j = 0; j < FRAMES_PER_PROCESS; ++j)
			{
				cout << "[i]:" << i << endl;
				cout << "process_list[i].frames_number[j]:" << process_list[i].frames_number[j] << endl;
				frame[process_list[i].frames_number[j]].ifUse = 0;
			}
			occupied_size -= FRAMES_PER_PROCESS;
		}
	}
}

void medium_swap_out(int pid_out)
{
	extern FileSystem fs;
	int i;
	for (i = 0; i < PROCESS_NUM; ++i)
	{
		if (process_list[i].pid == pid_out)
		{
			process_list[i].alive = 0;
			for (int j = 0; j < FRAMES_PER_PROCESS; ++j)
			{
				frame[process_list[i].frames_number[j]].ifUse = 0;
			}
			occupied_size -= FRAMES_PER_PROCESS;
			fs.swapToExternalStorage(pid_out, FRAMES_PER_PROCESS);
		}
	}
}
int getSize() {
	return PHYSICAL_SIZE - occupied_size;
}

bool medium_swap_in(PCB &p) //frame重新分配
{
	// int newFlag = 0; // 0 代表该进程在挂起状态
	for (int i = 0; i < PROCESS_NUM; i++)
	{
		if (process_list[i].pid == p.pid)
		{
			process_list[i].alive = 1;
			for (int j = 0; j < PHYSICAL_SIZE; ++j)
			{
				if (frame[j].ifUse == 0)
				{
					for (int k = j, l = 0; k < j + FRAMES_PER_PROCESS; ++k, ++l)
					{
						frame[k].pid = process_list[i].pid;
						frame[k].ifUse = 1;
						process_list[i].frames_number[l] = k;
						cout << k << " ";
					}
					break;
				}
			}
			for (int j = 0; j < FRAMES_PER_PROCESS; ++j)
			{
				process_list[i].lru[j] = 0;
			}
			occupied_size += FRAMES_PER_PROCESS;
			cout << "process_list:" << i << " framenumber:";
			for (int j = 0; j < FRAMES_PER_PROCESS; ++j)
			{
				cout << process_list[i].frames_number[j] << " ";
			}
			return true; //调入成功
		}
	}
	// 全新进程被调入 直接请求分配内存
	int flag = applyForMemory(p);
	if (flag == 0)
		return false;
	return true;
}

// void medium_swap(int pid_in, int pid_out)
// {
// 	//swapToExternalStorage(pid_out, FRAMES_PER_PROCESS);

// 	//删除页表,取消内存标记
// 	int i;
// 	for (i = 0; i < PROCESS_NUM; ++i)
// 	{
// 		if (process_list[i].pid == pid_out)
// 		{
// 			process_list[i].alive = 0;
// 			for (int j = 0; j < FRAMES_PER_PROCESS; ++j)
// 			{
// 				frame[process_list[i].frames_number[j]].ifUse = 0;
// 			}
// 			occupied_size -= FRAMES_PER_PROCESS;
// 		}
// 	}

// 	//swapToMemory(pid_in, FRAMES_PER_PROCESS);

// 	//创建新页表 ，标记内存占用
// 	process_list[i].alive = 1;
// 	process_list[i].pid = pid_in;
// 	for (int j = 0; j < FRAMES_PER_PROCESS; ++j)
// 	{
// 		frame[process_list[i].frames_number[j]].ifUse = 1;
// 	}
// 	occupied_size += 3;
// }

void printMemoryInfo()
{
	// OS_UI/commute
	// TOTALSIZE usage
	// INDEX PID 页号
	FILE *stream1;
	string fullPath = "./OS_UI/commute/frame.txt";

	//extern int Timer;
	//fullPath.append(to_string(Timer)+".txt");
	//cout << fullPath << endl;
	ofstream f(fullPath, ios::out);

	//ofstream f("./OS_UI/commute/frame.txt", ios::out);
	int totalSize = PHYSICAL_SIZE;
	double usage = 0;
	for (int i = 0; i < PHYSICAL_SIZE; ++i)
	{
		if (frame[i].ifUse == 1)
		{
			usage += 1;
		}
	}

	usage /= PHYSICAL_SIZE;
	f << PHYSICAL_SIZE << " " << usage * 100 << "%" << endl;

	for (int i = 0; i < PHYSICAL_SIZE; ++i)
	{
		if (frame[i].ifUse == 1)
		{
			//f << i << " " << frame[i].pid << " ";
			int flag = 0;
			for (int j = 0; j < PROCESS_NUM; ++j)
			{
				for (int k = 0; k < TABLE_LEN; ++k)
				{
					//
					if (process_list[j].page_table[k].valid == 1 && process_list[j].page_table[k].frame_no == i)
					{
						f << i << " " << frame[i].pid << " ";
						f << k << endl;
						flag = 1;
						break;
					}
				}
			}
			if (flag == 0)
			{
				f << i << " " << frame[i].pid << " ";
				f << -1 << endl;
			}
			//flag = 0;
		}
		else
		{
			f << i << " " << -1 << " "
				<< -1 << endl;
		}
	}

	/*for (int i = 0; i < PHYSICAL_SIZE; ++i)
	{
		if (frame[i].ifUse == 1)
		{
			f << i << " " << frame[i].pid << " ";
		}
		else
		{
			f << i << " " << -1 << endl;
		}
	}*/
	f.close();
}

// 回收