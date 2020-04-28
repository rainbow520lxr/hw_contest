#include "tool.h"
#include <regex>


using namespace std;

bool tools::resolve(Records& records, string path){

    ifstream rs;
    string line, id_s, id_r;

//    regex match_pat("([1-9][0-9]*,){2}([1-9][0-9]*)");
    std::regex re("[0-9][1-9]*");
    smatch sm;

    rs.open(path);

    if(rs.is_open()){
        while(!rs.eof()){
            getline(rs, line);
//            if(line=="")continue;
            if(regex_search(line, sm, re)){
                for(auto id : sm)cout << sm << endl;
            }
            getchar();
            int index = line.find(",");
            id_s = line.substr(0, index);
            id_r = line.substr(index+1, line.find(",", index+1)-index-1);
//            cout << id_s << "-" << id_r << endl;
            if(records.find(stoi(id_s, nullptr, 10))==records.end()){
                std::vector<int> id_rs;
                id_rs.push_back(stoi(id_r, nullptr, 10));
                records[stoi(id_s, nullptr, 10)] = id_rs;
            }else{
                records[stoi(id_s, nullptr, 10)].push_back(stoi(id_r, nullptr, 10));
            }
//            cout << "success!" << endl;

        }
        rs.close();
        return true;
    }
    rs.close();
    return false;

}
