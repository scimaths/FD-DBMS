#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <vector>
#include <thread>
#include "query_select.h"

#define PORT 12345
#define BACKLOG 50

std::vector<int> client_sockets;

void handle_client(int client_socket) {
    char buffer[1024] = {0};
    int valread;
    while ((valread = read(client_socket, buffer, sizeof(buffer))) > 0) {
        std::cout << "Received: " << buffer << std::endl;
        SelectQuery* sel_query;
        sel_query = new SelectQuery(string(buffer), "univ_db");
        cout<<"Object made"<<endl; 
        string output = stringify_records(sel_query->fetch());
        cout << output << endl<<endl ;
        // send(client_socket, output.c_str(), strlen(output.c_str()), 0);
        send(client_socket, "Server received", strlen("Server received"), 0);
        memset(buffer, 0, sizeof(buffer));
    }
    std::cout << "Client disconnected" << std::endl;
    client_sockets.erase(std::find(client_sockets.begin(), client_sockets.end(), client_socket));
    close(client_socket);
}

int main() {
    int server_socket, client_socket, max_socket, activity, i, valread;
    struct sockaddr_in server_address, client_address;
    fd_set readfds;
    

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Failed to bind socket" << std::endl;
        return 1;
    }

    if (listen(server_socket, BACKLOG) == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return 1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        max_socket = server_socket;

        for (i = 0; i < client_sockets.size(); i++) {
            client_socket = client_sockets[i];
            FD_SET(client_socket, &readfds);
            if (client_socket > max_socket) {
                max_socket = client_socket;
            }
        }

        activity = select(max_socket + 1, &readfds, NULL, NULL, NULL);
        if (activity == -1) {
            std::cerr << "Failed to select socket" << std::endl;
            return 1;
        }

        if (FD_ISSET(server_socket, &readfds)) {
            client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t*)&client_address);
            if (client_socket == -1) {
                std::cerr << "Failed to accept client connection" << std::endl;
                continue;
            }
            std::cout << "Client connected" << std::endl;
            std::thread client_thread(handle_client, client_socket);
            client_thread.detach();
            client_sockets.push_back(client_socket);
        }

        for (i = 0; i < client_sockets.size(); i++) {
            client_socket = client_sockets[i];
            if (FD_ISSET(client_socket, &readfds)) {
                // do nothing - this is handled by the thread
            }
        }
    }

    close(server_socket);
    return 0;
}