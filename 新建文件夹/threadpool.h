#ifndef THREADPOOL_H
#define THREADPOOL_H

#include<functional>
#include<thread>
#include<vector>
#include<queue>
#include<mutex>
#include<iostream>
#include<sstream>
#include<condition_variable>
#include<future>

#include"datatype.h"

using namespace std::tr1;

namespace mthread {

// 任务优先级
typedef function<void()> Task;
typedef std::queue<Task> Tasks;
typedef std::vector<std::thread*> Threads;
typedef std::condition_variable cond;
typedef unordered_set<string> RunningThreads;
typedef unsigned int ThreadSize;





// 初始线程数

class ThreadPool
{

public:

    static struct ThreadPoolInfo{

         unsigned int ThreadPoolId = 0;
         // 活动线程
         unordered_set<string> activeThreads;
         // 所有线程池的ID表, 状态, 不包含main线程ID
         unordered_map<string, ThreadPool*> threadIdTable;

    } TPI;

    static mutex logmtx;

    // 主线程条件变量
    cond master_cond;

    // 主线程的锁
    mutex master_mutex;

    ThreadPool(string name, ThreadSize kInitThreadsSize=2, ThreadSize maxThreadsSize=4,  bool verbose=true)
        :kInitThreadsSize(kInitThreadsSize),maxThreadsSize(maxThreadsSize),name(name),verbose(verbose),m_isStarted(false){
        this->id = TPI.ThreadPoolId;
        TPI.ThreadPoolId++;
    }

    ~ThreadPool(){
        // 如果运行
        if(m_isStarted)stop();
    }


    void produceThread();

    void start();

    void stop();

    template<typename T, typename... Args>
    void addTask(void (T::*f)(Args...), T* obj, Args&&... args){
        // 加互斥锁
        std::unique_lock<std::mutex> lock(m_mutex);
        // 创建任务, 将右值转发给左值类型

        if(activaThreadSize>=currentThreadSize){
             if(activaThreadSize<maxThreadsSize){
                 produceThread();
             }else{
                 string threadID = getThreadId();
                 if(TPI.activeThreads.find(threadID)!=TPI.activeThreads.end()){
                     // 主线程池活动线程状态
                     TPI.activeThreads.erase(threadID);
                     TPI.threadIdTable[threadID]->activaThreadSize--;
                     TPI.threadIdTable[threadID]->hangThreadSize++;
                     if(verbose)log("is busy, currenThreadSize="+to_string(currentThreadSize)+"|activaYhreadSize="+to_string(activaThreadSize)+
                                    ", make "+TPI.threadIdTable[threadID]->name+ "-ThreadPool thread-"+threadID+" wait"+
                             "(currenThreadSize="+to_string(TPI.threadIdTable[threadID]->currentThreadSize)+"|activaYhreadSize="+to_string(TPI.threadIdTable[threadID]->activaThreadSize)+")", 1);
                 }else{
                     if(verbose)log("is busy, currenThreadSize="+to_string(currentThreadSize)+"|activaYhreadSize="+to_string(activaThreadSize)+
                                    ", make master thread-"+threadID+" wait", 1);
                 }
                 master_cond.wait(lock);
                 if(TPI.activeThreads.find(threadID) ==TPI.activeThreads.end()){
                     TPI.threadIdTable[threadID]->activaThreadSize++;
                     TPI.threadIdTable[threadID]->hangThreadSize--;
                     TPI.activeThreads.insert(threadID);
                     if(verbose)log("is free, currenThreadSize="+to_string(currentThreadSize)+"|activaYhreadSize="+to_string(activaThreadSize)+
                                    ", make "+TPI.threadIdTable[threadID]->name+ "-ThreadPool thread-"+threadID+" wake up"+
                             "(currenThreadSize="+to_string(TPI.threadIdTable[threadID]->currentThreadSize)+"|activaYhreadSize="+to_string(TPI.threadIdTable[threadID]->activaThreadSize)+")", 1);
                 }else{
                     if(verbose)log("is free, currenThreadSize="+to_string(currentThreadSize)+"|activaYhreadSize="+to_string(activaThreadSize)+
                                    ", make master thread-"+threadID+" wake up", 1);
                    }
                 }
        }

        Task task = std::bind(f, obj, forward<Args>(args)...);
        m_tasks.push(task);
        m_cond.notify_one();
    }

    string getThreadId();

    unsigned int getThreadPoolId();

    Tasks getTaskQueue();

    ThreadSize getActiveThreadsSize();

    ThreadSize  getMaxThreadsSize();

    ThreadSize getHangThreaSize();

    void log(std::string msg, bool isThreadPool=false);

private:


    // 核心线程数
    ThreadSize kInitThreadsSize;

    // 当前线程数
    ThreadSize currentThreadSize = 0;

    // 最大线程数
    ThreadSize maxThreadsSize;

    // 活动线程数
    ThreadSize activaThreadSize = 0;

    // 抑制线程数
    ThreadSize hangThreadSize = 0;


    // ID号
    unsigned int id;

    string name;

    bool verbose;

    // 线程队列
    Threads m_threads;

    // 任务队列
    Tasks m_tasks;

    // 互斥锁
    std::mutex m_mutex;

    // 当多线程下的条件变量, 该线程池的信号量
    cond m_cond;

    // 是否开始
    atomic<bool> m_isStarted{ false };

    ThreadPool(const ThreadPool&);    //禁止复制拷贝.

    const ThreadPool& operator=(const ThreadPool&);  //赋值函数

    // 线程循环
    void threadLoop();

    // 获取任务
    Task take();

};



}

#endif // THREADPOOL_H
