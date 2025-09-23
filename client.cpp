#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

const int PORT = 8082;
const std::string SERVER_IP = "127.0.0.1";

void send_request(int client_socket, const std::string& request) {
    send(client_socket, request.c_str(), request.length(), 0);
    char buffer[1024] = {0};
    recv(client_socket, buffer, 1024, 0);
    std::cout << "Response: " << buffer << std::endl;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }
 
    // Send GET request
    std::string get_request = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
    std::cout << "Sending GET request..." << std::endl;
    send_request(client_socket, get_request);

    // Send HEAD request
    std::string head_request = "HEAD /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
    std::cout << "Sending HEAD request..." << std::endl;
    send_request(client_socket, head_request);

    close(client_socket);
    return 0;
}
   