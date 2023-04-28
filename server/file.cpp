#include "file.h"

using namespace std;

void create_folder(string pathname) {
    system(("mkdir " + pathname).c_str()) ;
}

void delete_folder(string pathname) {
    system(("rm -r " + pathname).c_str()) ;
}

void  create_file(string pathname) {
    ofstream file ;
    file.open(pathname) ;
    file.close() ;
}

vector<string> create_list_from_file(string pathname) {
    vector<string> list ;

    ifstream file(pathname) ;
    string item ;
    if (file.is_open()) {
        while ( getline (file, item) )
        {
            list.push_back(item) ;
        }
        file.close();
    }
    else {
        throw pathname + " not found" ;
    }
    return list ;
}

void append_item_to_file(string pathname, string item) {
    ofstream file(pathname, std::ios::app) ;
    if (file.is_open()) {
        file << item << endl ;
        file.close() ;
    }
    else {
        throw pathname + " not found" ;
    }
}

void write_list_to_file(string pathname, vector<string> &list) {
    ofstream file(pathname, std::ios::trunc) ;
    if (file.is_open()) {
        for (string item: list) {
            file << item << endl ;
        }
        file.close() ;
    }
    else {
        throw pathname + " not found" ;
    }
}

bool check_item_in_file(string pathname, string item) {
    ifstream file(pathname) ;
    string _item ;
    if (file.is_open()) {
        while ( getline (file, _item) )
        {
            if (item == _item) {
                return true ;
            }
        }
        file.close();
    }
    else {
        throw pathname + " not found" ;
    }

    return false ;
}
