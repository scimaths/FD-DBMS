#ifndef QUERY_SELECT_HEADER
#define QUERY_SELECT_HEADER

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include "query.h"
#include "data.h"
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
(SELECT (attributes) FROM ...)
    JOIN
(SELECT (attributes) FROM ...)
    ON (filters)
*/
class Join {
    public:
    SelectQuery *left_query, *right_query;
    map<string, Expression*> attributes;
    vector<Filter*> filters;
    string join_type;
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
    SelectQuery* subquery;
    Join* join;
    vector<Filter*> filters;
    map<string, Expression*> attributes;

    string atomic_query_db; // For atomic query
    string atomic_query_table; // For atomic query

    SelectQuery(string db, string table);
    vector<Record*> fetch();
};

vector<string> filter_tokenize(string str_query);
void dump(Filter* filter, int depth);
#endif