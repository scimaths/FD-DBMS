#include <string>
#include <vector>
#include <stack>
#include <map>
#include <algorithm>
#include <iostream>
#include "query.h"

using namespace std;

class Record {
    public:
    map<string, Value*> elements;
};

class Filter {
    public:
    vector<string> tokens;
    Filter *left_child, *right_child;
    vector<string> attribs;
    string filter_op; // One of [<, <=, >, >=, =, !=, ><] with "><" representing LIKE
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

    pair<bool, Record*> fetch();
};

vector<string> filter_tokenize(string str_query);
void dump(Filter* filter, int depth);