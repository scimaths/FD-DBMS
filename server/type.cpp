#include "type.h"

Type::Type(string type) {
    if (type == "Int" ) {
        this->type = type ;
        size = 4 ; 
    }
    else if (type == "Float" ) {
        this->type = type ;
        size = 4 ; // ?
    }
    else if (type == "String" ) {
        this->type = type ;
        size = 4 ; // ?
    }
}

IntValue::IntValue(int num) {
    this->num = num;
    this->type = 0;
}

StringValue::StringValue(string str) {
    this->str = str;
    this->type = 1;
}

FloatValue::FloatValue(float num) {
    this->num = num;
    this->type = 2;
}

vector<string> strip_brackets_from_tokens(vector<string> tokens) {
    // Remove surrounding brackets
    stack<int> st;
    vector<int> brack_nums;
    int brack_num = 0;
    // For each pair of brackets, give a unique identifier (integer)
    for (string token: tokens) {
        // Add new entry to stack
        if (token == "(") {
            brack_num += 1;
            st.push(brack_num);
            brack_nums.push_back(brack_num);
        }
        // Pop first element off stack
        else if (token == ")") {
            brack_nums.push_back(st.top());
            st.pop();
        }
    }

    // Remove brackets from start and end
    int start = 0;
    while (true) {
        // Proceed inwards till outside brackets match
        if (
            tokens[start] == "(" && tokens[(int)tokens.size() - 1 - start] == ")"
            &&
            brack_nums[start] == brack_nums[(int)brack_nums.size() - 1 - start]
            &&
            (int)brack_nums.size() - 1 - start > start
        ) {start++;}
        else {break;}
    }
    // Copy squeezed tokens vector
    vector<string> tokens_squeezed(tokens.size() - 2*start);
    copy(tokens.begin() + start, tokens.end() - start, tokens_squeezed.begin());
    return tokens_squeezed;
}

// Find index where first subexpression (at highest level) ends (wrt separator_strs)
pair<int, string> separator_expression(vector<string> tokens, vector<string> separator_strs, vector<string> expression_ops) {
    // Handle sub-filters
    int depth = 0, cnt = 0, left_start = 0, left_end = -1;
    string cmp_token = "";
    for (string token: tokens) {
        cnt += 1;
        // Increase depth on seeing (
        if (token == "(") {depth++;}
        // Decrease depth on seeing )
        else if (token == ")") {depth--;}
        // Subfilter without brackets
        else if (
            find(separator_strs.begin(), separator_strs.end(), token) != separator_strs.end()
            &&
            depth == 0
        ) {
            left_end = cnt - 1;
            break;
        }
        // For atomic filter, set cmp_token (reached for atomic filter)
        else if (find(expression_ops.begin(), expression_ops.end(), token) != expression_ops.end()) {
            cmp_token = token;
        }
    }
    return make_pair(left_end, cmp_token);
}