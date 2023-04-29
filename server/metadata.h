#include <vector>
#include <map>
#include <fstream>
#include "type.h"
#include "token.h"

using namespace std;

class TableMetadata {
    public:
    
    vector<string> attrList ;
    map<string, Type> attrType ;
    vector<string> uniqueList ;

    TableMetadata() ;
    TableMetadata(vector<string> attrList, map<string, Type> attrType, vector<string> uniqueList) ;

    void dump(string pathname) ;
    void retrieve(string pathname) ;
    bool check_val(vector<string> valList) ; // check if valList satisfy attrType constraints
    Record create_record(vector<string> valList) ;
} ;

class DatabaseMetadata {

} ;