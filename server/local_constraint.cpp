#include "local_constraint.h"

void LocalConstraint::dump(string pathname) {
    ofstream file(pathname+"/local_constraints.txt", std::ios::trunc) ;
    if (file.is_open()) {
        for (Comparison comparison: comparisonList) {
            for (string token: comparison.tokens) {
                file << token << " " ;
            }
            file << "\n" ;
        }
    }
    else {
        throw pathname+"/local_constraints.txt" + " not found" ;
    }
}

void LocalConstraint::retrieve(string pathname) {
    ifstream file(pathname+"/local_constraints.txt") ;
    string item ;
    if (file.is_open()) {
        while(getline (file, item)) {
            vector<string> tokens = tokenize(item, " ") ;
            comparisonList.push_back(Comparison(tokens)) ;
        }
        file.close();
    }
    else {
        throw pathname + "/local_constraints.txt" + " not found" ;
    }
}

bool LocalConstraint::check(Record record) {
    for (Comparison comparison: comparisonList) {
        if(!comparison.evaluate(record)) return false ;
    }
    return true ;
} 