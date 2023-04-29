#ifndef ARITHMETIC_EXPRESSION_HEADER
#define ARITHMETIC_EXPRESSION_HEADER

#include <string>
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
    Value* evaluate(Record& rec);
};

class Comparison {
    public:
    vector<string> tokens;
    string filter_op; // One of [<, <=, >, >=, =, !=, ><] with "><" representing LIKE
    Expression *left_expr, *right_expr;

    Comparison(vector<string> tokens);
    bool evaluate(Record& rec);
};

void dump(Expression* expression, int depth);
void dump(Comparison* comparison, int depth);
#endif