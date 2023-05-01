#ifndef FUNC_DEP_H
#define FUNC_DEP_H

#include <string>
#include <vector>
#include <map>
#include "data.h"
#include "token.h"

using namespace std;

class FuncDependancy {
    vector<string> lAttrList ;
    vector<string> rAttrList ;

    public:

    FuncDependancy(string fd_str) ;
    bool check(vector<Record*>& recordList) ;
};

#endif // FUNC_DEP_H