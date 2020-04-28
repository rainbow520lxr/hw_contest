#ifndef BANK_H
#define BANK_H
#include <iostream>
#include <stdlib.h>
#include <memory>
#include <map>
#include <sstream>
#include <fstream>
#include <queue>
#include <unistd.h>
#include <pthread.h>

using namespace std;
using namespace std::tr1;


string toString(unsigned int num){
    stringstream ss;
    ss << num;
    return ss.str();
}



namespace tool {

vector<string> split(string s,char ch);

}

namespace mthread{

//class Test{

//   void process();
//};


typedef struct worker_t{
    void* (*process) (void *arg); // 执行的任务方法
    void *arg; // 执行任务的参数
    struct worker_t  *next; // 指向的下一个任务s
} worker_t;


// 这个线程池只能处理某个类的任务
class ThreadPool{


public:

    // 线程运行
    static void *phtread_run(void* arg){

        ThreadPool* tp = (ThreadPool*)arg;

        while(1){
            pthread_mutex_lock(&(tp->mutex));
            while(tp ->worker_queue_size == 0 && tp -> shutdown != 1){
                pthread_cond_wait(&(tp->cond), &(tp->mutex));
            }
            if(tp -> shutdown){
                 pthread_mutex_unlock(&(tp->mutex));
                 pthread_exit(NULL);
            }
            tp->activeThreadSize++;
            tp->worker_queue_size --;
            worker_t *worker = tp->worker_queue;
            tp->worker_queue = tp->worker_queue->next;
            pthread_mutex_unlock(&(tp->mutex));
            (*(worker->process))(worker->arg);
            tp->activeThreadSize--;
            free(worker);
            worker = NULL;
         }
         pthread_exit(NULL);
    }

    ThreadPool(unsigned int core_thread_num):
        core_thread_num(core_thread_num)
    {
        this->tids = (pthread_t*) malloc(sizeof(pthread_t) * core_thread_num);
        for(unsigned int i=0; i < core_thread_num; i++){
            pthread_create(&(this->tids[i]), NULL, ThreadPool::phtread_run, (void*)this);
        }

    }

    ~ThreadPool(){
        if(!shutdown){
            shutdown = 1;
            pool_destroy();
        }
    }

    unsigned int pool_add_worker(void* (*process)(void* arg), void *arg){
        worker_t* new_worker = (worker_t*) malloc(sizeof(worker_t));
        new_worker->process = process;
        new_worker->arg = arg;
        new_worker->next = NULL;
        pthread_mutex_lock(&(mutex));
        worker_t* queue_head = worker_queue;
        if( queue_head != NULL){
            while(queue_head->next != NULL)
                queue_head = queue_head->next;
            queue_head->next = new_worker;
    }
        else
            worker_queue = new_worker;
        worker_queue_size ++;
        pthread_mutex_unlock(&(mutex));
        pthread_cond_signal(&(cond));
            return 0;
    }


    // 线程池销毁
    unsigned int pool_destroy(){
        if(shutdown)
            return -1;
        shutdown = 1;
        pthread_cond_broadcast(&(cond));
        int i;
        for(i=0; i < core_thread_num; i++){
            pthread_join(tids[i], NULL);
        }
        free(this->tids);
            worker_t *head = worker_queue;
            while(head != NULL){
                head = head->next;
                free(head);
        }
        pthread_mutex_destroy(&(mutex));
        pthread_cond_destroy(&(cond));
        return 0;
    }


    unsigned int getActiveThreadSize(){
        return activeThreadSize;
    }

private:

    unsigned int core_thread_num; // 线程池里面核心线程的数量

    worker_t *worker_queue = NULL; // 线程池的工作队列

    int shutdown;	// 标记线程池是否被销毁

    unsigned int worker_queue_size; //

    pthread_t *tids; // 线程池所有线程的线程id，用于等待线程完成

    pthread_mutex_t	mutex; // 互斥锁

    pthread_cond_t	cond; // 线程池条件变量

    unsigned int activeThreadSize;


};


}

namespace bank {

class TransferAnalyzer{


public:

    TransferAnalyzer(string account, string outPath, bool isMutiThread=false, bool verbose=false):
    account(account),outPath(outPath),isMutiThread(isMutiThread),verbose(verbose),COUNT(0){
        // 初始为空
        if(isMutiThread)pool = new mthread::ThreadPool(3);
    }

    ~TransferAnalyzer(){
        delete pool;
    }

    void resolve();

    void splitGraph();

    void search(ID cur_id);

    void mutiThreadSearch(ID& cur_id, Path& path);

    void write();

    void run();

    string account;

    string outPath;

    pthread_mutex_t m_mutex;

    // 记录
    Records records;

    // 头id
    HeadIds headIds;

    // headid指针
    unsigned int idNum;

    // 文件数据块
    map<unsigned int, char*> fblocks;

    // 文件位置指针
    unsigned int fpos;

    // 路径
    Path mpath;

    // 路径集
    PathSet pathSet;

    // 复杂度
    unsigned int COUNT;

    // 是否多线程
    bool isMutiThread;

    // 线程池
    mthread::ThreadPool *pool = NULL;

    // 日志开关
    bool verbose;

private:

    TransferAnalyzer(const TransferAnalyzer&);    //禁止复制拷贝.

    const TransferAnalyzer& operator=(const TransferAnalyzer&);  //赋值函数

};


}

#endif // TRANSFERANALYZER_H
