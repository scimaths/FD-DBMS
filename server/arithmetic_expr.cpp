#include "arithmetic_expr.h"

// Initialize expression class, create expression trees
Expression::Expression(vector<string> tokens) {
    // Strip brackets from expression
    tokens = strip_brackets_from_tokens(tokens);
    this->tokens = tokens;

    // Handle sub-expressions
    pair<int, string> left_end_atomic_token = separator_expression(tokens, op_tokens, vector<string>());
    int left_end = left_end_atomic_token.first;

    // left_end set, so subexpressions exist
    if (left_end != -1) {
        // Copy tokens to left and right subexpressions
        vector<string> left_tokens(left_end), right_tokens((int)tokens.size() - left_end - 1);
        copy(tokens.begin(), tokens.begin() + left_end, left_tokens.begin());
        copy(tokens.begin() + left_end + 1, tokens.end(), right_tokens.begin());

        // Create the subexpressions (recursive call)
        this->left_child = new Expression(left_tokens);
        this->right_child = new Expression(right_tokens);
        
        // Set expr_op
        this->expr_op = tokens[left_end];
        this->atomic_expr_str = "";
    }
    // left_end not set, so no subexpressions and this is an atomic expression
    else {
        // cmp_token contains the comparison for this expression
        this->atomic_expr_str = tokens[0];
        this->expr_op = "";
        this->right_child = NULL;
        this->left_child = NULL;
    }
}

// Evaluate expression trees recursively for +-*/:
Value* Expression::evaluate(Record* record) {
    Value* result;
    // Both children present
    if (this->left_child != NULL && this->right_child != NULL) {
        // Evaluate both the children
        Value *left_val = this->left_child->evaluate(record);
        Value *right_val = this->right_child->evaluate(record);

        // String concatenation
        if (expr_op == ":") {
            if (left_val->type != 2 || right_val->type != 2) {
                cerr << "Incorrect types for concat\n";
                return NULL;
            }
            string concat = ((StringValue*)left_val)->str + ((StringValue*)right_val)->str;
            result = (Value*)(new StringValue(concat));
        }
        else {
            // Numerical operations with strings - ERROR
            if (left_val->type == 2 || right_val->type == 2) {
                cerr << "Incorrect types for " << expr_op << '\n';
                return NULL;
            }

            // Get left and right numbers as floats
            float left_num, right_num;
            if (left_val->type == 0) {left_num = ((IntValue*)left_val)->num;}
            else if (left_val->type == 1) {left_num = ((FloatValue*)left_val)->num;}
            if (right_val->type == 0) {right_num = ((IntValue*)right_val)->num;}
            else if (right_val->type == 1) {right_num = ((FloatValue*)right_val)->num;}

            // For division, result is always float & check division by 0
            if (expr_op == "/") {
                if (right_num == 0) {
                    cerr << "Division by zero error\n";
                    return NULL;
                }
                result = (Value*)(new FloatValue(left_num/right_num));
            }
            else {
                float result_num;
                if (expr_op == "*") {result_num = left_num * right_num;}
                else if (expr_op == "+") {result_num = left_num + right_num;}
                else if (expr_op == "-") {result_num = left_num - right_num;}

                // For +-*, if both are INTs, return is also an INT
                if (left_val->type == 0 && right_val->type == 0) {
                    result = (Value*)(new IntValue(result_num));
                }
                // One argument is FLOAT, return is a FLOAT
                else {
                    result = (Value*)(new FloatValue(result_num));
                }
            }
        }
    }
    // One child is not null, not both
    else if (this->left_child != NULL || this->right_child != NULL) {
        cerr << "Ill-formed expression";
        return NULL;
    }
    // Atomic expression (fetch record column directly)
    else {
        result = record->elements[atomic_expr_str];
    }
    return result;
}

// Print expression tree for visualization and verification
void dump(Expression* expression, int depth) {
    // Base case (null-ptr)
    if (expression == NULL) {return;}
    
    // Dump for EXPRESSION
    cout << "Depth - " << depth << '\n';
    cout << "Expression - ";
    for (string token: expression->tokens) {cout << token << " ";}
    cout << '\n';
    // Recursive dump for children
    dump(expression->left_child, depth + 1);
    dump(expression->right_child, depth + 1);
}

// Initialize comparison class, create 1-level comparison trees
Comparison::Comparison(vector<string> tokens) {
    // Update tokens
    this->tokens = tokens;

    // Iterate over the tokens array, find the index with cmp_token
    int cmp_index = 0;
    for (; cmp_index < (int)tokens.size(); ++cmp_index) {
        if (find(cmp_tokens.begin(), cmp_tokens.end(), tokens[cmp_index]) != cmp_tokens.end()) {
            break;
        }
    }

    // Create root filter operation
    this->filter_op = tokens[cmp_index];

    // Set left and right child expressions
    vector<string> left_tokens(cmp_index), right_tokens((int)tokens.size() - cmp_index - 1);
    copy(tokens.begin(), tokens.begin() + cmp_index, left_tokens.begin());
    copy(tokens.begin() + cmp_index + 1, tokens.end(), right_tokens.begin());
    this->left_expr = new Expression(left_tokens);
    this->right_expr = new Expression(right_tokens);
}

// Evaluate a comparison
bool Comparison::evaluate(Record* record) {
    Value* left_val = this->left_expr->evaluate(record);
    Value* right_val = this->right_expr->evaluate(record);

    // Check equality/inequality for strings
    if (
        (filter_op == "!=" || filter_op == "=")
        &&
        left_val->type == 2 && right_val->type == 2
    ) {
        string left_str = ((StringValue*)left_val)->str;
        string right_str = ((StringValue*)right_val)->str;
        if (filter_op == "=") {
            return left_str == right_str;
        }
        else if (filter_op == "!=") {
            return left_str != right_str;
        }
    }
    // >< - LIKE operator for regex matching with strings
    else if (filter_op == "><") {
        // Check if arguments are strings
        if (left_val->type != 2 || right_val->type != 2) {
            cerr << "Illegal comparison - >< requires two strings\n";
            return 0;
        }
        else {
            regex regex_cmp(((StringValue*)right_val)->str);
            return regex_match(
                ((StringValue*)left_val)->str,
                regex_cmp
            );
        }
    }
    // One string but all string operators used already
    else if (left_val->type == 2 || right_val->type == 2) {
        cerr << "Illegal comparison - Only = != >< available for string comparison\n";
        return 0;
    }
    else {
        // Get left and right numbers as floats
        float left_num, right_num;
        if (left_val->type == 0) {left_num = ((IntValue*)left_val)->num;}
        else if (left_val->type == 1) {left_num = ((FloatValue*)left_val)->num;}
        if (right_val->type == 0) {right_num = ((IntValue*)right_val)->num;}
        else if (right_val->type == 1) {right_num = ((FloatValue*)right_val)->num;}

        // Add individual comparison operators
        if (filter_op == "<=") {
            return left_num <= right_num;
        }
        else if (filter_op == "<") {
            return left_num < right_num;
        }
        else if (filter_op == ">=") {
            return left_num >= right_num;
        }
        else if (filter_op == ">") {
            return left_num > right_num;
        }
        else if (filter_op == "=") {
            return fabs(left_num - right_num) < MIN_DIFF;
        }
        else if (filter_op == "!=") {
            return fabs(left_num - right_num) > MIN_DIFF;
        }
        else {
            cerr << "Unidentified comparison operator\n";
            return false;
        }
    }
}

// Print comparison tree (depth = 1)
void dump(Comparison* comparison, int depth) {
    // Dump for COMPARISON
    cout << "Depth - " << depth << '\n';
    cout << "Comparison - ";
    for (string token: comparison->tokens) {cout << token << " ";}
    cout << '\n';
    // Dump for child expressions
    dump(comparison->left_expr, depth + 1);
    dump(comparison->right_expr, depth + 1);
}

void test_arithmetic_expr() {
    Record* rec_1 = new Record();
    rec_1->elements["num_1"] = new IntValue(3);
    rec_1->elements["num_2"] = new FloatValue(5);
    rec_1->elements["str_1"] = new StringValue("alpha");
    rec_1->elements["str_2"] = new StringValue("beta");

    Expression* expr; 
    vector<string> tokens;
    Value* result_val;

    // Test 1 - INT + INT
    tokens = vector<string>({"(", "num_1", "+", "num_2", ")"});
    expr = new Expression(tokens);
    result_val = (expr->evaluate(rec_1));
    if (result_val != NULL) {result_val->print(); cout << '\n';}

    // Test 2 - STRING : STRING
    tokens = vector<string>({"(", "str_1", ":", "str_2", ")"});
    expr = new Expression(tokens);
    result_val = (expr->evaluate(rec_1));
    if (result_val != NULL) {result_val->print(); cout << '\n';}

    // Test 3 - STRING + INT
    tokens = vector<string>({"(", "str_1", "+", "num_1", ")"});
    expr = new Expression(tokens);
    result_val = (expr->evaluate(rec_1));
    if (result_val != NULL) {result_val->print(); cout << '\n';}
}

void test_comparison_expr() {
    Record* rec_1 = new Record();
    rec_1->elements["num_1"] = new IntValue(3);
    rec_1->elements["num_2"] = new FloatValue(5);
    rec_1->elements["str_1"] = new StringValue("alpha");
    rec_1->elements["str_2"] = new StringValue("beta");
    rec_1->elements["str_3"] = new StringValue("alphabeta");
    rec_1->elements["str_4"] = new StringValue("(.*)abe(.*)");

    Comparison* cmpr; 
    vector<string> tokens;
    bool result_val;

    // Test 1 - Int < Int
    tokens = vector<string>({"num_1", "+", "num_2", "<", "num_1", "-", "num_2"});
    cmpr = new Comparison(tokens);
    for (string token: tokens) {cout << token << " ";}
    cout << "----- " << (cmpr->evaluate(rec_1)) << '\n';

    // Test 2 - String = String
    tokens = vector<string>({"str_1", ":", "str_2", "=", "str_3"});
    cmpr = new Comparison(tokens);
    for (string token: tokens) {cout << token << " ";}
    cout << "----- " << (cmpr->evaluate(rec_1)) << '\n';

    // Test 3 - String >< String
    tokens = vector<string>({"str_1", ":", "str_2", "><", "str_4"});
    cmpr = new Comparison(tokens);
    for (string token: tokens) {cout << token << " ";}
    cout << "----- " << (cmpr->evaluate(rec_1)) << '\n';
}

int main() {
    test_arithmetic_expr();
    test_comparison_expr();
}