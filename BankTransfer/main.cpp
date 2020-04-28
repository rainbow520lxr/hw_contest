#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <fstream>
#include<algorithm>
#include <set>
#include <map>
#include <windows.h>
#include <stack>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <pthread.h>

using namespace std;


typedef unsigned int ID;


bool comp(const pair<ID, unordered_set<ID>>& A, const pair<ID, unordered_set<ID>>& B){
    return A.second.size()<B.second.size();
}

struct S{
    ID id;
    unsigned int siz;
    S(ID id, unsigned int siz){
        this->id = id;
        this->siz = siz;
    }
    bool operator< (const S& other)const {
        return this->siz > other.siz;
    }
};


// ID 标记ID的节点的
typedef map<ID, unordered_set<ID>> Records;
typedef unordered_map<ID, ID> Path;
typedef unordered_set<ID> HeadIds;
struct PathInfo{
    vector<ID> pathv;
    string pathstr;
    PathInfo(const vector<ID>& pathv,const string& pathstr){
        this->pathv = pathv;
        this->pathstr = pathstr;
    }
    bool operator< (const PathInfo& other) const{
        if(other.pathv.size()==this->pathv.size()){
            for(unsigned int i=0; i<this->pathv.size(); i++){
                if(this->pathv[i]!=other.pathv[i])return this->pathv[i]<other.pathv[i];
            }
        }
        return this->pathv.size()<other.pathv.size();

    }

    bool operator== (const PathInfo& other) const{
        return this->pathstr==other.pathstr;
    }

};

typedef set<PathInfo> PathSet;


string toString(unsigned int num){
    stringstream ss;
    ss << num;
    return ss.str();
}

namespace tool {

vector<string> split(string s,char ch);

}


namespace bank {



// 图
class Graph
{

private:


public:
    vector<Records> sccs;
    Graph* gr = nullptr;
    unsigned int vexnum;				// 顶点数量

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
                DFS(*i, visited, scc);
                vals.emplace(*i);
            }else{
                vals.emplace(*i);
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


class TransferAnalyzer{


public:

    TransferAnalyzer(string account, string outPath, bool isMutiThread=false, bool verbose=false):
    account(account),outPath(outPath),isMutiThread(isMutiThread),verbose(verbose),COUNT(0){
        g.gr=new Graph();
    }

    ~TransferAnalyzer(){


    }

    void resolve();

    void splitGraph();

    void search(ID cur_id, const Records& scc, unordered_set<ID>& ids);

//    void search_(ID cur_id, unordered_set<ID>& ids);

    void mutiThreadSearch(const ID& cur_id, Path& path);

    void write();

    void run();

    Graph g;

    string account;

    string outPath;

    // 记录
    Records records;

    // 入度统计
    Records records_;

    // 头id
    HeadIds headIds;

    // 路径
    Path mpath;

    // 路径集
    PathSet pathSet;

    // 复杂度
    uint64_t COUNT = 0;

    unsigned pathsize = 0;

    // 是否多线程
    bool isMutiThread;

    // 日志开关
    bool verbose;

private:

    TransferAnalyzer(const TransferAnalyzer&);    //禁止复制拷贝.

    const TransferAnalyzer& operator=(const TransferAnalyzer&);  //赋值函数

};


}


int main()
{

//    DWORD start, end;

//    start= GetTickCount();
      string txt_path = "C://Users//lxr//Desktop//hw_contest//BankTransfer//test_data.txt";
//    string txt_path = "C://Users//lxr//Desktop//hw_contest//test_data.txt";
//    string txt_path = "/data/test_data.txt";


    string outPath = "C://Users//lxr//Desktop//result.txt";
//    string outPath = "/projects/student/result.txt";


    bank::TransferAnalyzer* transferAnalyzer = new bank::TransferAnalyzer(txt_path,outPath, false, true);

    transferAnalyzer->run();

    delete transferAnalyzer;

    transferAnalyzer = nullptr;

//    end = GetTickCount();


//    cout << "TotalEplased: " << end-start << "ms" << endl;

}

using namespace tool;

vector<string> tool::split(string s,char ch){
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

using namespace bank;

void TransferAnalyzer::resolve()
{

    ifstream rs(account.c_str(), ios::in);
    string line;
    ID key, val;
    vector<string> items;

    if(rs.good()){
        while(!rs.eof()){
            getline(rs, line);
            if(line=="")continue;

            items = tool::split(line, ',');
            key = static_cast<ID>(atoi(items[0].c_str()));
            val = static_cast<ID>(atoi(items[1].c_str()));
//            cout << id_s << "-" << id_r << endl;
            g.addEdge(key, val);
            g.gr->addEdge(val, key);
        }
    }
    rs.close();

}

void TransferAnalyzer::search(ID cur_id, const Records& scc, unordered_set<ID>& ids)
{


        COUNT += 1;
        cout << "pathSize: " << mpath.size() << endl;

        if(scc.find(cur_id)==scc.end())return;


        ID key_id;
        ID min_id;
        ID id;

        if(ids.find(cur_id)==ids.end()){
            ids.emplace(cur_id);
            for(auto &id_:scc.at(cur_id)){
                mpath.insert(std::make_pair(cur_id, id_));
                search(id_, scc, ids);
                mpath.erase(cur_id);
            }
        }else{
            if(mpath.find(cur_id)==mpath.end()){

                if(mpath.size()<=7){
                    for(auto &id_:scc.at(cur_id)){
                        mpath.insert(std::make_pair(cur_id, id_));
                        search(id_, scc, ids);
                        mpath.erase(cur_id);
                    }
                }

             }else{
                 key_id = mpath[cur_id];
                 min_id = cur_id;
                 unsigned int count = 1;

                 while(key_id!=cur_id){
                     if(key_id < min_id)min_id = key_id;
                     key_id = mpath[key_id];
                     count++;
                 }

                 // 限制条件
                 if(count>=3&&count<=7){

                     string pathstr = toString(min_id);
                     vector<ID> pathv;
                     id = min_id;
                     pathv.push_back(min_id);

                     while(mpath[id]!=min_id){
                         pathstr += ("," + toString(mpath[id]));
                         pathv.push_back(mpath[id]);
                         id = mpath[id];
                     }

                     PathInfo pathInfo(pathv, pathstr);

                     if(pathSet.find(pathInfo)==pathSet.end())pathSet.insert(pathInfo);

                 }

             }
        }


}


void TransferAnalyzer::write(){
    ofstream f;
    f.open(outPath.c_str());
    if(f.is_open()){
        f << pathSet.size() << "\n";
        for(PathInfo item:pathSet){
            f<<item.pathstr << "\n";
        }
    }else{
        cout << "open file fail!" << endl;
    }
    f.close();
}

void TransferAnalyzer::run(){


    cout << "=============================START=============================" << endl;

    DWORD start, end;


    if(verbose)cout << "read txt..." << endl;
    // 并发读取到文件块，将其合并到sstream, 最后进行解析
    resolve();

    if(verbose)cout << "split graph..." << endl;
    start = GetTickCount();
    g.collectSCCs();
    if(verbose)cout << "headids.size=" << g.sccs.size() << endl;


    if(verbose)cout << "search graph..." << endl;
    for(auto &scc:g.sccs){
//            search(*head_id);
        unordered_set<ID> ids;
        search(scc.begin()->first, scc, ids);
    }
    if(verbose)cout << "maxpath.size=" << pathsize << endl;
    end = GetTickCount();

    if(verbose)cout << "write txt..." << endl;
    write();

    if(verbose){
      cout << "COUNT: " << COUNT << " TimeEplased: " << end-start << "ms" << endl;
//        cout << "COUNT: " << COUNT << endl;
        cout << "=============================RESULT=============================" << endl;
        cout << pathSet.size() << endl;
        for(PathSet::iterator item=pathSet.begin(); item!=pathSet.end(); ++item){
            cout << (*item).pathstr << endl;
        }
    }

}





