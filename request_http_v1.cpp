#include "request_http_v1.h"

#define PATH_FILESYSTEM "/home/matvey/2023_tizhin215/ООП/t4/filesystem/"

namespace fs = std::filesystem;


int RequestHttp_v1::fill_request(const std::string &str_method, const std::string &str_url){
    if (str_url == "")
        url = "index.html";
    else 
        url = str_url;
    if (fill_command(str_method) == false){
        status_error = NOT_IMPLEMENTED;
        explanation_error = "METHOD NOT SUPPORTED";
        return -1;
    }

    return 0;
}

void RequestHttp_v1::response_banner_request(){
    std::cout << "HTTP/1." << version << " " << this->status_error << " " << this->explanation_error << "\n";
}

void RequestHttp_v1::response_get_head_banner(std::filesystem::path resource_path) {
    fs::file_status resource_status = fs::status(resource_path);
    // Вывод заголовков (GET)HEAD файла с путем resource_path

    // Вывод текущей даты и времени
    std::time_t current_time = std::time(nullptr);
    std::tm* current_tm = std::gmtime(&current_time);
    char current_time_str[80];
    std::strftime(current_time_str, 80, "%a, %d %b %Y %T GMT", current_tm);

    

    std::cout << "Date: " << current_time_str << "\n";
    std::cout << "Host: " << "ServerHttp/1.1:8080\n";
    std::cout << "Referer: " << resource_path << std::endl;
    std::cout << "User-agent: " << "clientsocket\n";
    std::cout << "Server: MyServer/1.0\n";
    if (fs::is_directory(resource_path)){
        int file_count = 0;
        for (const auto& entry : fs::directory_iterator(resource_path)) {
            file_count++;
        }
        std::cout << "Content-Length: " << file_count << "\n";
    }
    else{
        std::cout << "Content-Length: " << fs::file_size(resource_path) << "\n";
    }
    std::cout << "Content-Type: " << resource_path.extension().string() << "\n";
    std::cout << "Allow: GET, HEAD\n";
    std::cout << "Last-Modiried: " << current_time_str << "\n";
    // std::cout << "\n";
}


void RequestHttp_v1::get_execute(){
    //перенаправленный вывод
    switch (access_to_file_directory(PATH_FILESYSTEM, url)){
        case FORBIDDEN:
            this->status_error = FORBIDDEN;
            this->explanation_error = "FORBIDDEN";
            response_banner_request();
            return;
        case NOT_FOUND:
            this->status_error = NOT_FOUND;
            this->explanation_error = "NOT_FOUND";
            response_banner_request();
            return;
        case OK:
            this->status_error = OK;
            this->explanation_error = "OK";
            //потом может изменится
    }
    // if (access_to_file_directory(PATH_FILESYSTEM, url) == false){
    //     std::cout << "Error access file\n";
    //     return;
    // }
    std::string abs_path =  PATH_FILESYSTEM + url;
    fs::path resource_path(abs_path);
    

    if (fs::exists(resource_path)){
        response_banner_request();
        response_get_head_banner(resource_path);
        std::cout << "\n";
        
        //проверка на файл
        if (fs::is_empty(resource_path)){
            //пустой файл или директория
            std::cout << "file is empty\n";
        }
        if (fs::is_regular_file(resource_path)){
            //выводит текст файла
            int pid;
            if ((pid = fork()) == 0){
                execlp("cat", "cat", abs_path.c_str(), nullptr);
                std::cout << "Error execlp\n";
            }
            else{
                int status;
                waitpid(pid, &status, 0);
            }
        }

        if (fs::is_directory(resource_path)){
            //выводит как ls список файлов
            int pid;
            if ((pid = fork()) == 0){
                execlp("ls", "ls", "-l", abs_path.c_str(), nullptr);
                std::cout << "Error execlp\n";
            }
            else{
                int status;
                waitpid(pid, &status, 0);
            }
        }
    }
    else{
        // нет файла
        this->status_error = NOT_FOUND;
        this->explanation_error = "NOT_FOUND";
        response_banner_request();
    }
    
}

void RequestHttp_v1::head_execute(){
    //перенаправленный вывод
    switch (access_to_file_directory(PATH_FILESYSTEM, url)){
        case FORBIDDEN:
            this->status_error = FORBIDDEN;
            this->explanation_error = "FORBIDDEN";
            response_banner_request();
            return;
        case NOT_FOUND:
            this->status_error = NOT_FOUND;
            this->explanation_error = "NOT_FOUND";
            response_banner_request();
            return;
        case OK:
            this->status_error = OK;
            this->explanation_error = "OK";
            //потом может изменится
    }
    // if (access_to_file_directory(PATH_FILESYSTEM, url) == false){
    //     std::cout << "Error access file\n";
    //     return;
    // }
    std::string abs_path =  PATH_FILESYSTEM + url;
    fs::path resource_path(abs_path);
    

    if (fs::exists(resource_path)){
        response_get_head_banner(resource_path);
    }
    
}
void RequestHttp_v1::execute_request_http(){
    
    switch (command){
        case GET:
            get_execute();
            break;
        // case HEAD:
        //     head_execute(url, version);
        //     break;
        case HEAD:
            head_execute();
            break;
        default:
            this->status_error = BAD_REQUEST;
            this->explanation_error = "BAD_REQUEST";
            response_banner_request();
    }
    
}

bool RequestHttp_v1::fill_command(std::string command){ 
    if (command == "GET"){
        this->command = GET;
        return true;
    }
    else if (command == "HEAD"){
        this->command = HEAD;
        return true;
    }
    else{
        return false;
    }
}