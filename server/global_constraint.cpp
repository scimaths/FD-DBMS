#include "global_constraint.h"

void GlobalConstraint::retrieve(string pathname) {
    ifstream file(pathname+"/global_constraints.txt") ;
    string item ;
    if (file.is_open()) {
        while(getline (file, item)) {
            fdList.push_back(FuncDependancy(item)) ;
        }
        file.close();
    }
    else {
        throw pathname + "/global_constraints.txt" + " not found" ;
    }
}

void GlobalConstraint::add_constraint(string pathname, string fd_str) {
    FuncDependancy *fd = new FuncDependancy(fd_str) ;
    vector<string> table_list = create_list_from_file(pathname+"table_list.txt") ;
    for (string table_name: table_list) {
        TableData tableData(pathname+"/"+table_name) ;
        if (!fd->check(tableData.recordList)) {
            throw "Functional Dependancy Inconsistent with data present" ;
        }
    }
    append_item_to_file(pathname+"global_constraint.txt", fd_str) ;
}
