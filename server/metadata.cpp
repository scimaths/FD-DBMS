#include "metadata.h"

using namespace std;

TableMetadata::TableMetadata(vector<string> attrList, map<string, Type> attrType, vector<string> pkList) {
    this->attrList = attrList ;
    this->attrType = attrType ;
    this->pkList = pkList ;
}

void TableMetadata::dump(string pathname) {
    ofstream file(pathname, std::ios::trunc) ;
    if (file.is_open()) {
        file << attrList.size() << "\n" ;
        for (string item: attrList) {
            file << item << " " << attrType[item].type << "\n" ;
        }
        file << pkList.size() << "\n" ;
        for (string item: pkList) {
            file << item << " " ;
        }
        file << "\n" ;
        file.close() ;
    }
    else {
        throw pathname + " not found" ;
    }
}

void TableMetadata::retrieve(string pathname) {
    vector<string> list ;

    ifstream file(pathname) ;
    string item ;
    if (file.is_open()) {
        getline (file, item) ;
        int num_attr = stoi(item) ;

        for (int i=0; i<num_attr; i++) {
            getline (file, item) ;
            vector<string> attrData = tokenize(item, " ") ;
            attrList.push_back(attrData[0]) ;
            attrType[attrData[0]] = Type(attrData[1]) ;
        }

        getline (file, item) ;
        pkList = tokenize(item, " ") ;
        file.close();
    }
    else {
        throw pathname + " not found" ;
    }
}