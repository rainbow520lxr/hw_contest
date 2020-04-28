#ifndef DATATYPE_H
#define DATATYPE_H

#include <string>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <vector>

using namespace std;

struct pathlist{
    int id;
    struct pathlist* next;
    pathlist(int id){
        this->id = id;
        this->next = nullptr;
    }

};

struct PathInfo{

    int length = 0;
    pathlist* pathnode = nullptr;
    string pathstr = "";

    PathInfo(int length, pathlist* pathnode, string pathstr){
        this->length = length;
        this->pathnode = pathnode;
        this->pathstr = pathstr;
    }

    bool operator<(const PathInfo& other)const{

        if(this->length==other.length){
            const pathlist* tempa = this->pathnode;
            const pathlist* tempb = other.pathnode;

            while(tempa!=nullptr&&tempb!=nullptr){
                if(tempa->id<tempb->id)return true;
                else if(tempa->id==tempb->id){
                    tempa = tempa->next;
                    tempb = tempb->next;
                }else return false;
            }
            return this->length>other.length;

        }
        return this->length<other.length;

    }

    bool operator==(const PathInfo& other) const{//这个重载==号必须要写，否则报错
        return this->pathstr==other.pathstr;
    }

};

typedef unordered_map<int, vector<int>> Records;
typedef unordered_map<int, int> Path;
typedef set<PathInfo> PathInfos;
typedef unordered_set<int> HeadIds;



#endif // DATATYPE_H
