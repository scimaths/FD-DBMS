#include "data.h"

void TableData::retrieve(string pathname) {
    tableMetadata.retrieve(pathname+"/metadata.txt") ;

    ifstream file(pathname+"/data.txt") ;
    string item ;
    if (file.is_open()) {
        while(getline(file, item)) {
            vector<string> valList = tokenize(item, ", ") ; 

            if (valList.size() != tableMetadata.attrList.size()) {
                throw "Metadata doesn't match with data file" ;
            }

            recordList.push_back(tableMetadata.create_record(valList)) ;
        }
        file.close();
    }
    else {
        throw pathname + " not found" ;
    }
}

