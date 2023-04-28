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