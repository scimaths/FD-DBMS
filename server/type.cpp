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