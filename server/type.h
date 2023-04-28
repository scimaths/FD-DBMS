#include <string> 

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