#include "func_dep.h"

FuncDependancy::FuncDependancy(string fd_str) {
    vector<string> attrLists = tokenize(fd_str, "|") ;
    lAttrList = tokenize(attrLists[0], ", ") ;
    rAttrList = tokenize(attrLists[1], ", ") ;
}

bool FuncDependancy::check(vector<Record*>& recordList) {
    // if recordList is empty then fd is trivially true
    if (recordList.size() == 0) {
        return true ;
    }
    // if all L-attributes are not there in record, then fd is trivially satisfies
    for (string lAttr: lAttrList) {
        if (!recordList[0]->elements.count(lAttr)) {
            return true ;
        }
    }
    map<vector<string>, vector<string>> checkMap ;
    for (Record* record:recordList) {
        vector<string> lValList, rValList ;
        for (string lAttr: lAttrList) {
            lValList.push_back(record->elements[lAttr]->get_string()) ;
        }
        for (string rAttr: rAttrList) {
            if (record->elements.count(rAttr))
                rValList.push_back(record->elements[rAttr]->get_string()) ;
        }
        if (checkMap.count(lValList)) {
            if (checkMap[lValList] != rValList) {
                return false ;
            }
        }
        else {
            checkMap[lValList] = rValList ;
        }
    }
    return true ;
}