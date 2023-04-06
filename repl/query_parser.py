class QueryParser:
    def __init__(self):
        self.stmt_type_list = [
            'INSERT',
            'SELECT',
            'DELETE',
            'UPDATE',
            'CREATE DATABASE',
            'CREATE TABLE',
            'FUNCDEP'
        ]

    def parse_stmt(self, stmt):
        pass