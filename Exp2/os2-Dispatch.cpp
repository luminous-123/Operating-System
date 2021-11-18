// os2-Dispatch.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <conio.h>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <io.h>
#include <fstream>
#pragma warning(suppress : 4996)
using namespace std;

typedef struct JCB {
    int id; // 进程标识符
    char name[8]; //进程名
    int time_arrive; //作业到达时间
    int time_need;  // 作业要求运行时间
    int time_start; //作业开始时间
    int time_finish; //作业完成时间
    int time_wait; //作业等待时间
    int time_tur; //作业周转时间
    float time_Wtur; //带权周转时间
};
//最大进程数
int const maxnum = 10;
JCB jcbdata[maxnum];

int ready[maxnum]; //就绪队列
int order[maxnum];//排序队列

//实际进程数
int num = 5;

// 停顿时间即运行时间
void _sleep(int n) {
    clock_t goal;
    goal = (clock_t)n * CLOCKS_PER_SEC + clock();
    while (goal > clock());
}

//输入数据
int input() {
    int i = 0;
    ifstream inFile; //读取文件中的各进程关系
    const char* file = "data.txt";
    cout << "请输入作业数量:";
    cin >> num;
    inFile.open(file);
    puts("Read Data File \n");
    while (inFile) {
        inFile >> jcbdata[i].id;
        inFile >> jcbdata[i].name;
        inFile >> jcbdata[i].time_arrive;
        inFile >> jcbdata[i].time_need;
        jcbdata[i].time_finish = 0;
        jcbdata[i].time_start = 0;
        jcbdata[i].time_wait = 0;
        jcbdata[i].time_tur = 0;
        jcbdata[i].time_Wtur = 0;
        i++;
        inFile.get();
    }
    int choice = 0;
    cout << "请输入要选择的作业调度算法:" << endl;
    cout << "(1)先来先服务调度算法" << endl;
    cout << "(2)非抢占式短作业优先调度" << endl;
    cout << "(3)高响应比调度算法" << endl;
    cout << "(4)退出" << endl;
    cin >> choice;
    return choice;
}
//输出作业详细信息
void print(JCB* p) {
    int i = 0;
    printf("*********************************************************************\n");
    printf("ID\t作业名称\t到达时间\t运行时间\t开始时间\t等待时间\t完成时间\t周转时间\t带权周转时间\n");
    for (i = 0; i < num; i++) {
        cout.width(5);
        printf("%d\t%s\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%.3f\n", p[i].id, p[i].name, p[i].time_arrive, p[i].time_need, p[i].time_start, p[i].time_wait, p[i].time_finish, p[i].time_tur, p[i].time_Wtur);
    }
}
void stastic(JCB* p) {
    float AvgTur1;//平均周转时间
    float AvgWTur;//平均带权周转时间
    float t1 = 0;
    float t2 = 0;
    int i = 0;
    for (i = 0; i < num; i++) {
        t1 += p[i].time_tur; //周转时间和
        t2 += p[i].time_Wtur; //带权周转时间和
    }
    AvgTur1 = t1 / num;
    AvgWTur = t2 / num;
    printf("\n平均周转时间：%.3f\t平均带权周转时间：%.3f\n", AvgTur1, AvgWTur);
    printf("*********************************************************************\n");
}

void calc(JCB* data) {
    for (int i = 0; i < num; i++) {
        if (i == 0) {
            data[ready[i]].time_start = data[ready[i]].time_arrive;
        }
        else {
            data[ready[i]].time_start = data[ready[i - 1]].time_finish;
        }
        printf("第%d个任务在运行中\n", i + 1);
        _sleep(data[ready[i]].time_need);
        data[ready[i]].time_wait = data[ready[i]].time_start - data[ready[i]].time_arrive; //等待时间=开始运行时间-到达时间
        data[ready[i]].time_finish = data[ready[i]].time_start + data[ready[i]].time_need; //完成时间=开始时间+运行时间
        data[ready[i]].time_tur = data[ready[i]].time_finish - data[ready[i]].time_arrive; //周转时间=完成时间-到达时间
        data[ready[i]].time_Wtur = (float)data[ready[i]].time_tur / (float)data[ready[i]].time_need; //带权周转时间 = 周转时间/运行时间
    }
    cout << endl;
}

void FCFS(JCB* data) {
    printf("*************************先来先服务调度*******************************\n");
    int temp;
    for (int i = 0; i < num; i++) {
        order[i] = data[i].time_arrive;
        ready[i] = i;
    }
    //使用到达时间进行排序
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num-i-1; j++) {
            if (order[j] > order[j + 1]) {
                temp = order[j];
                order[j] = order[j+1];
                order[j+1] = temp;
                temp = ready[j];
                ready[j] = ready[j+1];
                ready[j+1] = temp;
            }
        }
    }
    cout << endl;
    calc(data);
}
//非抢占式短作业优先调度
/*
1. 判断作业到达的时间，到达时间早的先开始，到达时间一样，运行时间短的先开始
2. 当前一个作业完成后，判断运行时间，将运行时间最短的优先运行
*/
void SJF(JCB* data) {
    printf("**************************非抢占式短作业优先调度**********************\n");
    int temp,min,index=0;
    for (int i = 0; i < num; i++) {
        order[i] = data[i].time_arrive;
        ready[i] = i;
    }
    //使用到达时间进行排序
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num - i - 1; j++) {
            if (order[j] > order[j + 1]) {
                temp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = temp;
                temp = ready[j];
                ready[j] = ready[j + 1];
                ready[j + 1] = temp;
            }
        }
    }
    temp = data[ready[0]].time_arrive;
    bool flag = false;
    int temp1=0;
    for (int i = 0; i < num; i++) {
        temp = temp + data[ready[i]].time_need; //作业完成时间
        min = 999;
        index = 0;
        flag = false;
        for (int j = i + 1; j < num; j++) {
            if (data[ready[j]].time_arrive <= temp) { //如果作业到达时间在前一个作业的完成时间
                if (data[ready[j]].time_need < min) { //比较运行时间，选择最小的保存
                    index = j;
                    min = data[ready[j]].time_need;
                    flag = true;
                }
            }
        }
        //把服务时间最小的放到就绪队列的最前面
        if (flag) {
            temp1 = ready[i + 1];
            ready[i + 1] = ready[index];
            ready[index] = temp1;
        }
    }

    calc(data);
}
void HRN(JCB* data) {
    printf("*********************************高响应比调度*************************\n");
    int last = 0,temp = 0,k = 0;
    //使用到达时间进行排序
    for (int i = 0; i < num; i++) {
        order[i] = data[i].time_arrive;
        ready[i] = i;
    }
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num - i - 1; j++) {
            if (order[j] > order[j + 1]) {
                temp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = temp;
                temp = ready[j];
                ready[j] = ready[j + 1];
                ready[j + 1] = temp;
            }
        }
    }
    for (int i = 0; i < num; i++) {
        cout << ready[i] << " ";
    }
    cout << endl;
    //找最大响应比的放在最前面
    last = data[ready[0]].time_arrive;
    bool flag = false;
    float max = 0, temp2 = 0;
    int wait = 0, index = 0, temp1 = 0;
    while (k != num) {
        last = last + data[ready[k]].time_need;
        flag = false;
        max = 0;
        for (int i = k+1; i < num; i++) {
            if (data[ready[i]].time_arrive < last) { //如果到达时间小于上一个的完成时间
                wait = last - data[ready[i]].time_arrive;
                temp2 = 1 + (float)wait / (float)data[ready[i]].time_need;
                if (temp2 > max) {
                    index = i;
                    max = temp2;
                    flag = true;
                }
            }
        }
        if (flag) {
            temp1 = ready[k + 1];
            ready[k + 1] = ready[index];
            ready[index] = temp1;
        }
        k++;
    }
    for (int i = 0; i < num; i++) {
        cout << ready[i] << " ";
    }
    cout << endl;
    calc(data);
}
int main()
{
    int choice = 0;
    while (true) {
        choice = input();
        switch (choice)
        {
        case 1:
            FCFS(jcbdata);
            break;
        case 2:
            SJF(jcbdata);
            break;
        case 3:
            HRN(jcbdata);
            break;
        case 4:
            exit(0);
            break;
        default:
            cout << "Wrong input!" << endl;
            break;
        }
        print(jcbdata);
        stastic(jcbdata);
    }
}