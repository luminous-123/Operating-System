/*
	读者-写者问题，
	读-读允许：可以有多个读者同时读取
	读-写互斥：在执行写操作时，其他读者必须退出
	写-写互斥：只能有一个进程进行写操作，在执行写操作前其他写者和读者都需要退出
	引入read_count对读进程进行计数，当读进程为0时，允许释放一个写进程
	mutex用于对read_count操作的互斥信号量
	W为写操作的互斥信号量
*/
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include <fstream>

#define INTE_PER_SEC 100 //每秒时钟中断的数目
#define MAX_THREAD_NUM 64
#define WRITER 'W'
#define READER 'R'

CRITICAL_SECTION rmutex,w; //互斥信号量
int read_count = 0;

using namespace std;

struct ThreadInfo
{
	int id; //线程标号
	char type; //线程类型
	double opTime; //线程延迟时间
	double lastTime;//读写持续时间
};

void WriteRead(const char* file);
void Writer(void *p);
void Reader(void *p);

int main(int argc, char* argv[]) {
	WriteRead("rw_data.txt");
	return 0;
}

void WriteRead(const char* file) {
	DWORD n_thread = 0; //线程数目
	DWORD thread_ID[MAX_THREAD_NUM]; //线程ID

	//线程对象的数组
	HANDLE h_thread[MAX_THREAD_NUM];
	ThreadInfo thread_info[MAX_THREAD_NUM];

	ifstream inFile; //读取文件中的各进程关系
	inFile.open(file);
	puts("Read Data File \n");
	while (inFile) {
		inFile >> thread_info[n_thread].id;
		inFile >> thread_info[n_thread].type;
		inFile >> thread_info[n_thread].opTime;
		inFile >> thread_info[n_thread].lastTime;
		n_thread++;
		inFile.get();
	}
	//n_thread--;
	for (int i = 0; i < (int)(n_thread); i++) {
		if (thread_info[i].type == WRITER)
		{
			h_thread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(Writer), &thread_info[i], 0, &thread_ID[i]); //Write为线程执行体的开始地址即执行的函数，利用&thread_info[i]给线程传参
		}//创建写进程
		else {
			if (thread_info[i].type == READER) {
				h_thread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(Reader), &thread_info[i], 0, &thread_ID[i]);
			}//创建读进程
			else {
				puts("Bad Data \n");
				exit(0);
			}
		}
	}
	//初始化临界区对象
	InitializeCriticalSection(&rmutex);
	InitializeCriticalSection(&w);

	WaitForMultipleObjects(n_thread, h_thread, TRUE, -1); //当所有线程都完成后发出信号，表示完成
	printf("Task is Finished!\n");
	_getch();
}

void Writer(void* p) {
	int id = ((ThreadInfo*)(p))->id;
	int opTime = (DWORD)(((ThreadInfo*)(p))->opTime * INTE_PER_SEC);
	int lastTime = (DWORD)(((ThreadInfo*)(p))->lastTime * INTE_PER_SEC);
	int count = 0;
	while (count<10)
	{
		Sleep(opTime); //延迟等待
		printf("Thread %d: waiting to write\n",id);
		//进入写操作临界区
		EnterCriticalSection(&w); 
		printf("Thread %d: start writing\n",id);
		Sleep(lastTime);
		printf("Thread %d: end writing\n",id);
		//释放临界资源
		LeaveCriticalSection(&w); 
		count++;
	}
}

void Reader(void* p) {
	int id = ((ThreadInfo*)(p))->id;
	int opTime = (DWORD)(((ThreadInfo*)(p))->opTime * INTE_PER_SEC);
	int lastTime = (DWORD)(((ThreadInfo*)(p))->lastTime * INTE_PER_SEC);
	int count = 0;
	while (count<13) {
		Sleep(opTime);
		printf("Thread %d: waiting to read\n", id);
		EnterCriticalSection(&rmutex);
		read_count++;
		if (read_count == 1) { //如果这是第一个读进程，需要将资源进行上锁，防止写入
			EnterCriticalSection(&w);
		}
		LeaveCriticalSection(&rmutex);
		printf("Thread %d: start reading\n", id);
		Sleep(lastTime);
		printf("Thread %d: end reading\n", id);
		EnterCriticalSection(&rmutex);
		read_count--;
		if (read_count == 0) { //如果这是最后一个读进程则释放资源，可以进行写操作
			LeaveCriticalSection(&w);
		}
		LeaveCriticalSection(&rmutex);
		count++;
	}
}