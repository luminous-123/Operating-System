#include <iostream>
using namespace std;
#define resourceNum 3 //ϵͳ��Դ��
#define processNum 5  //ϵͳ������

//ϵͳʣ�������Դ��
int available[resourceNum] = { 3,3,2 };
//���̵��������
int MaxRequest[processNum][resourceNum] = {
	{7,5,3},
	{3,2,2},
	{9,0,2},
	{2,2,2},
	{4,3,3}
};
//�����Ѿ��������Դ
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
//�������״̬
bool Finish[processNum] = { false };

void showInfo() {
	cout << "\n------------------------------------------------------------------------------------\n";
	cout << "��ǰϵͳ������Դʣ��:" << endl;
	for (int j = 0; j < resourceNum; j++) {
		cout << available[j] << " ";

	}
	cout << "\n\n��ǰϵͳ��Դ���:" << endl;
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
//�ж�һ�������Ƿ�������Դ���õ�����
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

//��ȫ���㷨
bool isSafe() {
	int Work[resourceNum];
	int allFinish = 0; //��Դȫ���õ����������

	//��ִ�а�ȫ�㷨��ʼʱ��Work = Available��
	for (int i = 0; i < resourceNum; i++) {
		Work[i] =available[i];
	}
	//Ԥ��Finish
	for (int i = 0; i < processNum; i++) {
		if (isAllZero(i)) {
			Finish[i] = true;
			allFinish++;
		}
		else {
			Finish[i] = false;
		}
	}
	//��ʼԤ���䣬��Ѱ�Ұ�ȫ����
	int safeIndex = 0; //��ȫ���ж�Ӧ��λ��
	int safe[processNum]; //���氲ȫ����
	int current = 0;  //Ŀǰ���ڵ�PID
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
			cout << "��ǰϵͳ���ڲ���ȫ״̬" << endl;
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
	//��ӡ��ȫ����
	cout << "\n��ǰϵͳ���ڰ�ȫ״̬��\n��ȫ����Ϊ��";
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
			cout << "��Ҫ�������Դ������Ŀǰ������Դ���򳬹������Ƶ����������Դ��" << endl;
			break;
		}
	}
	if (num == resourceNum) {
		num = 0;
		//Ԥ������Դ
		for (int j = 0; j < resourceNum; j++) {
			available[j] = available[j] - ask[j];
			Allocation[request][j] += ask[j];
			Need[request][j] -= ask[j];
			//�鿴��Դ�����᲻��������������Դ���������������Դ������ʱ���ͷ�����ӵ�еĵ�������Դ
			if (Need[request][j] == 0) {
				num++;
			}
		}
		if (num == resourceNum) {
			for (int i = 0; i < resourceNum; i++) {
				available[i] += Allocation[request][i];
			}
			cout << "\n���η������P" << request << "��ɣ��ý���ռ�õ�������Դ���ͷ����" << endl;
		}
		else {
			cout << "\n���η������P" << request << "δ���" << endl;
		}
		showInfo();
		cout << "\n��ǰϵͳ��ȫ���������" << endl;
		cout << " PID\t Work\t\tAllocation\t Need\t\t Work+Allocation" << endl;
		//Ԥ������ɺ��ж�ϵͳ�Ƿ��ڰ�ȫ״̬����������ڷ���Ԥ�������Դ�������ȫ�����
		if (!isSafe()) {
			for (int j = 0; j < resourceNum; j++) {
				available[j] += ask[j];
				Allocation[request][j] -= ask[j];
				Need[request][j] += ask[j];
			}
			cout << "��Դ���㣬����ʧ�ܣ�" << endl;
		}
	}
}
int main() {
	int request = 0;
	int ask[resourceNum];
	showInfo();
	cout << "\n��ǰϵͳ��ȫ���������" << endl;
	cout << " PID\t Work\t\tAllocation\t Need\t\t Work+Allocation" << endl;
	bool issafe = isSafe();
	char con;
	while(isSafe){
		do {
			cout << "������Ҫ����Ľ��̣�";
			cin >> request;
		} while (request > processNum);
		cout << endl;
		for (int i = 0; i < resourceNum; i++) {
			cout << "������ҪΪ����" << request << "����ĵ�" << i+1 << "����Դ��";
			cin >> ask[i];
			cout << endl;
		}
		Request(request, ask);
		cout << "\n�Ƿ��������(Y/n)��";
		cin >> con;
		if (con == 'n' || con == 'N') {
			exit(0);
		}
	}
}