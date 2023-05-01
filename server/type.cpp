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

FloatValue::FloatValue(float num) {
    this->num = num;
    this->type = 1;
}

StringValue::StringValue(string str) {
    this->str = str;
    this->type = 2;
}

void IntValue::print() {cout << this->num;}
void StringValue::print() {cout << this->str;}
void FloatValue::print() {cout << this->num;}

string IntValue::get_string() {return to_string(this->num);}
string StringValue::get_string() {return this->str;}
string FloatValue::get_string() {return to_string(this->num);}

string lower(string str) {
    string result = "";
    for (char c: str) {
        if (c >= 'A' && c <= 'Z') {result += 'a' + (c - 'A');}
        else {result += c;}
    }
    return result;
}

string join(vector<string> tokens, string join_str) {
    string result = tokens[0];
    for (int idx = 1; idx <= (int)tokens.size(); ++idx) {
        result += join_str + tokens[idx];
    }
    return result;
}

bool is_equal(Value* val_1, Value* val_2) {
    // Check for nullity
    if (val_1 == NULL || val_2 == NULL) {return false;}
    // Check if types match
    if (val_1->type != val_2->type) {return false;}
    // Check type-wise
    if (val_1->type == 0) {
        return ((IntValue*)val_1)->num == ((IntValue*)val_2)->num;
    }
    else if (val_1->type == 1) {
        return ((FloatValue*)val_1)->num == ((FloatValue*)val_2)->num;
    }
    else if (val_1->type == 2) {
        return ((StringValue*)val_1)->str == ((StringValue*)val_2)->str;
    }
}

void print_records(vector<Record*> records) {
    for (Record* rec: records) {
        for (pair<string, Value*> rec_val: rec->elements) {
            cout << "|" << rec_val.first << " "; rec_val.second->print(); cout << "| ";
        }
        cout << '\n';
    }
}

string stringify_records(vector<Record*> records) {
    if (records.empty()) {
        return "Empty Table";
    }
    vector<string> keys;
    vector<int> string_sizes;
    for (pair<string, Value*> rec_val: records[0]->elements) {
        keys.push_back(rec_val.first);
        string_sizes.push_back(rec_val.first.size());
    }
    vector<vector<string>> record_strings;
    for (Record* rec: records) {
        vector<string> record_string;
        int index = 0;
        for (pair<string, Value*> rec_val: rec->elements) {
            string record_elem = rec_val.second->get_string();
            record_string.push_back(record_elem);
            string_sizes[index] = max(string_sizes[index], (int)record_elem.size());
            index += 1;
        }
        record_strings.push_back(record_string);
    }

    string output = "|";
    int index = 0;
    for (auto key: keys) {
        output += key + string(string_sizes[index] - (int)key.size() + 1, ' ') + "|";
        index += 1;
    }
    int curr_size = output.size();
    output = string(curr_size, '-') + "\n" + output + "\n" + string(curr_size, '-');

    for (vector<string> record_string: record_strings) {
        output += "\n";
        index = 0;
        output += "|";
        for (string record_elem: record_string) {
            output += record_elem + string(string_sizes[index] - (int)record_elem.size() + 1, ' ') + "|";
            index += 1;
        }
    }
    output += "\n" + string(curr_size, '-');
    return output;
}

Value* numerical_str_to_value(string str) {
    bool decimal_pres = 0;
    for (char c: str) {if (c == '.') {decimal_pres = 1;}}
    if (decimal_pres) {
        return (Value*)(new FloatValue(stof(str)));
    }
    else {
        return (Value*)(new IntValue(stoi(str)));
    }
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