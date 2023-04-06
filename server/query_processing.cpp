#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

struct Query {
    string query_type ;
    string db_name ;
} ;

void process_query(Query &query) {
    if (query.query_type == "CREATE DATABASE") {

        ifstream in_db_list("./db/db-list.txt") ;
        string db_name ;
        if (in_db_list.is_open()) {
            while ( getline (in_db_list, db_name) )
            {
                cout << db_name ;
                if (query.db_name == db_name) {
                    throw "Database " + db_name + " already exists\n" ;
                }
            }
            in_db_list.close();
        }

        ofstream out_db_list("./db/db-list.txt") ;
        if (out_db_list.is_open()) {
            out_db_list << query.db_name << endl ;
            out_db_list.close() ;
        }
        
        mkdir(("./db/"+query.db_name).c_str(), 0777) ;
        fstream file ;
        file.open("./db/"+query.db_name+"/rel_list.txt") ;
        if(file.is_open()) {
            file.close() ;
        }
        file.open("./db/"+query.db_name+"/constraints.txt") ;
        if(file.is_open()) {
            file.close() ;
        }

        return ;
    }

    if (query.query_type == "DROP DATABASE") {
        
    }
    
}

int main() {
    Query q ;
    q.db_name = "abc" ;
    q.query_type = "CREATE DATABASE" ;
    process_query(q) ;
}