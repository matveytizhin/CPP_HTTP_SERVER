#ifndef HTTP_H
#define HTTP_H

#include "request_http.h"
#include "request_http_v1.h"
// #include "request_http_v2.h"

namespace http{
    bool prov_correct_request(const std::string & str_method, const std::string & str_url, const std::string & str_http, const std::string & str_version){
        //не самая лучшая передача параметров в функцию))))
        //Только проверяет на корректность данных
        //так как некоторые проверки для оптимизации были вынесены из этой функции то здесь идет проверка не всех элементов запроса
        if (str_http != "HTTP"){
            std::cout << "HTTP/" << str_version << " " << BAD_REQUEST << " INVALID_FORMAT_REQUEST" << "" << "\n";
            return false;
        }
        if ((str_version == "1.1") && (str_url.find('?') != std::string::npos)){
            std::cout << "HTTP/" << str_version << " " << NOT_IMPLEMENTED << "CGI NOT SUPPORTED IN VERSION 1.1" << "" << "\n";
            return false;
        }
        bool flag_up = true;
        for (char ch : str_method) {
            if (!std::isupper(ch)) {
                flag_up = false;
            }
        }
        if (flag_up == false){
            std::cout << "HTTP/" << str_version << " " << BAD_REQUEST << " BAD REQUEST" << "" << "\n";
            return false;
        }
        
        return true;
    }

    void start_proc_request(int client_socket,const std::string &request){
        //Принимаем с сокета запрос
        // std::getline(client_socket, request);
        // GET URL/HTTP/1.1
        //перенаправим стандартый вывод
        RequestHttp *Request_pointer = nullptr;
        int error;
        int fd_save = STDOUT_FILENO;
        if (dup2(client_socket, STDOUT_FILENO) == -1){
            error = SERVICE_UNAVAILABLE;
            //Отправляем клиенту ошибку другим способом
            const char* message = "HTTTP/1.1 503 SERVICE_UNAVAILABLE\n";
            int bytesent = send(client_socket, message, strlen(message), 0);
            if (bytesent == -1){
                std::cerr << "Error: Failed to send data to client\n";
                close(client_socket);
            }

        }
        //Перенаправленный вывод
        int stage = 0; // 0 - method 1 - url 2 - 
        int max_index = request.size() - 1;
        int index_version = max_index;
        int index = 0;
        while ((index <  max_index) && (request[index] != ' ')){
            ++index;
        } 
        std::string str_method = request.substr(0, index);
        //пропускаем все пробелы
        while ((index <  max_index) && (request[index] == ' ')){
            ++index;
        }
        int index_start_url = index;
        //обрабатываем с конца 
        while (request[index_version] != '/'){
            --index_version;
        }
        std::string str_version = request.substr(index_version + 1);   
        std::string str_http = request.substr(index_version - sizeof("HTTP") + 1, sizeof("HTTP") - 1); 
        int index_end_url =index_version - sizeof("HTTP");
        std::string str_url = request.substr(index_start_url, index_end_url - index_start_url);

        //Проверяем на корректность
        if (!prov_correct_request(str_method, str_url, str_http, str_version)) 
            goto clean_request;

        // Заполнение Request_class по типу версии
        if (str_version == "1.1"){

            Request_pointer= new RequestHttp_v1();
            Request_pointer->version = 1;
            if (Request_pointer->fill_request(str_method, str_url) != -1){
                Request_pointer->execute_request_http();
            }
            else{
                //Выводим ошибку
                Request_pointer->response_banner_request();
            }
        }
        else if (str_version == "1.2"){
            Request_pointer= new RequestHttp_v2();
            Request_pointer->version = 2;
            
            if (Request_pointer->fill_request(str_method, str_url) != -1){
                Request_pointer->execute_request_http();
            }
            else{
                //Выводим ошибку
                Request_pointer->response_banner_request();
            }
        }
        else{
            std::cout << "HTTP/" << str_version << " " << NOT_IMPLEMENTED << " " << "VERSION NOT SUPPORTED" << "\n";
        }
        //восстанавливаем STDOUT_FILENO
    clean_request:
        dup2(STDOUT_FILENO, fd_save);
        //Удаляем из динамической памяти  Request_pointe+r
        if (Request_pointer != nullptr){
            delete Request_pointer;
        }
    }

}

#endif //HTTP_H