import re
import os
import sys
import socket
import subprocess
from query_parser import QueryParser

HOST = "127.0.0.1"
PORT = 12345


if __name__ == "__main__":
    print("Welcome to FD-DBMS (project under CS387)\nCreated by Harshvardhan, Vaibhav, Shashwat, Pulkit")
    # infinite REPL loop

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))

        while True:
            statement = input("> ")
            parser = QueryParser()
            # parsed_query = parser.parse_stmt(statement.encode)
            query = 'SELECT id, ref_id FROM instructor'
            parsed_query = parser.parse_stmt(query)
            print(parsed_query)
            s.sendall(parsed_query.encode())
            data = s.recv(1024)
            print(data.decode())

            if statement == "exit":
                print("Terminated")
                exit()
        