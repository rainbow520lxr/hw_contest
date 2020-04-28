#include "algorithm.h"



using namespace std;


//template <typename T>
void algorithm::search(Records records, int cur_id, Path& path, PathInfos& pathInfos)
{


//     cout << "cur_id: " << cur_id << endl;
     if(records[cur_id].size()==0)return ;

     pathlist* pathnode = nullptr;
     pathlist* tempnode = nullptr;


     // 每个vector为下一跳节点集
     if(path.find(cur_id)==path.end()){
         std::vector<int> ids = records[cur_id];
         for(int id:ids){
              path.insert(std::make_pair(cur_id, id));
              search(records, id, path, pathInfos);
              path.erase(cur_id);
         }
     }else{

             int key_id = path[cur_id];
             pathnode = new pathlist(cur_id);
             tempnode = pathnode;
             int count = 1;
             while(key_id!=cur_id){
                 tempnode->next = new pathlist(key_id);
                 tempnode = tempnode->next;
                 key_id = path[key_id];
                 count++;
             }

             // 限制条件
             if(count>=3&&count<=7){

                 // 重排序
                 pathnode = algorithm::sort(pathnode);
                 // 转字符串
//                 paths.push_back();
                 string pathstr;
                 stringstream ss;
                 tempnode = pathnode;
                 ss << tempnode->id;
                 while(tempnode->next!=nullptr){
                     ss << "," << tempnode->next->id;
                     tempnode = tempnode->next;
                 }
                 // 销毁
                 ss >> pathstr;

                 PathInfo pathInfo(count, pathnode, pathstr);

                 if(pathInfos.find(pathInfo)==pathInfos.end())pathInfos.insert(pathInfo);

             }

     }

}


bool algorithm::splitGraph(Records& records, HeadIds& headIds){

    // 已经遍历过的结点
    unordered_set<int> ids;

    for(auto &record : records){
        int first = record.first;
        vector<int> second = record.second;
        if(ids.find(first)==ids.end()){

            // 检查头ID的上是否还有不可达父ID
            for(uint8_t i=0;i<second.size();i++){
                if(ids.find(second[i])==ids.end())continue;
                else{
                    records[first].erase(records[first].begin()+i);

//                    cout << "delete records" << first << "-" << record.second[i] << endl;

                }
            }

            ids.insert(first);
            headIds.insert(first);

//            cout << "insert head_id:" << first << endl;

//            cout << "开辟遍历队列..." << endl;
            queue<int> q;
            q.push(first);
            while(!q.empty()){
               string out = "";
               for(uint8_t i=0;i<q.size();i++){
                   vector<int> childs = records[q.front()];
                   for(auto &child:childs){

                       if(ids.find(child)==ids.end()){
                           q.push(child);
                           ids.insert(child);
//                           cout << "ids add id:" << child << endl;
                       }
                   }
                   q.pop();
               }

        }

        }

//        cout << record.first << endl;

    }
    return true;



}



pathlist* algorithm::sort(pathlist* pathnode){
    pathlist* pre = new pathlist(INT_MAX);
    pre->next = pathnode;
    pathlist* temp = pre;
    int min = temp->id;
    pathlist* min_ptr = temp;
    while(temp->next!=nullptr){
        if(temp->next->id<min){
            min = temp->next->id;
            min_ptr = temp;
        }

        temp = temp->next;
    }
    pathnode = min_ptr->next;
    temp->next = pre;
    min_ptr->next = nullptr;
    temp->next = pre->next;
    pre->next = nullptr;
    delete pre;
    pre = nullptr;
    return pathnode;
}
