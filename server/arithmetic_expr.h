#ifndef ARITHMETIC_EXPRESSION_HEADER
#define ARITHMETIC_EXPRESSION_HEADER

#include <string>
#include <regex>
#include <cmath>
#include <vector>
#include <iostream>
#include "type.h"

using namespace std;

class Expression {
    public:
    vector<string> tokens;
    string atomic_expr_str; // Name of column if atomic expression
    string expr_op; // One of [+, -, *, /, ":"]
    Expression *left_child, *right_child;

    Expression(vector<string> tokens);
    Value* evaluate(Record* rec);
    string get_name();
};

class GroupedExpression {
    public:
    vector<string> tokens;

    // For arithmetic expressions on grouped results
    string expr_op;
    GroupedExpression *left_child, *right_child;

    // For calls to usual expressions on groups
    string func_name;
    Expression *aggregate_expr;

    // For atomic calls (like key to group)
    string atomic_expr_str;

    GroupedExpression(vector<string> tokens);
    Value* evaluate(GroupedRecord* rec);
};

class Comparison {
    public:
    vector<string> tokens;
    string filter_op; // One of [<, <=, >, >=, =, !=, ><] with "><" representing LIKE
    Expression *left_expr, *right_expr;

    Comparison(vector<string> tokens);
    bool evaluate(Record* rec);
};

class GroupedComparison {
    public:
    vector<string> tokens;
    string filter_op; // One of [<, <=, >, >=, =, !=, ><] with "><" representing LIKE
    GroupedExpression *left_expr, *right_expr;

    GroupedComparison(vector<string> tokens);
    bool evaluate(GroupedRecord* rec);
};

Value* combine_values_binary(Value* left_val, Value* right_val, string expr_op);
Value* compute_aggregate(vector<Value*> values, string agg_func);
Value* combine_possibly_null_values(Value* val_1, Value* val_2, string expr_op);
void dump(Expression* expression, int depth);
void dump(Comparison* comparison, int depth);
void dump(GroupedExpression* group_expression, int depth);
void dump(GroupedComparison* group_comparison, int depth);
#endif