#ifndef SIMPLTHREADPOOL_H
#define SIMPLTHREADPOOL_H

#include <unordered_set>
#include <mutex>
#include <functional>
#include <iostream>
#include <condition_variable>
#include <queue>
#include <thread>
#include <sstream>
#include <atomic>
#include <assert.h>

using namespace std;

namespace  mthread {

typedef  unsigned int ThreadSize;
typedef  function<void()> Task;
typedef  queue<Task> Tasks;
typedef  vector<thread*> Threads;
typedef  condition_variable Cond;


class simplthreadpool
{


public:

    static mutex logmtx;

    simplthreadpool(string name, ThreadSize kInitThreadsSize=3, bool verbose=true):
        name(name),kInitThreadsSize(kInitThreadsSize),verbose(verbose){}

    ~simplthreadpool(){
        if(m_isStarted)stop();
    }

    void start();

    void stop();

    void threadLoop();

    template<typename T, typename... Args>
    void addTask(void (T::*f)(Args...), T* obj, Args&&... args){
        // 加互斥锁
        std::unique_lock<std::mutex> lock(m_mutex);
        // 创建任务, 将右值转发给左值类型
        Task task = std::bind(f, obj, forward<Args>(args)...);
        m_tasks.push(task);
        m_cond.notify_one();
    }

    string getThreadId();

    ThreadSize getActiveThreadsSize();

    bool hasTask();

private:

    string name;

    ThreadSize kInitThreadsSize;

    ThreadSize activeThreadsSize;

    atomic_bool m_isStarted;

    Threads m_threads;

    Tasks m_tasks;

    mutex m_mutex;

    Cond m_cond;

    bool verbose;

    Task take();

    void log(std::string msg, bool isThreadPool);

};

}





#endif // SIMPLTHREADPOOL_H
