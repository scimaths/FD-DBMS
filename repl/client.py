import re
import os
import sys
import socket
import subprocess

HOST = "127.0.0.1"
PORT = 8080

if __name__ == "__main__":
    print("Welcome to FD-DBMS (project under CS387)\nCreated by HV, Vaibhav, Shashwat, Pulkit")
    # infinite REPL loop

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(b"Hello, world")
        data = s.recv(1024)
        print(data.decode())

    while True:
        statement = input("> ")
        if statement == "exit":
            print("Terminated")
            exit()
        