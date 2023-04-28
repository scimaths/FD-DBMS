#include <fstream>
#include <vector>

using namespace std;

void create_folder(string pathname) ;
void delete_folder(string pathname) ;

void create_file(string pathname) ;

vector<string> create_list_from_file(string pathname) ;
void append_item_to_file(string pathname, string item) ;
void write_list_to_file(string pathname, vector<string> &list) ;
bool check_item_in_file(string pathname, string item) ;