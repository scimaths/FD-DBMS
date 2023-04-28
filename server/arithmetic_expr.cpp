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

Value* Expression::evaluate(Record* record) {

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

bool Comparison::evaluate(Record* record) {
    
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