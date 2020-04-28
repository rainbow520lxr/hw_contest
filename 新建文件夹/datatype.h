#ifndef DATATYPE_H
#define DATATYPE_H
#include <string>
#include <set>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <vector>
#include <memory>


using namespace std;


typedef unsigned int ID;
typedef tr1::unordered_map<ID, vector<ID> > Records;
typedef tr1::unordered_map<ID, ID> Path;
typedef vector<ID> HeadIds;


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



#endif // DATATYPE_H
