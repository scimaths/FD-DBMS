#include "query_select.h"

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

// Initialize Filter Class - built tree for filter (with comparisons and expressions)
Filter::Filter(vector<string> tokens) {
    // Strip brackets from filter
    tokens = strip_brackets_from_tokens(tokens);
    this->tokens = tokens;

    // Get left_end and atomic token at top level
    pair<int, string> left_end_atomic_token = separator_expression(tokens, or_and_tokens, cmp_tokens);
    int left_end = left_end_atomic_token.first;
    string cmp_token = left_end_atomic_token.second;

    // left_end set, so subfilters exist
    if (left_end != -1) {
        // Copy tokens to left and right subfilters
        vector<string> left_tokens(left_end), right_tokens((int)tokens.size() - left_end - 1);
        copy(tokens.begin(), tokens.begin() + left_end, left_tokens.begin());
        copy(tokens.begin() + left_end + 1, tokens.end(), right_tokens.begin());

        // Create the subfilters (recursive call)
        this->left_child = new Filter(left_tokens);
        this->right_child = new Filter(right_tokens);
        
        // Set prod_sum
        if (tokens[left_end] == "&&") {this->prod_sum = 0;}
        else if (tokens[left_end] == "||") {this->prod_sum = 1;}
        this->atomic_filter = NULL;
    }
    // left_end not set, so no subfilters and this is an atomic filter
    else {
        // cmp_token contains the comparison for this filter
        this->atomic_filter = new Comparison(tokens);
        this->prod_sum = -1;
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

// Print filter tree for visualization and verification
void dump(Filter* filter, int depth) {
    // Base case (null-ptr)
    if (filter == NULL) {return;}
    
    // Dump for FILTER
    cout << "Depth - " << depth << '\n';
    cout << "Filter - ";
    for (string token: filter->tokens) {cout << token << " ";}
    cout << '\n';
    // Recursive dump for children
    dump(filter->left_child, depth + 1);
    dump(filter->right_child, depth + 1);
    
    // Dump for atomic filters
    if (filter->atomic_filter != NULL) {
        dump(filter->atomic_filter, depth + 1);
    }
}

// Initializing atomic subquery
SelectQuery::SelectQuery(string db, string table) {
    this->atomic_query_db = db;
    this->atomic_query_table = table;
}

// TODO - Deal with JOIN
vector<Record*> SelectQuery::fetch() {
    vector<Record*> recordList;
    if (this->subquery == NULL && this->join == NULL) {
        TableData td = TableData("db/" + this->atomic_query_db + "/" + this->atomic_query_table);
        recordList = td.recordList;
    }
    else if (this->join == NULL) {
        recordList = this->subquery->fetch();
    }
    else if (this->subquery == NULL) {
        // TODO
    }

    vector<Record*> result;
    for (Record* rec: recordList) {
        bool valid = true;
        for (Filter* filter: this->filters) {
            // Found a filter where mismatch occurs
            if (!filter->check(rec)) {
                valid = false;
                break;
            }
        }
        
        Record* result_rec = new Record();
        for (pair<string, Expression*> attr_exp: this->attributes) {
            result_rec->elements[attr_exp.first] = (attr_exp.second)->evaluate(rec);
        }
        result.push_back(result_rec);
    }
    return result;
}

// int main() {
//     Filter* filter = new Filter("(a.b=3 && c.d+e.f-k><4) || (l.m*k >= 2 || (n != 4)) && x.y <= 3");
//     dump(filter, 0);
// }