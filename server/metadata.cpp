#include "metadata.h"

using namespace std;

TableMetadata::TableMetadata() {

}

TableMetadata::TableMetadata(string pathname) {
    this->retrieve(pathname);
}

TableMetadata::TableMetadata(vector<string> attrList, map<string, string> attrType, vector<string> pkList) {
    this->attrList = attrList ;
    this->attrType = attrType ;
    this->uniqueList = uniqueList ;
}

void TableMetadata::dump(string pathname) {
    ofstream file(pathname+"/metadata.txt", std::ios::trunc) ;
    if (file.is_open()) {
        file << attrList.size() << "\n" ;
        for (string item: attrList) {
            file << item << " " << attrType[item] << "\n" ;
        }
        file << uniqueList.size() << "\n" ;
        for (string item: uniqueList) {
            file << item << " " ;
        }
        file << "\n" ;
        file.close() ;
    }
    else {
        throw pathname+"/metadata.txt" + " not found" ;
    }
}

void TableMetadata::retrieve(string pathname) {
    attrList.clear();
    attrType.clear();
    uniqueList.clear();

    ifstream file(pathname+"/metadata.txt") ;
    string item ;
    if (file.is_open()) {
        getline (file, item) ;
        int num_attr = stoi(item) ;

        for (int i=0; i<num_attr; i++) {
            getline (file, item) ;
            vector<string> attrData = tokenize(item, " ") ;
            attrList.push_back(attrData[0]) ;
            attrType[attrData[0]] = attrData[1] ;
        }

        getline (file, item) ;
        uniqueList = tokenize(item, " ") ;
        file.close();
    }
    else {
        throw pathname+"/metadata.txt" + " not found" ;
    }
}

bool TableMetadata::check_val(vector<string> valList) {
    if (valList.size() == attrList.size()) return false ;

    int iter = 0 ;
    for (string attr: attrList) {
        if(attrType[attr] == "INT") {
            try {
                stoi(valList[iter]) ;
            }
            catch (exception &err){
                return false ;
            }
        }
        else if(attrType[attr] == "FLOAT") {
            try {
                stof(valList[iter]) ;
            }
            catch (exception &err){
                return false ;
            }
        }
        iter++ ;
    }

    return true ;
}

Record* TableMetadata::create_record(vector<string> valList) {
    Record* record = new Record();
    int iter = 0 ;
    for (string attr: attrList) {
        if(attrType[attr] == "INT") {
            record->elements[attr] = new IntValue(stoi(valList[iter])) ;
        }
        else if(attrType[attr] == "FLOAT") {
            record->elements[attr] = new FloatValue(stof(valList[iter])) ;
        }
        else if(attrType[attr] == "STRING") {
            record->elements[attr] = new StringValue(valList[iter]) ;
        }
        iter++ ;
    }
    return record;
}