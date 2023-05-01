#include "query_select.h"

// Tokenize query filter substring into tokens (for parsing of filters and expressions)
vector<string> filter_tokenize(string str_query) {
    vector<string> tokens;
    
    string token = "";

    for (int i=0; i<(int)str_query.size(); ++i) {
        // Look for function start token [
        if (str_query[i] == '[') {
            if (token.size() == 0) {
                cerr << "Incorrect query format - no function specified before [\n";
                return vector<string>();
            }
            else if (find(aggregate_functions.begin(), aggregate_functions.end(), lower(token)) == aggregate_functions.end()) {
                cerr << "Unidentified aggregate function " << token << '\n';
                return vector<string>();
            }
            else {
                token = "[" + token;
                tokens.push_back(token);
                token = "";
            }
        }
        // Look for parentheses symbols - () or operation tokens - +-*/: or function end token ]
        else if (
            find(brkt_tokens.begin(), brkt_tokens.end(), str_query[i]) != brkt_tokens.end()
            ||
            find(op_tokens.begin(), op_tokens.end(), string(1, str_query[i])) != op_tokens.end()
            ||
            str_query[i] == ']'
        ) {
            // Push previous token
            if (token.size() > 0) {
                tokens.push_back(token);
            }
            // Add ()+-*/:]
            token = string(1, str_query[i]);
            tokens.push_back(token);
            // Empty current token
            token = "";
        }
        // Check for comparison tokens
        else if (find(cmp_char.begin(), cmp_char.end(), str_query[i]) != cmp_char.end()) {
            // Push previous token
            if (token.size() > 0) {
                tokens.push_back(token);
                token = "";
            }
            // Iterate over all string-tokens (to account for > 1 character), match should exist
            for (string cmp_token: cmp_tokens) {
                // Too large a cmp_token to come up next
                if ((int)cmp_token.size() - 1 > (int)str_query.size() - i - 1) {
                    continue;
                }
                // Found the token in the next few characters
                if (cmp_token == str_query.substr(i, cmp_token.size())) {
                    tokens.push_back(cmp_token);
                    i += (int)cmp_token.size() - 1; // Jump length-1 places
                    break;
                }
            }
        }
        // Check for ||, &&
        else if ((int)str_query.size() - i - 1 >= 1 && (str_query[i] == '|' || str_query[i] == '&')) {
            // Push previous token
            if (token.size() > 0) {
                tokens.push_back(token);
                token = "";
            }
            string combined = str_query.substr(i, 2);
            if (find(or_and_tokens.begin(), or_and_tokens.end(), combined) != or_and_tokens.end()) {
                tokens.push_back(combined);
                i += 1; // Jump one place
            }
        }
        // End of a token
        else if (str_query[i] == ' ') {
            // Push previous token
            if (token.size() > 0) {
                tokens.push_back(token);
                token = "";
            }
        }
        else {
            token += str_query[i];
        }
    }

    if (token.size() > 0) {
        tokens.push_back(token);
    }
    return tokens;
}

// Initialize Filter Class - built tree for filter (with comparisons and expressions)
Filter::Filter(vector<string> tokens) {
    // Strip brackets from filter
    tokens = strip_brackets_from_tokens(tokens);
    this->tokens = tokens;

    // Get left_end and atomic token at top level
    pair<int, string> left_end_atomic_token = separator_expression(tokens, or_and_tokens, cmp_tokens);
    int left_end = left_end_atomic_token.first;
    string cmp_token = left_end_atomic_token.second;

    // left_end set, so subfilters exist
    if (left_end != -1) {
        // Copy tokens to left and right subfilters
        vector<string> left_tokens(left_end), right_tokens((int)tokens.size() - left_end - 1);
        copy(tokens.begin(), tokens.begin() + left_end, left_tokens.begin());
        copy(tokens.begin() + left_end + 1, tokens.end(), right_tokens.begin());

        // Create the subfilters (recursive call)
        this->left_child = new Filter(left_tokens);
        this->right_child = new Filter(right_tokens);
        
        // Set prod_sum
        if (tokens[left_end] == "&&") {this->prod_sum = 0;}
        else if (tokens[left_end] == "||") {this->prod_sum = 1;}
        this->atomic_filter = NULL;
    }
    // left_end not set, so no subfilters and this is an atomic filter
    else {
        // cmp_token contains the comparison for this filter
        this->atomic_filter = new Comparison(tokens);
        this->prod_sum = -1;
        this->right_child = NULL;
        this->left_child = NULL;
    }
}

// Proxy for filter instantiation
// Tokenize string to tokens and pass to Filter(tokens) function
Filter::Filter(string str_rep) : Filter(filter_tokenize(str_rep)) {}

// Filter check function (comparison & expression manipulations handled within)
bool Filter::check(Record* rec) {
    if (!this->right_child && !this->left_child) {
        return this->atomic_filter->evaluate(rec);
    }
    // Exactly one child null, not possible
    else if (!this->right_child || !this->left_child) {
        cerr << "Ill-formed filter\n";
        return false;
    }
    // Check ||, && for child conditions
    else {
        if (this->prod_sum == 0) {
            return this->right_child->check(rec) && this->left_child->check(rec);
        }
        else {
            return this->right_child->check(rec) || this->left_child->check(rec);
        }
    }
}

// Print filter tree for visualization and verification
void dump(Filter* filter, int depth) {
    // Base case (null-ptr)
    if (filter == NULL) {return;}
    
    // Dump for FILTER
    cout << "Depth - " << depth << '\n';
    cout << "Filter - ";
    for (string token: filter->tokens) {cout << token << " ";}
    cout << '\n';
    // Recursive dump for children
    dump(filter->left_child, depth + 1);
    dump(filter->right_child, depth + 1);
    
    // Dump for atomic filters
    if (filter->atomic_filter != NULL) {
        dump(filter->atomic_filter, depth + 1);
    }
}

// Initialize GroupedFilter Class - built tree for group-filter (with comparisons and expressions)
GroupedFilter::GroupedFilter(vector<string> tokens) {
    // Strip brackets from filter
    tokens = strip_brackets_from_tokens(tokens);
    this->tokens = tokens;

    // Get left_end and atomic token at top level
    pair<int, string> left_end_atomic_token = separator_expression(tokens, or_and_tokens, cmp_tokens);
    int left_end = left_end_atomic_token.first;
    string cmp_token = left_end_atomic_token.second;

    // left_end set, so subfilters exist
    if (left_end != -1) {
        // Copy tokens to left and right subfilters
        vector<string> left_tokens(left_end), right_tokens((int)tokens.size() - left_end - 1);
        copy(tokens.begin(), tokens.begin() + left_end, left_tokens.begin());
        copy(tokens.begin() + left_end + 1, tokens.end(), right_tokens.begin());

        // Create the subfilters (recursive call)
        this->left_child = new GroupedFilter(left_tokens);
        this->right_child = new GroupedFilter(right_tokens);
        
        // Set prod_sum
        if (tokens[left_end] == "&&") {this->prod_sum = 0;}
        else if (tokens[left_end] == "||") {this->prod_sum = 1;}
        this->atomic_filter = NULL;
    }
    // left_end not set, so no subfilters and this is an atomic filter
    else {
        // cmp_token contains the comparison for this filter
        this->atomic_filter = new GroupedComparison(tokens);
        this->prod_sum = -1;
        this->right_child = NULL;
        this->left_child = NULL;
    }
}

// Proxy for grouped-filter instantiation
// Tokenize string to tokens and pass to GroupedFilter(tokens) function
GroupedFilter::GroupedFilter(string str_rep) : GroupedFilter(filter_tokenize(str_rep)) {}

// Filter check function (comparison & expression manipulations handled within)
bool GroupedFilter::check(GroupedRecord* rec) {
    if (!this->right_child && !this->left_child) {
        return this->atomic_filter->evaluate(rec);
    }
    // Exactly one child null, not possible
    else if (!this->right_child || !this->left_child) {
        cerr << "Ill-formed filter\n";
        return false;
    }
    // Check ||, && for child conditions
    else {
        if (this->prod_sum == 0) {
            return this->right_child->check(rec) && this->left_child->check(rec);
        }
        else {
            return this->right_child->check(rec) || this->left_child->check(rec);
        }
    }
}

// Print filter tree for visualization and verification
void dump(GroupedFilter* filter, int depth) {
    // Base case (null-ptr)
    if (filter == NULL) {return;}
    
    // Dump for FILTER
    cout << "Depth - " << depth << '\n';
    cout << "Filter - ";
    for (string token: filter->tokens) {cout << token << " ";}
    cout << '\n';
    // Recursive dump for children
    dump(filter->left_child, depth + 1);
    dump(filter->right_child, depth + 1);
    
    // Dump for atomic filters
    if (filter->atomic_filter != NULL) {
        dump(filter->atomic_filter, depth + 1);
    }
}

// Tokenize select_query standardized representation
vector<string> select_query_tokenize(string select_query) {
    vector<string> tokens;
    string token = "";
    int depth = 0;
    for (int idx=0; idx < (int)select_query.size(); ++idx) {
        if (select_query[idx] == '{') {
            if (depth == 0) {
                if ((int)token.size() > 0) {
                    tokens.push_back(token);
                }
                token = "";
            }
            else {
                token += '{';
            }
            depth++;
        }
        else if (select_query[idx] == '}') {
            depth--;
            if (depth == 0) {
                tokens.push_back(token);
                token = "";
            }
            else {
                token += '}';
            }
        }
        else if (select_query[idx] == ' ' && depth == 0) {
            if ((int)token.size() > 0) {
                tokens.push_back(token);
                token = "";
            }
        }
        else {
            token += select_query[idx];
        }
    }
    if ((int)token.size() > 0) {
        tokens.push_back(token);
    }
    return tokens;
}

// Instantiate Join class using standardized select_query string
Join::Join(string join_query, string db) {
    this->db = db;
    vector<string> select_tokenized = select_query_tokenize(join_query);
    if ((int)select_tokenized.size() != 4) {
        cerr << "Ill-formed join query\n";
        exit(1);
    }

    this->left_query = new SelectQuery(select_tokenized[1], db);
    this->right_query = new SelectQuery(select_tokenized[2], db);
    if ((int)select_tokenized[3].size() > 0) {
        this->filters.push_back(new Filter(select_tokenized[3]));
    }
}

// Instantiate SelectQuery class using standardized select_query string
SelectQuery::SelectQuery(string select_query, string db) {
    this->atomic_query_db = db;
    vector<string> select_tokenized = select_query_tokenize(select_query);

    // Handle atomic SelectQuery
    if ((int)select_tokenized.size() == 1) {
        this->atomic_query_table = select_tokenized[0];
        this->subquery = NULL;
        this->join = NULL;
        this->group_by = false;

        TableMetadata* table_metadata = new TableMetadata("db/" + this->atomic_query_db + "/" + this->atomic_query_table);
        for (string attr: table_metadata->attrList) {
            if (attr == "__id") {continue;}
            this->non_group_attributes[attr] = new Expression(vector<string>({attr}));
        }
        return;
    }

    if ((int)select_tokenized.size() != 12) {
        cerr << "Ill-formed select query\n";
        exit(1);
    }
    
    // Combine column names needed with aliases in AS
    vector<string> selected_tokens_vec = tokenize(select_tokenized[1], ",");
    vector<string> as_vec = tokenize(select_tokenized[3], ",");
    if ((int)select_tokenized[9].size() > 0) {
        // If GROUPBY present, selected attributes are group-expressions
        this->group_by = true;
        int idx = 0;
        for (string selected_tokens: selected_tokens_vec) {
            vector<string> tokens = filter_tokenize(selected_tokens);
            this->group_attributes[as_vec[idx]] = new GroupedExpression(tokens);
            idx += 1;
        }
    }
    else {
        // If GROUPBY absent, selected attributes are non-group expressions
        this->group_by = false;
        int idx = 0;
        for (string selected_tokens: selected_tokens_vec) {
            vector<string> tokens = filter_tokenize(selected_tokens);
            this->non_group_attributes[as_vec[idx]] = new Expression(tokens);
            idx += 1;
        }
    }

    // Handle WHERE filters
    if ((int)select_tokenized[7].size() > 0) {
        this->filters.push_back(new Filter(select_tokenized[7]));
    }

    if (this->group_by) {
        // Grouping attributes
        vector<string> grouped_tokens_vec = tokenize(select_tokenized[9], ",");
        for (string grouped_tokens: grouped_tokens_vec) {
            vector<string> tokens = filter_tokenize(grouped_tokens);
            this->grouping_keys.push_back(new Expression(tokens));
        }

        // Grouping filters
        if ((int)select_tokenized[11].size() > 0) {
            this->grouped_filters.push_back(new GroupedFilter(select_tokenized[11]));
        }
    }

    if (select_tokenized[5].substr(0, 5) == "JOIN ") {
        this->subquery = NULL;
        this->join = new Join(select_tokenized[5], this->atomic_query_db);
    }
    else if (select_tokenized[5].substr(0, 7) == "SELECT ") {
        this->join = NULL;
        this->subquery = new SelectQuery(select_tokenized[5], this->atomic_query_db);
    }
    else {
        this->join = NULL;
        this->subquery = NULL;
        this->atomic_query_table = select_tokenized[5];
    }
}

void Join::dump(int depth) {
    cout << "-----------------" << '\n';
    cout << "JOIN - DEPTH " << depth << '\n';
    this->left_query->dump(depth+1);
    this->right_query->dump(depth+1);
}

void SelectQuery::dump(int depth) {
    cout << "-----------------" << '\n';
    cout << "SELECT - DEPTH " << depth << " ";
    if (this->group_by) {
        cout << "GROUPED ";
        for (pair<string, GroupedExpression*> key_val: group_attributes) {cout << key_val.first << " ";}
        cout << '\n';
    }
    else {
        cout << "NON-GROUPED ";
        for (pair<string, Expression*> key_val: non_group_attributes) {cout << key_val.first << " ";}
        cout << '\n';
    }
    if (this->subquery != NULL) {
        cout << "SOURCE - Subquery\n";
        this->subquery->dump(depth+1);
    }
    else if (this->join != NULL) {
        cout << "SOURCE - Join\n";
        this->join->dump(depth+1);
    }
    else {
        cout << "SOURCE - Table " << this->atomic_query_table << '\n';
    }
}

// TODO - Implement Join fetch
vector<Record*> Join::fetch() {
    vector<Record*> result;
    vector<Record*> left_res = this->left_query->fetch();
    vector<Record*> right_res = this->right_query->fetch();

    for (Record* right_rec: right_res) {
        for (Record* left_rec: left_res) {
            Record* result_rec = new Record();
            for (pair<string, Value*> right_key: right_rec->elements) {
                result_rec->elements[right_key.first] = right_key.second;
            }
            for (pair<string, Value*> left_key: left_rec->elements) {
                if (result_rec->elements.find(left_key.first) != result_rec->elements.end()) {
                    result_rec->elements[left_key.first + ".2"] = result_rec->elements[left_key.first];
                    result_rec->elements[left_key.first + ".1"] = left_key.second;
                    result_rec->elements.erase(result_rec->elements.find(left_key.first));
                }
                else {
                    result_rec->elements[left_key.first] = left_key.second;
                }
            }
            bool valid = true;
            for (Filter* filter: this->filters) {
                if (!filter->check(result_rec)) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                result.push_back(result_rec);
            }
        }
    }
    return result;
}

// TODO - Deal with JOIN
vector<Record*> SelectQuery::fetch() {
    // Get records from subquery
    vector<Record*> recordList;
    if (this->subquery == NULL && this->join == NULL) {
        TableData* td = new TableData("db/" + this->atomic_query_db + "/" + this->atomic_query_table);
        recordList = td->recordList;
    }
    else if (this->join == NULL) {
        recordList = this->subquery->fetch();
    }
    else if (this->subquery == NULL) {
        recordList = this->join->fetch();
    }

    // Iterate over records, filter by where
    vector<Record*> result;
    for (Record* rec: recordList) {
        bool valid = true;
        for (Filter* filter: this->filters) {
            // Found a filter where mismatch occurs
            if (!filter->check(rec)) {
                valid = false;
                break;
            }
        }
        
        // Skip if filter mismatch
        if (!valid) {continue;}
        
        // If not group by, prune columns ELSE retain all columns
        Record* result_rec = new Record();
        if (this->group_by) {
            for (pair<string, Value*> record_elements: rec->elements) {
                result_rec->elements[record_elements.first] = record_elements.second;
            }
        }
        else {
            for (pair<string, Expression*> attr_exp: this->non_group_attributes) {
                result_rec->elements[attr_exp.first] = (attr_exp.second)->evaluate(rec);
            }
        }
        result.push_back(result_rec);
    }

    // Not group by - return Results
    if (!this->group_by) {return result;}

    // Start group by - maintain unique keys and then pass to grouped comparisons and filters
    vector<GroupedRecord*> groups;
    for (Record* rec: result) {
        // Evaluate grouping keys
        Record* grouping_key_rec = new Record();
        for (int i=0; i<(int)grouping_keys.size(); ++i) {
            grouping_key_rec->elements[grouping_keys[i]->get_name()] = grouping_keys[i]->evaluate(rec);
        }

        // Iterate over already saved groups
        bool found_rec = false;
        for (GroupedRecord* already_saved: groups) {
            bool found_equal = true;
            // Iterate over keys and check for equality
            for (int i=0; i<(int)grouping_keys.size(); ++i) {
                string expr_name = grouping_keys[i]->get_name();
                if (!is_equal(already_saved->group_keys[grouping_keys[i]->get_name()], grouping_key_rec->elements[grouping_keys[i]->get_name()])) {
                    found_equal = false;
                    break;
                }
            }
            // Found keys for grouping this record
            if (found_equal) {
                already_saved->group_value_records.push_back(rec);
                found_rec = true;
                break;
            }
        }

        // Not found any groups with this, add new
        if (!found_rec) {
            GroupedRecord* new_group = new GroupedRecord();
            // Set keys
            new_group->group_keys = grouping_key_rec->elements;
            // Set attributes
            new_group->group_value_attributes = vector<string>();
            for (pair<string, Value*> column_val: rec->elements) {
                string column = column_val.first;
                new_group->group_value_attributes.push_back(column);
            }
            // Add first record
            new_group->group_value_records.push_back(rec);
            
            groups.push_back(new_group);
        }
    }

    result.clear();

    // Start HAVING and computation - filter using group comparisons, evaluate group expressions
    for (GroupedRecord* group_rec: groups) {
        bool valid = true;
        for (GroupedFilter* grouped_filter: this->grouped_filters) {
            if (!grouped_filter->check(group_rec)) {valid = false;}
        }
        if (valid) {
            Record* result_rec = new Record();
            for (pair<string, GroupedExpression*> attr_exp: this->group_attributes) {
                result_rec->elements[attr_exp.first] = (attr_exp.second)->evaluate(group_rec);
            }
            result.push_back(result_rec);
        }
    }

    return result;
}

void test_grouped_filter() {
    GroupedFilter* filter = new GroupedFilter("(a.b=3 && sum[c.d+e.f-k]><4) || (l.m*k >= 2 || (n != 4)) && x.y <= 3");
    dump(filter, 0);
}

void test_select_query() {
    string query; SelectQuery* sel_query;

    query = "SELECT {name,age,weight,new_name} AS {name,age,weight,new_name} FROM {JOIN {people} {SELECT {name} AS {new_name} FROM {people} WHERE {weight>75} GROUPBY {} HAVING {}} {name>new_name}} WHERE {} GROUPBY {} HAVING {}";
    sel_query = new SelectQuery(query, "b");
    cout << stringify_records(sel_query->fetch()); cout << '\n';

    query = "SELECT {name,age+weight} AS {name,sum} FROM {JOIN {people} {SELECT {name} AS {new_name} FROM {people} WHERE {weight>75} GROUPBY {} HAVING {}} {name>new_name}} WHERE {weight>0} GROUPBY {} HAVING {}";
    sel_query = new SelectQuery(query, "b");
    cout << stringify_records(sel_query->fetch()); cout << '\n';

    query = "SELECT {max[name],weight} AS {max_name,common_weight} FROM {people} WHERE {weight>0} GROUPBY {weight} HAVING {}";
    sel_query = new SelectQuery(query, "b");
    cout << stringify_records(sel_query->fetch()); cout << '\n';
}

// int main() {
//     test_select_query();
// }