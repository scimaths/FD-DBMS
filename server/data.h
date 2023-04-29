#ifndef DATA_H
#define DATA_H

#include <vector>
#include <map>
#include <fstream>
#include <string> 
#include "type.h"
#include "token.h"
#include "metadata.h"

using namespace std;

class TableData {
    public:

    TableMetadata tableMetadata ;
    vector<Record> recordList ;

    void retrieve(string pathname) ;
} ;

#endif // DATA_H