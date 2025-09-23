#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <poll.h>
#include <map>

#include <filesystem>
#include <cstdlib>

#include "macro.h"
#include "request_http_v2.h"
#include "client_class.h"
#include "http.h"

namespace fs = std::filesystem;

class Client;
std::map<int, Client> list_connected_clients; // ключ = fd


//return state error
//request_http_v1 difference from the equest_http_v2 in chmod
        //chmod v1 - all file - allow  or forbidden
        //chmod v2 - variant access issuance + by default all public files are allowed
// int method_current(const std::string &method){
//     if (method == "GET"){
//         return GET;
//     }
//     else if (method == "HEAD"){
//         return HEAD;
//     }
//     else{
//         return -1;
//     }
// }

// RequestHttp * fill_request(int client_socket){
//     std::string request= "HEAD direct/HTTP/1.1";
//     // std::getline(client_socket, request);
//     // GET URL/HTTP/1.1
//     std::string build_request[3];
//     int stage = 0; // 0 - method 1 - url 2 - 
//     int max_index = request.size() - 1;
//     int index_version = max_index;
//     int index = 0;
//     while ((index <  max_index) && (request[index] != ' ')){
//         ++index;
//     } 
//     std::string str_method = request.substr(0, index);
//     //пропускаем все пробелы
//     while ((index <  max_index) && (request[index] == ' ')){
//         ++index;
//     }
//     int index_start_url = index;
//     //обрабатываем с конца 
//     while (request[index_version] != '/'){
//         --index_version;
//     } 
//     std::string str_version = request.substr(index_version + 1);    
//     int index_end_url =index_version - sizeof("HTTP");
//     std::string str_url = request.substr(index_start_url, index_end_url - index_start_url);

//     // В случае некорректного ввода заполняем поле -1
//     // Заполнение Request_class по типу версии
//     RequestHttp *Request_pointer;
//     if (str_version == "1.1"){
//         Request_pointer= new RequestHttp_v1();
//         Request_pointer->version = 1;
        
//     }
//     else if (str_version == "1.2"){
//         Request_pointer= new RequestHttp_v1();
//         Request_pointer->version = 2;
//     }
//     else{
//         Request_pointer= new RequestHttp_v1(); //ьез разницы какая версия
//         Request_pointer->status_error = NOT_IMPLEMENTED;
//         Request_pointer->explanation_error = "VERSION NOT SUPPORTED";
//         // std::cout << "Ошибка в запросе\n";
//         return nullptr;
//     }

//     if (str_url == "")
//         str_url = "index.html";
//     Request_pointer->url = str_url;

//     if (Request_pointer->fill_command(str_method) == false){
//         Request_pointer->status_error = NOT_IMPLEMENTED;
//         Request_pointer->explanation_error = "METHOD NOT SUPPORTED";
//     }
    
//     Request_pointer->client.fd = client_socket;
//     Request_pointer->client.mode = ALL_FILE_ALLOW;
    
//     Request_pointer->status_error = OK;

//     return Request_pointer;
// }


// int main(){
//     // setlocale(LC_ALL, "rus");
//     //Переменные окружения для cgi-прогnрамм
//     setenv("USER_AGENT", "clientserver", 1);
//     std::string request= "GET cgi-bin/test_cgi?VAR_NAME=MATVEY&VAR_PORT=8080 HTTP/1.2";
//     http::start_proc_request(STDOUT_FILENO, request);

//     // RequestHttp *Request_pointer= fill_request(STDOUT_FILENO);
//     // Request_pointer->execute_request_http();
//     // if (Request_pointer!= nullptr)
//         // delete Request_pointer;
//     return 0;
// }

int main() {
    setlocale(LC_ALL, "rus");
    // Создание сокета
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Установка параметров сервера
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);  // Порт 8080
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Привязка сокета к адресу и порту
    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error binding socket\n";
        close(server_socket);
        return 1;
    }

    // Перевод сокета в неблокирующий режим
    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Error setting socket to non-blocking mode\n";
        close(server_socket);
        return 1;
    }

    // Прослушивание входящих соединений
    if (listen(server_socket, 5) == -1) {
        std::cerr << "Error listening\n";
        close(server_socket);
        return 1;
    }

    // Создание массива структур для событий poll
    const int MAX_EVENTS = 10;
    struct pollfd fds[MAX_EVENTS];
    memset(fds, 0, sizeof(fds));
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;

    // Основной цикл сервера
    while (true) {
        // Ожидание событий ввода-вывода
        int num_events = poll(fds, MAX_EVENTS, -1);
        if (num_events == -1) {
            std::cerr << "Error in poll\n";
            close(server_socket);
            return 1;
        }

        // Обработка событий
        for (int i = 0; i < MAX_EVENTS; ++i) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == server_socket) {
                    // Принятие входящего соединения
                    
                    int client_socket = accept(server_socket, NULL, NULL);
                    Client client(client_socket);
                    list_connected_clients.insert(fd, client);
                    if (client_socket == -1) {
                        std::cerr << "Error accepting connection\n";
                        close(server_socket);
                        return 1;
                    }

                    // Добавление нового клиентского сокета в массив структур для poll
                    for (int j = 1; j < MAX_EVENTS; ++j) {
                        if (fds[j].fd == 0) {
                            fds[j].fd = client_socket;
                            fds[j].events = POLLIN;
                            break;
                        }
                    }
                } else {
                    // Обработка запроса от клиента
                    // int client_socket = fds[i].fd;
                    // fill_request(client_socket);

                    // Создание нового процесса для обработки запроса

                    //запрос http формата
                    
                    if (fork() == 0) {
                        // Дублирование сокета для стандартного вывода
                        dup2(client_socket, STDIN_FILENO);
                        std::string request;
                        std::cin >> request;
                        http::start_proc_request(STDOUT_FILENO, request);
                        close(client_socket);
                    }
                    // close(client_socket);

                    // Удаление клиентского сокета из массива структур для poll
                    fds[i].fd = 0;
                    fds[i].events = 0;
                }
            }
        }
    }-

    // Закрытие серверного сокета
    close(server_socket);
    return 0;
}
