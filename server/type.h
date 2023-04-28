#ifndef TYPE_HEADER
#define TYPE_HEADER

#include <string> 
#include <vector> 
#include <stack>
#include <map>
#include <algorithm>

using namespace std;

class Type {
    public:
    string type ;
    int size ;

    Type(string type) ;
} ;

class Value {
    public:
    int type;
};

class IntValue : public Value {
    public:
    int num;

    IntValue(int num);
};

class StringValue : public Value {
    public:
    string str;
    int length;

    StringValue(string str);
};

class FloatValue : public Value {
    public:
    float num;
    
    FloatValue(float num);
};

class Record {
    public:
    map<string, Value*> elements;
};

vector<string> strip_brackets_from_tokens(vector<string> tokens);
pair<int, string> separator_expression(vector<string> tokens, vector<string> separator_strs, vector<string> expression_ops);

const vector<char> brkt_tokens{'(', ')'};
const vector<char> cmp_char{'<', '>', '!', '='};
const vector<string> cmp_tokens{"<=", ">=", "!=", "><", "<", ">", "="};
const vector<string> or_and_tokens{"||", "&&"};
const vector<string> op_tokens{"+", "-", "/", "*", ":"};
#endif