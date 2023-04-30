#include "arithmetic_expr.h"

// Perform binary operation on two values - with type checking
Value* combine_values_binary(Value* left_val, Value* right_val, string expr_op) {
    Value* result;

    // Handle operations for strings
    if (left_val->type == 2 && right_val->type == 2) {
        // Get left and right strings
        string left_str = ((StringValue*)left_val)->str;
        string right_str = ((StringValue*)right_val)->str;
        // Concatenation
        if (expr_op == ":") {
            string concat = left_str + right_str;
            result = (Value*)(new StringValue(concat));
        }
        // String maximum
        else if (expr_op == "max") {
            string max_str = max(left_str, right_str);
            result = (Value*)(new StringValue(max_str));
        }
        // String minimum
        else if (expr_op == "min") {
            string min_str = min(left_str, right_str);
            result = (Value*)(new StringValue(min_str));
        }
        // Unidentified string operation
        else {
            cerr << "Unidentified operation for strings \"" << expr_op << "\"\n";
            result = NULL;
        }
        return result;
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
            result = (Value*)(new FloatValue(1.0 * left_num/right_num));
        }
        else {
            float result_num;
            if (expr_op == "*") {result_num = left_num * right_num;}
            else if (expr_op == "+") {result_num = left_num + right_num;}
            else if (expr_op == "-") {result_num = left_num - right_num;}
            else if (expr_op == "max") {result_num = max(left_num, right_num);}
            else if (expr_op == "min") {result_num = min(left_num, right_num);}

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

    return result;
}

// Perform max/min/sum with possibly null inputs
Value* combine_possibly_null_values(Value* val_1, Value* val_2, string expr_op) {
    // Check for valid expr_op
    vector<string> allowed_aggs{"max", "min", "sum"};
    if (find(allowed_aggs.begin(), allowed_aggs.end(), expr_op) == allowed_aggs.end()) {
        cerr << "Null combination not supported for \"" << expr_op << "\"\n";
        return NULL;
    }
    
    // Handle null value cases
    if (val_1 == NULL && val_2 == NULL) {return NULL;}
    // One is null, return other after copying (valid for max, min, sum)
    else if (val_1 == NULL || val_2 == NULL) {
        Value* non_null = val_1;
        if (non_null == NULL) {non_null = val_2;}
        if (non_null->type == 0) {
            return (Value*)(new IntValue(((IntValue*)non_null)->num));
        }
        else if (non_null->type == 1) {
            return (Value*)(new FloatValue(((FloatValue*)non_null)->num));
        }
        else if (non_null->type == 2) {
            // Can't do sum for strings
            if (expr_op == "sum") {
                cerr << "sum not permitted for strings\n";
                return NULL;
            }
            else {
                return (Value*)(new StringValue(((StringValue*)non_null)->str));
            }
        }
    }
    // Combine for non-null inputs
    else {
        if (expr_op == "sum") {expr_op = "+";}
        return combine_values_binary(val_1, val_2, expr_op);
    }
}

// Perform aggregations on values - with type checking, skips NULL
Value* compute_aggregate(vector<Value*> values, string agg_func) {
    // Check validity of function
    if (find(aggregate_functions.begin(), aggregate_functions.end(), agg_func) == aggregate_functions.end()) {
        cerr << "Illegal aggregate function\n";
        return NULL;
    }
    // Return count directly - 0 if all nulls
    else if (agg_func == "count") {
        return (Value*)(new IntValue((int)values.size()));
    }
    // If empty, can't compute aggregates except count (done above)
    else if (values.empty()) {
        return NULL;
    }
    // Check validity of operation for strings
    else if ((agg_func == "sum" || agg_func == "mean") && values[0]->type == 2) {
        cerr << "Only MAX, MIN, COUNT aggregations available for strings";
        return NULL;
    }
    // Single value - No aggregation required, return directly
    else if ((int)values.size() == 1) {
        return values[0];
    }
    else {
        Value* aggregate = NULL;
        for (Value* value: values) {
            // For mean, use sum as aggregation, divide later
            if (agg_func == "mean") {
                aggregate = combine_possibly_null_values(aggregate, value, "sum");
            }
            else {
                aggregate = combine_possibly_null_values(aggregate, value, agg_func);
            }
        }
        // Already NULL aggregate
        if (aggregate == NULL) {return NULL;}
        // For mean, update aggregate to point to a value which is total/count
        if (agg_func == "mean") {
            float mean;
            if (aggregate->type == 0) {mean = ((IntValue*)aggregate)->num;}
            else if (aggregate->type == 1) {mean = ((FloatValue*)aggregate)->num;}
            aggregate = (Value*)(new FloatValue(1.0 * mean/(int)values.size()));
        }
        return aggregate;
    }
}

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

        result = combine_values_binary(left_val, right_val, this->expr_op);
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

// Initialize group expression class, create transition to usual expressions too
GroupedExpression::GroupedExpression(vector<string> tokens) {
    // Strip brackets from expression
    tokens = strip_brackets_from_tokens(tokens);
    this->tokens = tokens;

    pair<int, string> left_end_atomic_token = separator_expression(tokens, op_tokens, vector<string>());
    int left_end = left_end_atomic_token.first;

    // Check if first token is a [
    if (tokens[0][0] == '[') {
        // Find first ], separate about that
        int idx = 1;
        for (; idx < (int)tokens.size(); ++idx) {
            if (tokens[idx] == "]") {break;}
        }
        // The entire grouped expression is a function call
        if (idx == (int)tokens.size() - 1) {
            // Set expression details
            this->func_name = lower(tokens[0].substr(1, (int)tokens[0].size() - 1));
            vector<string> inner_expr_tokens(idx-1);
            copy(tokens.begin() + 1, tokens.begin() + idx, inner_expr_tokens.begin());
            this->aggregate_expr = new Expression(inner_expr_tokens);

            // Set group_expression attributes as null
            this->left_child = NULL;
            this->right_child = NULL;
            this->expr_op = "";
            return;
        }
        // Next index would have an operator
        else {
            left_end = idx + 1;
        }
    }

    // left_end set, so subexpressions exist
    if (left_end != -1) {
        // Copy tokens to left and right subexpressions
        vector<string> left_tokens(left_end), right_tokens((int)tokens.size() - left_end - 1);
        copy(tokens.begin(), tokens.begin() + left_end, left_tokens.begin());
        copy(tokens.begin() + left_end + 1, tokens.end(), right_tokens.begin());

        // Create the subexpressions (recursive call)
        this->left_child = new GroupedExpression(left_tokens);
        this->right_child = new GroupedExpression(right_tokens);
        
        // Set expr_op
        this->expr_op = tokens[left_end];
        this->atomic_expr_str = "";

        // Set function call attributes accordingly
        this->func_name = "";
        this->aggregate_expr = NULL;
    }
    // left_end not set, so no subexpressions and this is an atomic grouped expression (possibly the key)
    else {
        this->atomic_expr_str = tokens[0];
        // Children for atomic grouped expression are NULL
        this->expr_op = "";
        this->right_child = NULL;
        this->left_child = NULL;
        // Function for atomic group expression is NULL
        this->func_name = "";
        this->aggregate_expr = NULL;
    }
}

// Evaluate grouped expression for grouped records - recursively call grouped expressions or expressions
Value* GroupedExpression::evaluate(GroupedRecord* grouped_record) {
    Value* result;
    
    // When left and right children are present - evaluate and combine
    if (this->left_child != NULL && this->right_child != NULL) {
        // Evaluate both the children
        Value *left_val = this->left_child->evaluate(grouped_record);
        Value *right_val = this->right_child->evaluate(grouped_record);

        result = combine_values_binary(left_val, right_val, this->expr_op);
    }
    // Exactly one child present - ill formed group expression
    else if (this->left_child != NULL || this->right_child != NULL) {
        cerr << "Ill-formed group expression\n";
        result = NULL;
    }
    // Aggregation present
    else if (this->aggregate_expr != NULL) {
        // Create a vector of all individual values on which aggregation has to be performed
        vector<Value*> indiv_values;

        // Create a dummy record
        Record* dummy_rec = new Record();
        for (pair<string, Value*> key_value: grouped_record->group_keys) {
            dummy_rec->elements[key_value.first] = key_value.second;
        }
        // Iterate per record, get value of expression
        for (Record* rec: grouped_record->group_value_records) {
            for (string attr: grouped_record->group_value_attributes) {
                dummy_rec->elements[attr] = rec->elements[attr];
            }
            Value* this_rec_result = this->aggregate_expr->evaluate(dummy_rec);
            if (this_rec_result != NULL) {
                indiv_values.push_back(this_rec_result);
            }
        }

        // Compute aggregated value
        result = compute_aggregate(indiv_values, this->func_name);
    }
    // No aggregation, atomic grouped expression - one of the keys
    else if (this->aggregate_expr == NULL) {
        // Key not found
        if (grouped_record->group_keys.find(this->atomic_expr_str) == (grouped_record->group_keys).end()) {
            cerr << "Incorrect key value for GROUP BY\n";
            result = NULL;
        }
        else {
            result = grouped_record->group_keys[this->atomic_expr_str];
        }
    }
    return result;
}

void dump(GroupedExpression* grouped_expr, int depth) {
    // Base case (null-ptr)
    if (grouped_expr == NULL) {return;}
    
    // Dump for GROUPED_EXPRESSION
    cout << "Depth - " << depth << '\n';
    cout << "Grouped Expression - ";
    for (string token: grouped_expr->tokens) {cout << token << " ";}
    if (grouped_expr->left_child != NULL) {
        cout << '\n';
        // Recursive dump for children
        dump(grouped_expr->left_child, depth + 1);
        dump(grouped_expr->right_child, depth + 1);
    }
    else if (grouped_expr->aggregate_expr != NULL) {
        cout << "FUNCTION " << grouped_expr->func_name << '\n';
        // Recursive dump for expression
        dump(grouped_expr->aggregate_expr, depth + 1);
    }
    else {
        cout << "ATOMIC\n";
        return;
    }
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
    cout << "Arithmetic TEST 1 - ";
    tokens = vector<string>({"(", "num_1", "+", "num_2", ")"});
    expr = new Expression(tokens);
    result_val = (expr->evaluate(rec_1));
    if (result_val != NULL) {result_val->print(); cout << '\n';}

    // Test 2 - STRING : STRING
    cout << "Arithmetic TEST 2 - ";
    tokens = vector<string>({"(", "str_1", ":", "str_2", ")"});
    expr = new Expression(tokens);
    result_val = (expr->evaluate(rec_1));
    if (result_val != NULL) {result_val->print(); cout << '\n';}

    // Test 3 - STRING + INT
    cout << "Arithmetic TEST 3 - ";
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
    cout << "Comparison TEST 1 - ";
    tokens = vector<string>({"num_1", "+", "num_2", "<", "num_1", "-", "num_2"});
    cmpr = new Comparison(tokens);
    for (string token: tokens) {cout << token << " ";}
    cout << "----- " << (cmpr->evaluate(rec_1)) << '\n';

    // Test 2 - String = String
    cout << "Comparison TEST 2 - ";
    tokens = vector<string>({"str_1", ":", "str_2", "=", "str_3"});
    cmpr = new Comparison(tokens);
    for (string token: tokens) {cout << token << " ";}
    cout << "----- " << (cmpr->evaluate(rec_1)) << '\n';

    // Test 3 - String >< String
    cout << "Comparison TEST 3 - ";
    tokens = vector<string>({"str_1", ":", "str_2", "><", "str_4"});
    cmpr = new Comparison(tokens);
    for (string token: tokens) {cout << token << " ";}
    cout << "----- " << (cmpr->evaluate(rec_1)) << '\n';
}

void test_grouped_expr() {
    Record* rec_1 = new Record();
    rec_1->elements["num_1"] = new IntValue(94);
    rec_1->elements["num_2"] = new FloatValue(5);
    rec_1->elements["str_1"] = new StringValue("alpha");
    rec_1->elements["str_2"] = new StringValue("beta");

    Record* rec_2 = new Record();
    rec_2->elements["num_1"] = new IntValue(126);
    rec_2->elements["num_2"] = new FloatValue(6);
    rec_2->elements["str_1"] = new StringValue("gamma");
    rec_2->elements["str_2"] = new StringValue("delta");

    GroupedRecord* group_rec = new GroupedRecord();
    group_rec->group_value_attributes = vector<string>({"num_1", "num_2", "str_1", "str_2"});
    group_rec->group_keys["key_1"] = new StringValue("epsilon");
    group_rec->group_keys["key_2"] = new FloatValue(30);
    group_rec->group_value_records = vector<Record*>({rec_1, rec_2});

    GroupedExpression* expr; 
    vector<string> tokens;
    Value* result_val;

    // Test 1 - sum(float) - float
    cout << "Grouped TEST 1 - ";
    tokens = vector<string>({"[sum", "num_1", "+", "num_2", "]", "-", "key_2"});
    expr = new GroupedExpression(tokens);
    for (string token: tokens) {cout << token << " ";} cout << "----- ";
    result_val = (expr->evaluate(group_rec));
    if (result_val != NULL) {result_val->print(); cout << '\n';}

    // Test 2 - max(string:string):string
    cout << "Grouped TEST 2 - ";
    tokens = vector<string>({"[max", "str_1", ":", "str_2", "]", ":", "key_1"});
    expr = new GroupedExpression(tokens);
    for (string token: tokens) {cout << token << " ";} cout << "----- ";
    result_val = (expr->evaluate(group_rec));
    if (result_val != NULL) {result_val->print(); cout << '\n';}

    // Test 3 - count(string:string) + max(num_1-num_2*key_2)
    cout << "Grouped TEST 3 - ";
    tokens = vector<string>({"[count", "str_1", ":", "key_1", "]", "+", "[max", "num_1", "-", "num_2", "*", "key_2", "]"});
    expr = new GroupedExpression(tokens);
    for (string token: tokens) {cout << token << " ";} cout << "----- ";
    result_val = (expr->evaluate(group_rec));
    if (result_val != NULL) {result_val->print(); cout << '\n';}
}

// int main() {
//     test_arithmetic_expr();
//     test_comparison_expr();
//     test_grouped_expr();
// }