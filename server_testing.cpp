#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <poll.h>

int main(){

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1){
        std::cout << "Error creating socket\n";
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8081);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error binding socket\n";
        close(server_socket);
        return 1;
    }

    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1){
        std::cerr << "Error setting socket to non-blocking mode\n";
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 5) == -1){
        std::cerr << "Error listening socket\n";
        close(server_socket);
        return 1;
    }

    const int MAX_EVENTS = 10;
    struct pollfd fds[MAX_EVENTS];
    memset(fds, 0, sizeof(fds));
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;

    while (true){
        int num_events = poll(fds, MAX_EVENTS, -1);
        if (num_events == -1){
            std::cerr << "Error in poll\n";
            close(server_socket);
            return 1;
        }

        //обработка событий
        for (int i = 0; i < MAX_EVENTS; ++i){
            if (fds[i].fd == server_socket){
                //принимаем входящего клиента
                int client_socket = accept(server_socket, NULL, NULL);
            }
        }
    }

    return 0;

}