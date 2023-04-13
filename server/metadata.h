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
    vector<string> pkList ;

    TableMetadata(vector<string> attrList, map<string, Type> attrType, vector<string> pkList) ;

    void dump(string pathname) ;
    void retrieve(string pathname) ;
} ;

class DatabaseMetadata {

} ;