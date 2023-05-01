import re

class QueryParser:
    # temp_count = 0
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
        self.keywords = ["SELECT", "AS", "FROM", "NATURAL", "OUTER", "INNER", "LEFT", "RIGHT", "FULL", "JOIN", "ON", "USING", "WHERE", "GROUP", "BY", "HAVING", "SUM", "AVG", "MEAN", "COUNT", "MIN", "MAX"]
        self.clauses = ["SELECT", "FROM", "JOIN", "WHERE", "GROUP", "HAVING"]
        self.aggregates = ["SUM", "AVG", "MEAN", "COUNT", "MIN", "MAX"]

    def upper_case(self, word):
        return word.upper() if word.upper() in self.keywords else word
    
    def parse_stmt(self, stmt):
        
        stmt = re.sub(r"\s*\+\s*","+", stmt)
        stmt = re.sub(r"\s*\-\s*","-", stmt)
        stmt = re.sub(r"\s*\*\s*","*", stmt)
        stmt = re.sub(r"\s*\/\s*","/", stmt)
        stmt = re.sub(r"\s*\=\s*","=", stmt)
        stmt = re.sub(r"\s*\>\s*",">", stmt)
        stmt = re.sub(r"\s*\<\s*","<", stmt)
        stmt = re.sub(r"\s*\!\s*","!", stmt)
        stmt = re.sub(r"\s*\%\s*","%", stmt)
        stmt = re.sub(r"\s*\&\s*","&", stmt)
        stmt = re.sub(r"\s*\|\s*","|", stmt)
        stmt = stmt.replace("(", " ( ")
        stmt = stmt.replace(")", " ) ")
        stmt = stmt.replace(",", " , ")
        words = stmt.split()
        words = [self.upper_case(word)  for word in words]
        print(words,"\n")
        depth = 0
        if(stmt.count("(")!=stmt.count(")")):
            print("Invalid Query")
            return
        
        self.query_handle(words, depth)
        pass
    
    # def new_temp(self):
    #     QueryParser.temp_count+=1
    #     return "temp"+str(QueryParser.temp_count)
    
    
    def query_handle(self, words, depth):
        

        # Define the list of keywords and clauses

        # Split the query into words

        # Initialize the lists of SELECT items, FROM tables, JOIN conditions, GROUP BY items, ORDER BY items, and the LIMIT count
        select_attr = []
        select_as_attr = []
        from_db = []
        filter_conditions = []
        join_conditions = []
        group_by_items = []
        having_conditions = []

        i = 0

        
        
        while i < len(words):
            word = words[i]
            next_word = words[i+1] if i+1 < len(words) else ""

            
            if word in self.keywords:
                if word == "SELECT":
                    j = i + 1
                    while j < len(words):
                        if words[j]==",":
                            j+=1
                        if words[j] in self.aggregates:
                            if words[j+1]=="(":
                                current_depth = depth+1
                                k = j+2
                                while k<len(words):
                                    if words[k]=="(":
                                        current_depth+=1
                                    if words[k]==")":
                                        current_depth-=1
                                    k+=1
                                    if current_depth==depth:
                                        break
                                if k<len(words) and words[k] == "AS":
                                    select_attr.append("".join(words[j:k]))
                                    select_as_attr.append(words[k+1])
                                    j=k+2
                                else:
                                    select_attr.append("".join(words[j:k]))
                                    select_as_attr.append("".join(words[j:k]))
                                    j=k
                        if words[j] == "FROM":
                            break
                        if words[j+1] == "AS":
                            select_attr.append(words[j])
                            select_as_attr.append(words[j+2])
                            j+=2
                        else:
                            select_attr.append(words[j])
                            select_as_attr.append(words[j])
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
                            i, parsed_subquery = self.query_handle(words[j+1:k], depth+1 )
                            j = i+j+2
                            from_db.append(parsed_subquery)
                            print("DEPTH restored to", depth,"\n")
                        if words[j] in self.keywords:
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

                elif word in ["OUTER", "INNER","FULL", "NATURAL"] or word=="JOIN":
                    if(word=="JOIN"):
                        join_type = "NORMAL"
                        i+=1
                    else:
                        join_type = word
                        i += 2
                    table_name = words[i]
                    conditions = ""
                    if words[i+1] == "ON" or words[i+1]=="USING":
                        conditions += words[i+2]
                        i += 2
                    if len(join_conditions):
                        join_conditions[0] = ("JOIN "+ join_type + " {"+ table_name +"} {"+join_conditions[-1] +"} {" +conditions +"}")
                    else: 
                        join_conditions.append("JOIN "+ join_type + " {"+ table_name +"} {"+from_db[-1] +"} {"+conditions+"}" )


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
        parsed_query = ""
        parsed_query+="SELECT {"+",".join(select_attr)+"} AS {"+",".join(select_as_attr)+"} "
        if len(join_conditions):
            parsed_query+="FROM {"+",".join(join_conditions)+"} "
        else:
            parsed_query+="FROM {"+",".join(from_db)+"} "
        parsed_query+="WHERE {"+" ".join(filter_conditions)+"} "
        parsed_query+="GROUP BY {"+",".join(group_by_items)+"} "
        parsed_query+="HAVING {"+" ".join(having_conditions)+"}"
            
        print(parsed_query)
        # print(select_attr,"\n", from_db,"\n",  join_conditions,"\n",  filter_conditions,"\n",   group_by_items,"\n", having_conditions, "\n" )
        return i, parsed_query
        
    
parser = QueryParser()
# query = input()
# query = 'select frif as f from (select abcd from hshsh  ) where jfiejf>2'
query = 'select col1   +   cols, col2 as f from (select col2,col3 from db1  ) inner join b on this_col join c using good_col>0 && bad_col<0 where qwerty>2 && dief<9 group by col1 having col3<3'
parser.parse_stmt(query)

