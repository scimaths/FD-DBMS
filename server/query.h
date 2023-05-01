#include <string>
#include <vector>
#include <map>
#include "type.h"

using namespace std;

class Query {
    public:
    string query_type ;
} ;

class DatabaseQuery : public Query {
    public:
    string db_name ;
    string get_dbPath() ;
} ;

class TableQuery : public DatabaseQuery {
    public:
    string table_name ;
    string get_tablePath() ;
} ;

class CreateTableQuery : public TableQuery {
    public:
    vector<string> attrList ;
    map<string, string> attrType ;
    vector<string> uniqueList ;
} ;

class InsertQuery : public TableQuery {
    public:
    vector<string> valList ;
} ;

