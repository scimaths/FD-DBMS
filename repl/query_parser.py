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
        stmt = stmt.replace("[", " [ ")
        stmt = stmt.replace("]", " ] ")
        stmt = stmt.replace("(", " ( ")
        stmt = stmt.replace(")", " ) ")
        stmt = stmt.replace(",", " , ")
        words = stmt.split()
        words = [self.upper_case(word)  for word in words]
        depth = 0
        if(stmt.count("(")!=stmt.count(")")):
            print("Invalid Query")
            return
        
        _, parsed_query = self.query_handle(words, depth)
        return parsed_query
    
    
    
    def query_handle(self, words, depth, label="select"):
        

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

            if word=="(":
                current_depth = depth+1
                k = i+1
                while k<len(words):
                    if words[k]=="(":
                        current_depth+=1
                    if words[k]==")":
                        current_depth-=1
                    k+=1
                    if current_depth==depth:
                        break
                k, parsed_subquery = self.query_handle(words[i+1:k-1], depth+1,"select")
                i +=k+2
                if i>=len(words) or words[i] == "JOIN":
                    from_db.append(parsed_subquery)
                word = words[i]
            if word in self.keywords:
                if word == "SELECT":
                    j = i + 1
                    while j < len(words):
                        if words[j] in self.aggregates:
                            if words[j+1]=="[":
                                current_depth = depth+1
                                k = j+2
                                while k<len(words):
                                    if words[k]=="[":
                                        current_depth+=1
                                    if words[k]=="]":
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
                        if words[j]==",":
                            j+=1
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
                            # print("DEPTH increasing to", depth+1)
                            print(j+1, k)
                            i, parsed_subquery = self.query_handle(words[j+1:k], depth+1, "from" )
                            print(parsed_subquery)
                            j = i+j+2
                            from_db.append(parsed_subquery)
                            # print("DEPTH restored to", depth,"\n")
                        if j>=len(words) or words[j] in self.keywords:
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
                        join_type = ""
                        i+=1
                    else:
                        join_type = word+" "
                        i += 2
                    if words[i]=="(":
                        current_depth = depth+1
                        k = i+1
                        while k<len(words):
                            if words[k]=="(":
                                current_depth+=1
                            if words[k]==")":
                                current_depth-=1
                            k+=1
                            if current_depth==depth:
                                break
                        k, parsed_subquery = self.query_handle(words[i+1:k-1], depth+1, "select" )
                        i +=k+1
                        table_name = (parsed_subquery)
                        word = words[i]
                    else:
                        table_name = words[i]
                    conditions = ""
                    if words[i+1] == "ON" or words[i+1]=="USING":
                        conditions += words[i+2]
                        i += 2
                    if len(join_conditions):
                        join_conditions[0] = ("JOIN "+ join_type + "{"+ table_name +"} {"+join_conditions[-1] +"} {" +conditions +"}")
                    else: 
                        join_conditions.append("JOIN "+ join_type + "{"+ table_name +"} {"+from_db[-1] +"} {"+conditions+"}" )


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
        if label=="select":
            parsed_query+="SELECT {"+",".join(select_attr)+"} AS {"+",".join(select_as_attr)+"} "
            if len(join_conditions):
                parsed_query+="FROM {"+",".join(join_conditions)+"} "
            else:
                parsed_query+="FROM {"+",".join(from_db)+"} "
            parsed_query+="WHERE {"+" ".join(filter_conditions)+"} "
            parsed_query+="GROUPBY {"+",".join(group_by_items)+"} "
            parsed_query+="HAVING {"+" ".join(having_conditions)+"}"
        elif label=="from":
            if len(join_conditions):
                parsed_query+=",".join(join_conditions)
            else:
                parsed_query+="SELECT {"+",".join(select_attr)+"} AS {"+",".join(select_as_attr)+"} "
                parsed_query+="FROM {"+",".join(from_db)+"} "
                parsed_query+="WHERE {"+" ".join(filter_conditions)+"} "
                parsed_query+="GROUPBY {"+",".join(group_by_items)+"} "
                parsed_query+="HAVING {"+" ".join(having_conditions)+"}"
            
        return i, parsed_query
        
    



# parser = QueryParser()
# query = input()
# # query = 'select frif as f from (select abcd from hshsh  ) where jfiejf>2'
# query = 'select id.1, id.2, salary.1 - salary.2 from((SELECT id, salary FROM instructor) JOIN (SELECT id, salary FROM instructor) ON salary.1 > 0.5*salary.2 && id.1!=id.2) where id.1="knuth'
# print(parser.parse_stmt(query))



'''
SELECT min[name], department from instructor group by department

select id.1, id.2, salary.1 - salary.2 from((SELECT id, salary FROM instructor) JOIN (SELECT id, salary FROM instructor) ON salary.1 > 0.5*salary.2 && id.1!=id.2) where id.1="knuth"

'''