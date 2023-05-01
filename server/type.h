#ifndef TYPE_HEADER
#define TYPE_HEADER

#include <string> 
#include <iostream> 
#include <vector> 
#include <stack>
#include <map>
#include <algorithm>

using namespace std;

class Type {
    public:
    string type ; // INT, 
    int size ;

    Type(string type) ;
} ;

class Value {
    public:
    int type;

    virtual void print() = 0;
    virtual string get_string() = 0;
};

class IntValue : public Value {
    public:
    int num;

    IntValue(int num);
    void print();
    string get_string();
};

class StringValue : public Value {
    public:
    string str;
    int length;

    StringValue(string str);
    void print();
    string get_string();
};

class FloatValue : public Value {
    public:
    float num;
    
    FloatValue(float num);
    void print();
    string get_string();
};

class Record {
    public:
    map<string, Value*> elements;
};

class GroupedRecord {
    public:
    map<string, Value*> group_keys;
    vector<string> group_value_attributes;
    vector<Record*> group_value_records;
};

string lower(string str);
string join(vector<string> tokens, string join_str);
bool is_equal(Value* val_1, Value* val_2);
void print_records(vector<Record*> records);
string stringify_records(vector<Record*> records);
Value* numerical_str_to_value(string str);
vector<string> strip_brackets_from_tokens(vector<string> tokens) ;
pair<int, string> separator_expression(vector<string> tokens, vector<string> separator_strs, vector<string> expression_ops) ;

const vector<char> brkt_tokens{'(', ')'};
const vector<char> cmp_char{'<', '>', '!', '='};
const vector<string> cmp_tokens{"<=", ">=", "!=", "><", "<", ">", "="};
const vector<string> or_and_tokens{"||", "&&"};
const vector<string> op_tokens{"+", "-", "/", "*", ":"};
const float MIN_DIFF = 1e-9;
const vector<string> aggregate_functions{"max", "min", "count", "mean", "sum"};
#endif