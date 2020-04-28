#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include "datatype.h"

using namespace std;

namespace  algorithm{

    // 追溯搜索
    void search(Records records, int cur_id, Path& path, PathInfos& pathInfos);

    // 切割图
    bool splitGraph(Records& records, HeadIds& headIds);

    // 单链表移位
    pathlist* sort(pathlist* pathnode);


}


#endif // ALGORITHM_H
