#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <fstream>
#include<algorithm>
#include <set>
#include <list>
#include <map>
#include <chrono>
#include <stack>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <pthread.h>
#include <mutex>
#include <functional>
#include <iostream>
#include <condition_variable>
#include <queue>
#include <thread>
#include <assert.h>
#include <sstream>
#include <atomic>

using namespace std;


typedef unsigned int ID;
typedef unordered_map<ID, unordered_set<ID>> Records;
typedef vector<ID> Path;
struct myComp
{
    bool operator() (const Path& a, const Path& b)
    {
        if(a.size()==b.size()){
            for(unsigned int i=0; i<a.size(); i++){
                if(a[i]!=b[i])return a[i]<b[i];
            }
        }
        return a.size()<b.size();
    }
};
typedef set<Path, myComp> PathSet;


// 图
class Graph
{

private:


public:
    Graph* gr = nullptr;
    unsigned int vexnum;				// 顶点数量
    vector<Records> sccs;
    unordered_map<ID, unordered_set<ID>> adjlist;		// 邻接表

    // 初始化
    Graph(){}

    // 从 v 开始，进行 DFS
    void addEdge(const ID& v, const ID& w){
        if(adjlist.find(v)==adjlist.end()){
            std::unordered_set<ID> vals;
            vals.emplace(w);
            adjlist[v] = vals;
            vexnum++;
        }else{
            adjlist[v].emplace(w);
        }
    }

    void DFS(const ID& v, unordered_map<ID, bool>& visited, Records& scc){
        // 改变 v 的状态
        visited[v] = true;
    //    cout << v << " ";
//        scc.emplace(v);
        unordered_set<ID> vals;

        unordered_set<ID>::iterator i;
        for (i = adjlist[v].begin(); i != adjlist[v].end(); ++i)
        {

            if (!visited[*i])
            {
                vals.emplace(v);
                DFS(*i, visited, scc);
            }else{
                if(scc.find(*i)!=scc.end())vals.emplace(*i);
            }
        }
        scc[v] = vals;
    }

    // 以每个顶点的 post 值的递增顺序将顶点填入栈中
    void Poststack(const ID& v, unordered_map<ID, bool>& visited, stack<ID> &Stack){
        visited[v] = true;

        unordered_set<ID>::iterator i;
        for (i = adjlist[v].begin(); i != adjlist[v].end(); ++i)
        {
            if (!visited[*i])
            {
                Poststack(*i, visited, Stack);
            }
        }

        Stack.push(v);

        return;
    }

    // 收集强连通分量
    void collectSCCs(){

        // 栈
        stack<ID> Stack;

        // 创建visited
//        bool *visited = new bool[vexnum];
        unordered_map<ID, bool> visited;
        unordered_map<ID, bool> visited_;


        for (auto &adj:adjlist){
            visited[adj.first] = false;
            visited_[adj.first] = false;
        }

        // 根据访问结束时间填充栈 - 第一次DFS
        for (auto &adj:adjlist)
        {
            if (visited[adj.first] == false)Poststack(adj.first, visited, Stack);
        }

        // 根据栈中的顺序处理所有顶点
        while (Stack.empty() == false)
        {
            // 弹出post值最大的顶点
            ID v = Stack.top();
            Stack.pop();

            // 输出一个强连通分量
            if (visited_[v] == false)
            {
                Records scc;
                gr->DFS(v, visited_, scc);
    //            cout << endl;
                if(scc.size()>2)sccs.emplace_back(scc);
            }
        }
}

    // 得到逆序图
    Graph getTranspose()
    {
        Graph g;
        for (auto &adj:adjlist)
        {
            // 反转所有边
            unordered_set<ID>::iterator i;
            for (i = adjlist[adj.first].begin(); i != adjlist[adj.first].end(); ++i)
            {
                g.addEdge(*i, adj.first);
            }
        }
        return g;
    }

};

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



}


vector<string> split(string s,char ch){
    unsigned int start=0;
    unsigned int len=0;
    vector<string> ret;
    for(unsigned int i=0;i<s.length();i++){
        if(s[i]==ch){
            ret.push_back(s.substr(start,len));
            start=i+1;
            len=0;
        }
        else{
            len++;
        }
    }
    if(start<s.length())
        ret.push_back(s.substr(start,len));
    return ret;

}


class Bank{

public:

    Bank(){

        g.gr = new Graph();
    }

void resolve(const string& account)
{

    ifstream rs(account.c_str(), ios::in);
    string line;
    ID key, val;
    vector<string> items;

    if(rs.good()){
        while(!rs.eof()){
            getline(rs, line);
            if(line=="")continue;
            items = split(line, ',');
            key = static_cast<ID>(atoi(items[0].c_str()));
            val = static_cast<ID>(atoi(items[1].c_str()));
            g.addEdge(key, val);
            g.gr->addEdge(val, key);

        }
    }
    rs.close();
}

void cropGraph1(Graph& g, unordered_map<ID, vector<ID>>& pathMid){

    unordered_set<ID> ids;
    queue<ID> q;

    for(auto &record:g.adjlist){
        ID start = record.first;
        ID end = record.first;
        vector<ID> mid;
        mid.push_back(record.first);
        while(g.adjlist[end].size()==1){
            unordered_set<ID>::iterator it=g.adjlist[end].begin();
            g.adjlist[end].erase(*it);
            end = *it;
            mid.insert(mid.end(), end);
        }
        while(g.gr->adjlist[start].size()==1){
            unordered_set<ID>::iterator it=g.gr->adjlist[start].begin();
            g.gr->adjlist[start].erase(*it);
            start = *it;
            mid.insert(mid.begin(), start);
        }
        g.adjlist[start].emplace(end);
        g.gr->adjlist[end].emplace(start);
        if(mid.size()>1)pathMid[start]=mid;

    }

}

void cropGraph2(Graph& g){

    queue<ID> outq;
    queue<ID> inq;
    for(auto &record:g.adjlist){
        if(record.second.size()==0){
            outq.push(record.first);
        }
    }

    for(auto &record:g.gr->adjlist){
        if(record.second.size()==0){
            inq.push(record.first);
        }
    }

    while(!outq.empty()||!inq.empty()){
        for(unsigned int i=0; i<outq.size();i++){
             ID root = outq.front();
             if(g.gr->adjlist.find(root)!=g.gr->adjlist.end()){
                for(auto &in:g.gr->adjlist[root]){
                    g.adjlist[in].erase(root);
                    if(g.adjlist[in].size()==0)outq.push(root);
                }
                g.gr->adjlist.erase(root);
             }
             g.adjlist.erase(root);
        }
        for(unsigned int i=0; i<inq.size();i++){
             ID root = inq.front();
             if(g.adjlist.find(root)!=g.adjlist.end()){
                for(auto &out:g.adjlist[root]){
                    g.gr->adjlist[out].erase(root);
                    if(g.gr->adjlist[out].size()==0)inq.push(root);
                }
                g.adjlist.erase(root);
             }
             g.gr->adjlist.erase(root);
        }
    }





       // BfS修剪


}

void search(const ID& cur_id, Records& scc, Path& path, unordered_set<ID>& path_)
{
        if(cur_id==46){
            return;
        }

        if(scc.find(cur_id)==scc.end())return;
        if(path.size()==0){
            path.emplace_back(cur_id);
            for(auto &id:scc.at(cur_id))search(id, scc, path, path_);
            path.pop_back();
        }else{
            if(path[0]!=cur_id&&path.size()<7&&cur_id>path[0]&&path_.find(cur_id)==path_.end()){
                 path.emplace_back(cur_id);
                 path_.emplace(cur_id);
                 for(auto &id:scc.at(cur_id))search(id, scc, path, path_);
                 path.pop_back();
                 path_.erase(cur_id);
            }else if(path[0]==cur_id&&path.size()>=3){
                 {
                  unique_lock<mutex> lock(mtx);
                 if(pathSet.find(path)==pathSet.end())pathSet.emplace(path);
                 }
                 return;
            }

        }

}

void consearch(Records& scc){
    Path path;
    unordered_set<ID> path_;
    for(Records::iterator record=scc.begin(); record!=scc.end(); ++record){
//        pool.addTask(&Bank::search, this, record->first, scc, path, path_);
          this->search(record->first, scc, path, path_);
    }
}

void writeout(const string& outPath){
    ofstream f;
    f.open(outPath.c_str());
    if(f.is_open()){
        f << pathSet.size() << "\n";
        for(auto p:pathSet){
            stringstream ss;
            ss << p[0];
            for(unsigned int i=1; i<p.size();i++)ss << "," << p[i];
            f<< ss.str() << "\n";
        }
    }else{
        cout << "open file fail!" << endl;
    }
    f.close();
}

Graph g;

mutex mtx;

PathSet pathSet;

unsigned int COUNT=0;

private:




};

int main()
{

    auto t_start = chrono::steady_clock::now();

    string txt_path = "C:\\Users\\lxr\\Desktop\\hw_contest//test_data.txt";
    string outPath = "C://Users//lxr//Desktop//result.txt";


//    string txt_path = "/data/test_data.txt";
//    string outPath = "/projects/student/result.txt";


    bool verbose = false;

    mthread::simplthreadpool pool("consumer", 3, verbose=false);

    Bank* bank = new Bank();

    bank->resolve(txt_path);

    auto g_start = chrono::steady_clock::now();

    bank->g.collectSCCs();

    auto g_end = chrono::steady_clock::now();

    auto s_start = chrono::steady_clock::now();

//    pool.start();
//    for(auto &scc:bank->g.sccs){
//            pool.addTask(&Bank::consearch, bank, scc);
//    }
//    while(pool.getActiveThreadsSize()>0);

    Path path;
    unordered_set<ID> path_;
    for(auto &scc:bank->g.sccs){
    for(Records::iterator record=scc.begin(); record!=scc.end(); ++record){
          bank->search(record->first, scc, path, path_);
    }}

    auto s_end = chrono::steady_clock::now();

    bank->writeout(outPath);

    auto t_end = chrono::steady_clock::now();

    if(verbose){
        cout << bank->pathSet.size() << endl;
        for(auto &p:bank->pathSet){
            stringstream ss;
            ss << p[0];
            for(unsigned int i=1; i<p.size();i++)ss << "," << p[i];
            cout << ss.str() << endl;
        }
    }

    auto duration_t = chrono::duration_cast<chrono::microseconds>(t_end - t_start);
    auto duration_s = chrono::duration_cast<chrono::microseconds>(s_end - s_start);
    auto duration_g = chrono::duration_cast<chrono::microseconds>(g_end - g_start);


    cout << "pathNum: " << bank->pathSet.size() << endl;
    cout << "COUNT: " <<  bank->COUNT << endl;
    cout << "ssc_eplase: " <<  double(duration_g.count())/1000 << "ms" << endl;
    cout << "serach_eplase: " <<  double(duration_s.count())/1000 << "ms" << endl;
    cout << "total_eplase: " <<  double(duration_t.count())/1000 << "ms" << endl;

    delete bank;

    return 0;
}


