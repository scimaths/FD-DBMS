#include <vector>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "query.h"
#include "file.h"
#include "metadata.h"
#include "local_constraint.h"
#include "global_constraint.h"

using namespace std;

#define PATH_TO_DBLIST "db/db-list.txt"

void process_query(Query *query) {
    if (query->query_type == "CREATE DATABASE") {

        DatabaseQuery * databaseQuery = (DatabaseQuery*) query ;

        vector<string> db_list ;
        db_list = create_list_from_file("./db/db-list.txt") ;

        if (count(db_list.begin(), db_list.end(), databaseQuery->db_name)) {
            throw "Database " + databaseQuery->db_name + " already exists\n" ;
        }

        append_item_to_file("./db/db-list.txt", databaseQuery->db_name) ;
        
        create_folder("./db/"+databaseQuery->db_name) ;
        create_file("./db/"+databaseQuery->db_name+"/table_list.txt") ;
        create_file("./db/"+databaseQuery->db_name+"/constraints.txt") ;

        return ;
    }

    if (query->query_type == "CONNECT DATABASE") {

        DatabaseQuery * databaseQuery = (DatabaseQuery*) query ;

        vector<string> db_list ;
        db_list = create_list_from_file("./db/db-list.txt") ;

        if (count(db_list.begin(), db_list.end(), databaseQuery->db_name)==0) {
            throw "Database " + databaseQuery->db_name + " does not exist\n" ;
        }

        return ;
    }

    if (query->query_type == "DROP DATABASE") {

        DatabaseQuery * databaseQuery = (DatabaseQuery*) query ;
        
        vector<string> db_list ;
        db_list = create_list_from_file("./db/db-list.txt") ;

        if (count(db_list.begin(), db_list.end(), databaseQuery->db_name)==0) {
            throw "Database " + databaseQuery->db_name + " does not exists\n" ;
        }

        db_list.erase(find(db_list.begin(), db_list.end(), databaseQuery->db_name)) ;

        write_list_to_file("./db/db-list.txt", db_list) ;
        delete_folder("./db/"+databaseQuery->db_name) ;

        return ;
    }
    
    if (query->query_type == "CREATE TABLE") {

        CreateTableQuery * query = (CreateTableQuery*) query ;

        if (check_item_in_file(query->get_dbPath()+"/table-list", query->table_name)) {
            throw query->table_name + " already exists" ;
        } 

        create_folder(query->get_tablePath()) ;
        create_file(query->get_tablePath() + "/metadata.txt") ;
        create_file(query->get_tablePath() + "/data.txt") ;

        TableMetadata tableMetadata(query->attrList, query->attrType, query->uniqueList) ;
        tableMetadata.dump(query->get_tablePath() + "/metadata.txt") ;
        
        return ;
    }

    if (query->query_type == "DROP TABLE") {

        TableQuery * query = (TableQuery*) query ;

        if (check_item_in_file(query->get_dbPath()+"/table-list", query->table_name)) {
            throw query->table_name + " does not exist" ;
        } 

        // Handle Constraints 
        // delete from table list
        delete_folder(query->get_tablePath()) ;

        return ;

    }

    if (query->query_type == "SELECT") {
        return ;
    }

    if (query->query_type == "INSERT") {

        InsertQuery * query = (InsertQuery*) query ;

        TableMetadata tableMetaData ;
        tableMetaData.retrieve(query->get_tablePath()) ;
        if (!tableMetaData.check_val(query->valList)) {
            throw "Values do not satisfy type constraint" ;
        }

        LocalConstraint LocalConstraint ;
        LocalConstraint.retrieve(query->get_tablePath()) ;
        if (!LocalConstraint.check(tableMetaData.create_record(query->valList))) {
            throw "Values do not satisfy local constraints" ;
        }
        // Global Constraints

        vector<Record*> record_list = TableData(query->get_tablePath()).recordList ;
        record_list.push_back(tableMetaData.create_record(query->valList)) ;

        GlobalConstraint globalConstraint ;
        globalConstraint.retrieve(query->get_dbPath()) ;
        for (auto fundef:globalConstraint.fdList) {
            if (!fundef.check(record_list)) {
                throw "Values do not satisfy global constraints" ;
            }
        }

        string item = "" ;
        for (string val: query->valList) {
            item += val + ", " ;
        }

        append_item_to_file(query->get_tablePath()+"/data.txt", item) ;
    }

    if (query->query_type == "FUNCDEF") {
        FunDepQuery * query = (FunDepQuery*) query ;
        GlobalConstraint globalConstraint ;
        globalConstraint.add_constraint(query->db_name, query->fd_str) ;
    }

    
}

// int main() {
//     FunDepQuery q ;
//     q.db_name = "univ_db" ;
//     q.query_type = "FUNCDEF" ;
//     q.fd_str = "id|building" ;
//     try
//     {
//         process_query(&q) ;
//     }
//     catch(string s)
//     {
//         std::cerr << s << '\n';
//     }
    
    
// }