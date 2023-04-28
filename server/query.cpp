#include "query.h"

string DatabaseQuery::get_dbPath() {
    return "db/"+db_name ;
}

string TableQuery::get_tablePath() {
    return get_dbPath() + "/" + table_name ;
}