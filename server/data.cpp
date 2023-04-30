#include "data.h"

void TableData::retrieve(string pathname) {
    ifstream file(pathname+"/data.txt") ;
    string item ;
    if (file.is_open()) {
        while(getline(file, item)) {
            vector<string> valList = tokenize(item, ",") ; 

            if (valList.size() != tableMetadata->attrList.size()) {
                throw "Metadata doesn't match with data file" ;
            }

            recordList.push_back(tableMetadata->create_record(valList)) ;
        }
        file.close();
    }
    else {
        throw pathname + " not found" ;
    }
}

TableData::TableData(string pathname) {
    this->table_path = pathname;
    this->tableMetadata = new TableMetadata(pathname);
    this->retrieve(pathname);
}

// int main() {
//     TableData* td = new TableData("db/b/people");
//     for (Record* rec: td->recordList) {
//         for (pair<string, Value*> map_elem: rec->elements) {
//             cout << map_elem.first << ": ";
//             map_elem.second->print(); cout << '\n';
//         }
//     }
// }