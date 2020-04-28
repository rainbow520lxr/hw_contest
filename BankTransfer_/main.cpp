#include <iostream>
#include <unordered_map>
#include <stack>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <set>
#include "datatype.h"
#include "algorithm.h"
#include "tool.h"


using namespace std;


int main()
{

    cout << "=============================START=============================" << endl;
    Records records;
    Path path;
    PathInfos pathInfos;
    unordered_set<int> head_ids;


//    string txt_path = "C://Users//lxr//Desktop//banktransfer.txt";
    string txt_path = "C://Users//lxr//Desktop//hw_contest//test_data.txt";

    cout << "read txt..." << endl;
    if(!tools::resolve(records, txt_path))cout << "reading record fails!" << endl;
    cout << "complete!" << endl;

    cout << "split graph..." << endl;
    algorithm::splitGraph(records, head_ids);
    cout << "complete!" << endl;

    cout << "search graph..." << endl;
    for(auto &head_id:head_ids)algorithm::search(records, head_id, path, pathInfos);
    cout << "complete!" << endl;

    cout << "=============================RESULT=============================" << endl;
    cout << pathInfos.size() << endl;
    for(PathInfo pathInfo: pathInfos){
        cout << pathInfo.pathstr << endl;
    }





}
