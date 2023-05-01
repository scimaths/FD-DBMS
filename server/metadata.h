#include <vector>
#include <map>
#include <fstream>
#include "type.h"
#include "token.h"

using namespace std;

#ifndef METADATA_HEADER
#define METADATA_HEADER

class TableMetadata {
    public:
    
    vector<string> attrList ;
    map<string, string> attrType ;
    vector<string> uniqueList ;

    TableMetadata() ;
    TableMetadata(string pathname) ;
    TableMetadata(vector<string> attrList, map<string, string> attrType, vector<string> uniqueList) ;

    void dump(string pathname) ;
    void retrieve(string pathname) ;
    bool check_val(vector<string> valList) ; // check if valList satisfy attrType constraints
    Record* create_record(vector<string> valList) ;
} ;

class DatabaseMetadata {

} ;

#endif