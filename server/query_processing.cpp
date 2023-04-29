#include <vector>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "query.h"
#include "file.h"
#include "metadata.h"

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

        // syntactic check of uniqueList is indeed subset of attrList

        TableMetadata tableMetadata(query->attrList, query->attrMap, query->uniqueList) ;
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
            throw "Value does not satisfy type constraint" ;
        }

        // Check Local and Global Constraints

        string item = "" ;
        for (string val: query->valList) {
            item += val + ", " ;
        }

        append_item_to_file(query->get_tablePath()+"/data.txt", item) ;
    }

    
}

int main() {
    DatabaseQuery q ;
    q.db_name = "b" ;
    q.query_type = "CREATE DATABASE" ;
    try
    {
        process_query(&q) ;
    }
    catch(string s)
    {
        std::cerr << s << '\n';
    }
    
    
}