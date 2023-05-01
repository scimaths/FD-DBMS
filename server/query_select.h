#ifndef QUERY_SELECT_HEADER
#define QUERY_SELECT_HEADER

#include <string>
#include <cstring>
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

class GroupedFilter {
    public:
    vector<string> tokens;
    GroupedFilter *left_child, *right_child;
    vector<string> attribs;
    GroupedComparison* atomic_filter; // One of [<, <=, >, >=, =, !=, ><] with "><" representing LIKE
    int prod_sum; // 0 if product (AND) else 1 for sum (OR)

    GroupedFilter(string str_rep);
    GroupedFilter(vector<string> tokens);
    bool check(GroupedRecord* rec);
};

class SelectQuery;

/*
(SELECT (attributes) FROM ...)
    JOIN
(SELECT (attributes) FROM ...)
    ON (filters)
*/
class Join {
    public:
    SelectQuery *left_query, *right_query;
    // map<string, Expression*> attributes;
    vector<Filter*> filters;
    string join_type;
    string db;

    Join(string join_query, string db);
    vector<Record*> fetch();
    void dump(int depth);
};

/*
SELECT (attributes) AS (names)
    FROM (table/subquery)
    WHERE (filters)
    GROUPBY (grouping attribute)
    HAVING (condition)
*/
class SelectQuery : public Query {
    public:
    SelectQuery *subquery;
    Join* join;
    vector<Filter*> filters; // WHERE
    vector<Expression*> grouping_keys; // GROUP BY
    vector<GroupedFilter*> grouped_filters; // HAVING
    map<string, Expression*> non_group_attributes; // ATTRIBUTES
    map<string, GroupedExpression*> group_attributes; // ATTRIBUTES
    bool group_by;

    string atomic_query_db; // For atomic query
    string atomic_query_table; // For atomic query

    SelectQuery(string select_query, string db);
    vector<Record*> fetch();
    void dump(int depth);
};

vector<string> filter_tokenize(string str_query);
vector<string> select_query_tokenize(string select_query);
void dump(Filter* filter, int depth);
#endif