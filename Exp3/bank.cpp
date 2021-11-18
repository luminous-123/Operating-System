#include <iostream>
using namespace std;
#define resourceNum 3 //系统资源数
#define processNum 5  //系统进程数

//系统剩余可用资源数
int available[resourceNum] = { 3,3,2 };
//进程的最大需求
int MaxRequest[processNum][resourceNum] = {
	{7,5,3},
	{3,2,2},
	{9,0,2},
	{2,2,2},
	{4,3,3}
};
//程序已经分配的资源
int Allocation[processNum][resourceNum] = {
	{0,1,0},
	{2,0,0},
	{3,0,2},
	{2,1,1},
	{0,0,2}
};
int Need[processNum][resourceNum] = {
	{7,4,3},
	{1,2,2},
	{6,0,0},
	{0,1,1},
	{4,3,1}
};
//进程完成状态
bool Finish[processNum] = { false };

void showInfo() {
	cout << "\n------------------------------------------------------------------------------------\n";
	cout << "当前系统各类资源剩余:" << endl;
	for (int j = 0; j < resourceNum; j++) {
		cout << available[j] << " ";

	}
	cout << "\n\n当前系统资源情况:" << endl;
	cout << " PID\t Max\t\tAllocation\t Need" << endl;
	for (int i = 0; i < processNum; i++) {
		cout << "P" << i << "\t";
		for (int j = 0; j < resourceNum; j++) {
			printf("%2d", MaxRequest[i][j]);
		}
		cout << "\t\t";
		for (int j = 0; j < resourceNum; j++) {
			printf("%2d", Allocation[i][j]);
		}
		cout << "\t\t";
		for (int j = 0; j < resourceNum; j++) {
			printf("%2d", Need[i][j]);
		}
		cout << endl;
	}
}
//判断一个进程是否所有资源都得到满足
bool isAllZero(int process) {
	int num = 0;
	for (int i = 0; i < resourceNum; i++) {
		if (Need[process][i] == 0) {
			num++;
		}
	}
	if (num == resourceNum) return true;
	else return false;
}

void ShowSafeInfo(int *work, int current) {
	cout << " P" << current << "\t";
	for (int i = 0; i < resourceNum; i++) {
		printf("%2d", work[i]);
	}
	cout << "\t\t";
	for (int i = 0; i < resourceNum; i++) {
		printf("%2d", Allocation[current][i]);
	}
	printf("\t\t");
	for (int j = 0; j < resourceNum; j++)
	{
		printf("%2d", Need[current][j]);
	}
	printf("\t\t");
	for (int j = 0; j < resourceNum; j++)
	{
		printf("%2d", Allocation[current][j] + work[j]);
	}
	printf("\n");
}

//安全性算法
bool isSafe() {
	int Work[resourceNum];
	int allFinish = 0; //资源全部得到满足的数量

	//在执行安全算法开始时，Work = Available；
	for (int i = 0; i < resourceNum; i++) {
		Work[i] =available[i];
	}
	//预置Finish
	for (int i = 0; i < processNum; i++) {
		if (isAllZero(i)) {
			Finish[i] = true;
			allFinish++;
		}
		else {
			Finish[i] = false;
		}
	}
	//开始预分配，即寻找安全序列
	int safeIndex = 0; //安全序列对应的位置
	int safe[processNum]; //保存安全序列
	int current = 0;  //目前所在的PID
	while (allFinish < processNum) {
		bool flag = false;
		for (int i = 0; i < processNum; i++) {
			if (!Finish[i]) {
				int j = 0;
				for (j = 0; j < resourceNum; j++) {
					if (Need[i][j] > Work[j]) break;
				}
				if (j == resourceNum) {
					for (int k = 0; k < resourceNum; k++) {
						Work[k] +=  Allocation[i][k];
					}
					ShowSafeInfo(Work, i);
					safe[safeIndex++] = i;
					Finish[i] = true;
					allFinish++;
					flag = true;
				}
			}
		}
		if (!flag) {
			cout << "当前系统处于不安全状态" << endl;
			return false;
		}
	}
	int pNum = allFinish;
	for (int i = 0; i < processNum; i++)
	{
		bool result = isAllZero(i);
		if (result)
		{
			pNum--;
		}
	}
	//打印安全序列
	cout << "\n当前系统处于安全状态！\n安全序列为：";
	for (int i = 0; i < pNum; i++) {
		cout << safe[i] << " ";
	}
	cout << endl;
	return true;
}

void Request(int request,int *ask) {
	int num = 0;
	for (int i = 0; i < resourceNum; i++) {
		if (ask[i] <= Need[request][i] && ask[i] <= available[i]) {
			num++;
		}
		else {
			cout << "所要分配的资源数超过目前可用资源数或超过所声称的所需最大资源数" << endl;
			break;
		}
	}
	if (num == resourceNum) {
		num = 0;
		//预分配资源
		for (int j = 0; j < resourceNum; j++) {
			available[j] = available[j] - ask[j];
			Allocation[request][j] += ask[j];
			Need[request][j] -= ask[j];
			//查看资源分配后会不会出现满足最大资源的情况，当所有资源都满足时，就返还所拥有的的所有资源
			if (Need[request][j] == 0) {
				num++;
			}
		}
		if (num == resourceNum) {
			for (int i = 0; i < resourceNum; i++) {
				available[i] += Allocation[request][i];
			}
			cout << "\n本次分配进程P" << request << "完成，该进程占用的所有资源已释放完毕" << endl;
		}
		else {
			cout << "\n本次分配进程P" << request << "未完成" << endl;
		}
		showInfo();
		cout << "\n当前系统安全情况分析：" << endl;
		cout << " PID\t Work\t\tAllocation\t Need\t\t Work+Allocation" << endl;
		//预分配完成后判断系统是否处于安全状态，如果不处于返回预分配的资源，如果安全则继续
		if (!isSafe()) {
			for (int j = 0; j < resourceNum; j++) {
				available[j] += ask[j];
				Allocation[request][j] -= ask[j];
				Need[request][j] += ask[j];
			}
			cout << "资源不足，分配失败！" << endl;
		}
	}
}
int main() {
	int request = 0;
	int ask[resourceNum];
	showInfo();
	cout << "\n当前系统安全情况分析：" << endl;
	cout << " PID\t Work\t\tAllocation\t Need\t\t Work+Allocation" << endl;
	bool issafe = isSafe();
	char con;
	while(isSafe){
		do {
			cout << "请输入要分配的进程：";
			cin >> request;
		} while (request > processNum);
		cout << endl;
		for (int i = 0; i < resourceNum; i++) {
			cout << "请输入要为进程" << request << "分配的第" << i+1 << "类资源：";
			cin >> ask[i];
			cout << endl;
		}
		Request(request, ask);
		cout << "\n是否继续分配(Y/n)：";
		cin >> con;
		if (con == 'n' || con == 'N') {
			exit(0);
		}
	}
}