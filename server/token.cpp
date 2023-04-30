#include "token.h"

vector<string> tokenize(string str, string delim) {
    vector<string> tokenList ;
    string token = "" ;
   
   /* walk through other tokens */
    for (char c: str) {
        if (!count(delim.begin(), delim.end(), c)) {
            token += c ;
        }
        else {
            if (token != "") {
                tokenList.push_back(token) ;
                token = "" ;
            }
        }
    }
    if (token != "") {
        tokenList.push_back(token) ;
        token = "" ;
    }
    return tokenList ;
}