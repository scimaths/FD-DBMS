class QueryParser:
    temp_count = 0
    def __init__(self):
        self.stmt_type_list = [
            'INSERT',
            'SELECT',
            'DELETE',
            'UPDATE',
            'CREATE DATABASE',
            'CONNECT DATABASE',
            'DROP DATABASE',
            'CREATE TABLE',
            'FUNCDEP'
        ]
        self.keywords = ["SELECT", "FROM", "NATURAL", "INNER", "LEFT", "RIGHT", "FULL", "JOIN", "ON", "WHERE", "GROUP", "HAVING"]
        self.clauses = ["SELECT", "FROM", "JOIN", "WHERE", "GROUP", "HAVING"]

    def parse_stmt(self, stmt):
        
        stmt = stmt.replace("(", " ( ")
        stmt = stmt.replace(")", " ) ")
        words = stmt.split()
        words = [word.upper() for word in words]
        print(words,"\n")
        depth = 0
        if(stmt.count("(")!=stmt.count(")")):
            print("Invalid Query")
            return
        
        self.query_handle(words, depth, self.new_temp())
        pass
    
    def new_temp(self):
        QueryParser.temp_count+=1
        return "temp"+str(QueryParser.temp_count)
    
    
    def query_handle(self, words, depth, name):
        

        # Define the list of keywords and clauses

        # Split the query into words

        # Initialize the lists of SELECT items, FROM tables, JOIN conditions, GROUP BY items, ORDER BY items, and the LIMIT count
        select_attr = []
        from_db = []
        filter_conditions = []
        join_conditions = []
        group_by_items = []
        having_conditions = []

        i = 0

        
        
        while i < len(words):
            word = words[i]
            next_word = words[i+1] if i+1 < len(words) else ""
            print(word)

            
            if word in self.keywords:
                if word == "SELECT":
                    j = i + 1
                    while j < len(words):
                        if words[j]=="(":
                            current_depth = depth+1
                            k = j+1
                            while k<len(words):
                                if words[k]=="(":
                                    current_depth+=1
                                if words[k]==")":
                                    current_depth-=1
                                if current_depth==depth:
                                    break
                                k+=1
                            print("DEPTH increasing to", depth+1)
                            subquery_temp = self.new_temp()
                            j = self.query_handle(words[j+1:k], depth+1,subquery_temp )+j+2
                            from_db.append(subquery_temp)
                            print("DEPTH restored to", depth)
                        if words[j] == "FROM":
                            break
                        select_attr.append(words[j])
                        j += 1
                    i = j - 1

                elif word == "FROM":
                    j = i + 1
                    while j < len(words):
                        if words[j]=="(":
                            current_depth = depth+1
                            k = j+1
                            while k<len(words):
                                if words[k]=="(":
                                    current_depth+=1
                                if words[k]==")":
                                    current_depth-=1
                                if current_depth==depth:
                                    break
                                k+=1
                            print("DEPTH increasing to", depth+1)
                            subquery_temp = self.new_temp()
                            j = self.query_handle(words[j+1:k], depth+1,subquery_temp )+j+2
                            from_db.append(subquery_temp)
                            print("DEPTH restored to", depth)
                        if words[j] in self.clauses:
                            break
                        from_db.append(words[j])
                        j += 1
                    i = j - 1
                    
                    
                elif word == "WHERE":
                    j = i + 1
                    while j < len(words):
                        if words[j] in ["GROUP", "HAVING"]:
                            break
                        filter_conditions.append(words[j])
                        j += 1
                    i = j - 1

                elif word in ["INNER", "LEFT", "RIGHT", "FULL", "NATURAL"] or word=="JOIN":
                    if(word=="JOIN"):
                        join_type = ""
                        join_word = word
                        i+=1
                    else:
                        join_type = word
                        join_word = next_word
                        i += 2
                        
                    table_name = words[i]
                    if words[i+1] == "AS":
                        table_alias = words[i+2]
                        i += 2
                    else:
                        table_alias = ""
                    from_db.append(table_name)
                    join_conditions.append((join_type, join_word, table_name, table_alias))


                elif word == "GROUP" and next_word=="BY":
                    j = i + 2
                    while j < len(words):
                        if words[j] in ["HAVING"]:
                            break
                        group_by_items.append(words[j])
                        j += 1
                    i = j - 1

                elif word == "HAVING":
                    j = i + 1
                    while j < len(words):
                        if words[j] in self.keywords:
                            break
                        having_conditions.append(words[j])
                        j += 1
                    i = j - 1


            i += 1
        print(name, select_attr,"\n", from_db,"\n",  join_conditions,"\n",  filter_conditions,"\n",   group_by_items,"\n", having_conditions, "\n" )
        return i
        
    
parser = QueryParser()
# query = input()
# query = 'select frif as f from (select abcd from hshsh  ) where jfiejf>2'
query = 'select sum(col1) as col1_name from (select col2,col3 from db1  ) where qwerty>2 group by col1 having col3<3'
parser.parse_stmt(query)

