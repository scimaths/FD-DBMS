#ifndef GLOBAL_CONSTRAINT_H
#define GLOBAL_CONSTRAINT_H

#include <vector>
#include <map>
#include <fstream>
#include "type.h"
#include "token.h"
#include "arithmetic_expr.h"
#include "func_dep.h"
#include "file.h"
#include "data.h"

class GlobalConstraint {
    public:
    vector<FuncDependancy> fdList ;

    void retrieve(string pathname) ;
    void add_constraint(string pathname, string fd_str) ;
};

#endif