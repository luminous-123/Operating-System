# 实验二 作业调度实验

## 实验目的

1. 模拟作业调度算法，学习作业在操作系统中的调度过程，加深对作业管理的理解
2. 培养学生程序设计的方法和技巧。

## 实验内容

1. 本实验模拟单处理器系统的作业调度，加深对作业调度算法的理解。用某种语言编程实现先来先服务、短作业优先和最高响应比优先算法。有一些简单的界面，能够运行，仿真操作系统中作业调度的原理和过程。

2. 输出为一组作业调度信息和统计信息。 加权周转时间等。

   作业调度信息包括：

   作业编号 作业名称 提交时间 要求服务运行时间 开始时间 完成时间 等待时间 周转时间

   统计信息包括：平均周转时间、平均带权周转时间

## 实验过程

基本结构

```c
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
```



1. 先来先服务

按照到达时间的顺序构造就绪队列，CPU从就绪队列中取作业依次执行

```C
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
```

2. 非抢占式短作业优先
   1. 判断作业到达的时间，到达时间早的先开始，到达时间一样，运行时间短的先开始
   2. 当前一个作业完成后，判断运行时间，将运行时间最短的优先运行

```C
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
```

3. 高响应比优先策略

依次计算每个作业当前的响应比，找到响应比最大的，放到就绪队列的最前面。

```C
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
```

