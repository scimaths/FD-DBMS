/*
server.cpp
Inspired from Beej's guide - selectserver.c
*/

// C Imports
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>

// C++ Imports
#include <string>
#include <iostream>
#include <algorithm>

#define PORT "8080"

// get sockaddr, IPv4 or IPv6
void* get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc, char const* argv[]) {
    // Set of file descriptors
    fd_set master;
    fd_set read_fds;
    int fdmax = 32;
    // Initialize fd-sets to contain no file descriptors
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    // Listening socket FD
    int listener;
    // FD for newly accepted connection
    int newfd;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;

    char buf[4096];
    int nbytes;
    int result_var;
    int setsockopt_yes = 1;

    // String version of IP6-address
    char remoteIP[INET6_ADDRSTRLEN];

    struct addrinfo hints, *ai, *p;

    // Prepare structs for binding
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // v4 or v6, either
    hints.ai_socktype = SOCK_STREAM; // Stream Socket
    hints.ai_flags = AI_PASSIVE; // Filling our own IP
    if ((result_var = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "server.cpp - %s\n", gai_strerror(result_var));
        exit(1);
    }

    // Iterate over the linked list set by getaddrinfo
    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // Secure port for usage
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &setsockopt_yes, sizeof(int));
        
        // Bind to port
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        // If bound to any port 
        break;
    }

    if (p == NULL) {
        perror("server.cpp - bind");
        exit(2);
    }

    // Cleanup of address infos
    freeaddrinfo(ai);

    // Listen to incoming connections
    int listen_backlog = 16;
    if (listen(listener, listen_backlog) < 0) {
        perror("server.cpp - listen");
        exit(3);
    }

    // Listener is added to master set
    FD_SET(listener, &master);
    fdmax = listener;

    // Main loop - dealing with individual connections
    while (true) {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("server.cpp - select");
        }
    }
    std::cout << "1" << '\n';
}