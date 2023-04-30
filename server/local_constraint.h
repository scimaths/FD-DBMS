#ifndef LOCAL_CONSTRAINT_H
#define LOCAL_CONSTRAINT_H

#include <vector>
#include <map>
#include <fstream>
#include "type.h"
#include "token.h"
#include "arithmetic_expr.h"

class LocalConstraint {
    public:
    vector<Comparison> comparisonList ;

    void dump(string pathname) ;
    void retrieve(string pathname) ;
    bool check(Record record) ;
};

#endif