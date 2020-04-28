#include "simplthreadpool.h"

using namespace mthread;


// 全局的日志锁,方便所有日志打印都是顺序打印
mutex simplthreadpool::logmtx;

void simplthreadpool::log(std::string msg, bool isThreadPool) {
    std::unique_lock<std::mutex> lock(logmtx);
    if(isThreadPool)std::cout << "[INFO] " << name << "-ThreadPool " << msg << "." <<std::endl;
    else std::cout << "[INFO] " << name << "-Thread-" << getThreadId() << ": "<<  msg << "." << std::endl;
}

// 线程池启动
void simplthreadpool::start(){
    // 没有线程则中断
    assert(m_threads.empty());

    // 状态转运行
    m_isStarted = true;

    // 线程集合预留核心线程空间
    m_threads.reserve(kInitThreadsSize);

    // 初始创建三个核心线程
    for (unsigned int i = 0; i < kInitThreadsSize; ++i)
    {
      // 创建三个线程并进入循环中
       m_threads.emplace_back(new std::thread(std::bind(&simplthreadpool::threadLoop, this)));
    }

}

void simplthreadpool::stop()
{


    if(verbose)log("stop", 1);

    // 在初始化时加锁，当lock对象释放时，自动解锁
    std::unique_lock<std::mutex> lock(m_mutex);
    m_isStarted = false;
    // 主线程通知其他线程可以开始争夺锁了
    m_cond.notify_all();

    if(verbose)log("notify_all", 1);

    // 等每个线程执行完释放所有线程
    for(Threads::iterator it = m_threads.begin(); it != m_threads.end() ; ++it)
    {
       // 线程池中使用detach, 这里将线程挂入后台继续执行，主线程继续执行，主线程完毕则回收资源，join需要等子线程结束才能走主线程
      (*it)->detach();
      delete *it;
    }

    // 清楚线程指针
    m_threads.clear();

}

// 线程循环
void simplthreadpool::threadLoop(){


    if(verbose)log("start", 0);

    while(m_isStarted)
    {
      // 获取任务
      Task task = take();
      // 如果任务不为空，则执行
      if(task){

          task();
          {
            std::unique_lock<std::mutex> lock(m_mutex);
            activeThreadsSize--;
            if(verbose)log("processe one task(activeThreadsSize="+to_string(activeThreadsSize)+")", 0);
          }
      }
    }

    if(verbose)log("exit", 0);


}

// 消费
Task simplthreadpool::take()
{

    std::unique_lock<std::mutex> lock(m_mutex);
  // 当任务队列为空并且线程阻塞
    while(m_tasks.empty() && m_isStarted)
    {

       if(verbose)log("wait(activeThreadsSize="+to_string(activeThreadsSize)+")", 0);
        // 等待锁，将锁让出
        m_cond.wait(lock);
    }

   activeThreadsSize++;

   if(verbose)log("wakeup(activeThreadsSize="+to_string(activeThreadsSize)+")", 0);

   Task task;

   Tasks::size_type size = m_tasks.size();

  // 此时队列中存在任务
   if(!m_tasks.empty() && m_isStarted)
   {
     task = move(m_tasks.front());
     m_tasks.pop();
     assert(size - 1 == m_tasks.size());

   }

  return task;

}

bool simplthreadpool::hasTask(){
//    std::unique_lock<std::mutex> lock(m_mutex);
    if(m_tasks.empty())return false;
    return true;
}

ThreadSize simplthreadpool::getActiveThreadsSize(){
//    std::unique_lock<std::mutex> lock(m_mutex);
    return activeThreadsSize;
}

string simplthreadpool::getThreadId(){
    std::stringstream tmp;
    tmp << this_thread::get_id();
    return tmp.str();
}
