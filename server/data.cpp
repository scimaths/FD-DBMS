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

            Record record ;
            int iter = 0 ;
            for (string attr: tableMetadata.attrList) {
                if(tableMetadata.attrType[attr].type == "INT") {
                    record.elements[attr] = IntValue(stoi(valList[iter])) ;
                }
                else if(tableMetadata.attrType[attr].type == "FLOAT") {
                    record.elements[attr] = FloatValue(stof(valList[iter])) ;
                }
                else if(tableMetadata.attrType[attr].type == "STRING") {
                    record.elements[attr] = StringValue(valList[iter]) ;
                }
                iter++ ;
            }
            recordList.push_back(record) ;
        }
        file.close();
    }
    else {
        throw pathname + " not found" ;
    }
}

