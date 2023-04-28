#include "query_select.h"

vector<char> brkt_tokens{'(', ')'};
vector<char> cmp_char{'<', '>', '!', '='};
vector<string> cmp_tokens{"<=", ">=", "!=", "><", "<", ">", "="};
vector<string> or_and_tokens{"||", "&&"};
vector<string> op_tokens{"+", "-", "/", "*", ":"};

// Tokenize query filter substring into tokens (for parsing of filters and expressions)
vector<string> filter_tokenize(string str_query) {
    vector<string> tokens;
    
    string token = "";

    for (int i=0; i<(int)str_query.size(); ++i) {
        // Look for parentheses symbols - () or operation tokens - +-*/:
        if (
            find(brkt_tokens.begin(), brkt_tokens.end(), str_query[i]) != brkt_tokens.end()
            ||
            find(op_tokens.begin(), op_tokens.end(), string(1, str_query[i])) != op_tokens.end()
        ) {
            // Push previous token
            if (token.size() > 0) {
                tokens.push_back(token);
            }
            // Add ()+-*/:
            token = string(1, str_query[i]);
            tokens.push_back(token);
            // Empty current token
            token = "";
        }
        // Check for comparison tokens
        else if (find(cmp_char.begin(), cmp_char.end(), str_query[i]) != cmp_char.end()) {
            // Push previous token
            if (token.size() > 0) {
                tokens.push_back(token);
                token = "";
            }
            // Iterate over all string-tokens (to account for > 1 character), match should exist
            for (string cmp_token: cmp_tokens) {
                // Too large a cmp_token to come up next
                if ((int)cmp_token.size() - 1 > (int)str_query.size() - i - 1) {
                    continue;
                }
                // Found the token in the next few characters
                if (cmp_token == str_query.substr(i, cmp_token.size())) {
                    tokens.push_back(cmp_token);
                    i += (int)cmp_token.size() - 1; // Jump length-1 places
                    break;
                }
            }
        }
        // Check for ||, &&
        else if ((int)str_query.size() - i - 1 >= 1 && (str_query[i] == '|' || str_query[i] == '&')) {
            // Push previous token
            if (token.size() > 0) {
                tokens.push_back(token);
                token = "";
            }
            string combined = str_query.substr(i, 2);
            if (find(or_and_tokens.begin(), or_and_tokens.end(), combined) != or_and_tokens.end()) {
                tokens.push_back(combined);
                i += 1; // Jump one place
            }
        }
        // End of a token
        else if (str_query[i] == ' ') {
            // Push previous token
            if (token.size() > 0) {
                tokens.push_back(token);
                token = "";
            }
        }
        else {
            token += str_query[i];
        }
    }

    if (token.size() > 0) {
        tokens.push_back(token);
    }
    return tokens;
}

// Print filter tree for visualization and verification
void dump(Filter* filter, int depth) {
    if (filter == NULL) {return;}
    cout << "Depth - " << depth << '\n';
    cout << "Filter - ";
    for (string token: filter->tokens) {cout << token << " ";}
    cout << '\n';
    dump(filter->left_child, depth + 1);
    dump(filter->right_child, depth + 1);
}

// TODO - Handle operation tree
Filter::Filter(vector<string> tokens) {
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
    this->tokens = tokens_squeezed;

    // Handle sub-filters
    int depth = 0, cnt = 0, left_start = 0, left_end = -1;
    string cmp_token;
    for (string token: tokens) {
        cnt += 1;
        // Increase depth on seeing (
        if (token == "(") {depth++;}
        // Decrease depth on seeing )
        else if (token == ")") {depth--;}
        // Subfilter without brackets
        else if (
            find(or_and_tokens.begin(), or_and_tokens.end(), token) != or_and_tokens.end()
            &&
            depth == 0
        ) {
            left_end = cnt - 1;
            break;
        }
        // For atomic filter, set cmp_token (reached for atomic filter)
        else if (find(cmp_tokens.begin(), cmp_tokens.end(), token) != cmp_tokens.end()) {
            cmp_token = token;
        }
    }

    // left_end set, so subfilters exist
    if (left_end != -1) {
        // Copy tokens to left and right subfilters
        vector<string> left_tokens(left_end), right_tokens((int)tokens.size() - left_end - 1);
        copy(tokens.begin() + left_start, tokens.begin() + left_end, left_tokens.begin());
        copy(tokens.begin() + left_end + 1, tokens.end(), right_tokens.begin());

        // Create the subfilters (recursive call)
        this->left_child = new Filter(left_tokens);
        this->right_child = new Filter(right_tokens);
        
        // Set prod_sum
        if (tokens[left_end] == "&&") {this->prod_sum = 0;}
        else if (tokens[left_end] == "||") {this->prod_sum = 1;}
        this->filter_op = "--";
    }
    // left_end not set, so no subfilters and this is an atomic filter
    else {
        // cmp_token contains the comparison for this filter
        this->filter_op = cmp_token;
        this->right_child = NULL;
        this->left_child = NULL;
    }
}

// Proxy for filter instantiation
// Tokenize string to tokens and pass to Filter(tokens) function
Filter::Filter(string str_rep) : Filter(filter_tokenize(str_rep)) {}

// TODO - Check for atomic filters
bool Filter::check(Record* rec) {
    if (!this->right_child && !this->left_child) {
        // TODO
        return false;
    }
    else {
        if (this->prod_sum == 0) {
            return this->right_child->check(rec) && this->left_child->check(rec);
        }
        else {
            return this->right_child->check(rec) || this->left_child->check(rec);
        }
    }
}

// TODO - Implement fetch with filters
pair<bool, Record*> SelectQuery::fetch() {

}

int main() {
    Filter* filter = new Filter("(a.b=3 && c.d+e.f-k><4) || (l.m*k >= 2 || (n != 4)) && x.y <= 3");
    dump(filter, 0);
}