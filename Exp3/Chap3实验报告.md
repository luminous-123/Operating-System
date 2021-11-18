# 实验三 银行家算法的仿真

## 实验目的

1. 理解死锁的概念和消除方法；


## 实验内容

1. 编程实现银行家算法进行资源分配以避免死锁的发生。

## 代码思路

设置总的资源向量、最大需求矩阵、已分配矩阵、请求矩阵、当前可用资源、判断安全时动态可用资源的向量、可完成标志向量等数据结构，然后在此基础上设计实现银行家算法，即判断如果进行某种分配，是否安全的算法。

## 实验过程

1. 安全性算法

需要设置`Work=Available`来确定在预分配过程中资源的可分配数量，假设每次分配都是分配给各进程所剩余的所有资源，因此在分配后`Work[k]=Allocation[i][k]`，记录下此时所分配给的进程pid，依次类推，最后确定是否所有进程的所有资源都得到满足，如果在某一次分配资源的过程中遍历所有的进程都不能分配资源，说明此时系统处于不安全状态

```C
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
```

2. 资源分配过程

首先判断分配的资源数是否超过此时可用资源数和该进程所声称的所需最大资源数。如果满足，就对其进行预分配，在预分配完成后，进行安全性检查，如果系统处于不安全状态则收回所预分配的资源

```C
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
```

![image-20211118204138744](D:\Learning\Semester5\操作系统\Chap3\Chap3实验报告.assets\image-20211118204138744.png)

