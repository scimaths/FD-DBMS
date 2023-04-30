#ifndef DATA_H
#define DATA_H

#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <string> 
#include "type.h"
#include "token.h"
#include "metadata.h"

using namespace std;

class TableData {
    public:

    TableMetadata* tableMetadata ;
    vector<Record*> recordList ;
    string table_path;

    TableData(string pathname);
    void retrieve(string pathname) ;
} ;

#endif // DATA_H