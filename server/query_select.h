#ifndef QUERY_SELECT_HEADER
#define QUERY_SELECT_HEADER

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include "query.h"
#include "arithmetic_expr.h"

using namespace std;

class Filter {
    public:
    vector<string> tokens;
    Filter *left_child, *right_child;
    vector<string> attribs;
    Comparison* atomic_filter; // One of [<, <=, >, >=, =, !=, ><] with "><" representing LIKE
    int prod_sum; // 0 if product (AND) else 1 for sum (OR)

    Filter(string str_rep);
    Filter(vector<string> tokens);
    bool check(Record* rec);
};

/*
SELECT (attributes)
    FROM (table/subquery)
    WHERE (filters)
    GROUP BY (grouping attribute)
    HAVING (condition)
*/
class SelectQuery : public Query {
    public:
    SelectQuery* from_query;
    vector<Filter*> filters;
    vector<Type> result_format;

    vector<Record> fetch();
};

vector<string> filter_tokenize(string str_query);
void dump(Filter* filter, int depth);
#endif