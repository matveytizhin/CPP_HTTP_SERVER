#include "request_http_v2.h"

#define PATH_FILESYSTEM "/home/matvey/2023_tizhin215/ООП/t4/filesystem/"

namespace fs = std::filesystem;

void RequestHttp_v2::execute_request_http(){
    
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
        case DELETE:
            delete_execute();
            break;
        default:
            this->status_error = BAD_REQUEST;
            this->explanation_error = "BAD_REQUEST";
            response_banner_request();
    }
    
}

bool RequestHttp_v2::fill_command(std::string command){ 
    //при вызове этой функции url уже заполнен
    if (command == "GET"){
        //если задействован общий шлюзововой интерфейс, то разбиваем str_url на действительный url и переменные окружения
        if (url.find('?') != std::string::npos){
            flag_cgi = 1;
            fill_cgi_inf(); //заполняет vector переменных окружения и url
        }
        this->command = GET;
        return true;
    }
    else if (command == "HEAD"){
        this->command = HEAD;
        return true;
    }
    else if (command == "DELETE"){
        this->command = DELETE;
        return true;
    }
    else{
        return false;
    }
}

void RequestHttp_v2::response_delete_banner(){
    std::cout << "FILE DELETED SUCCESSFULLY\n";
}

void RequestHttp_v2::update_environment_variables() {
    // Проходим по всем переменным окружения из map_var_env
    for (const auto& pair : map_var_env) {
        // Получаем имя переменной и ее значение
        std::string var_name = pair.first;
        std::string var_value = pair.second;
        
        // Преобразуем имя и значение переменной в формат, подходящий для функции setenv
        std::string env_var = var_name + "=" + var_value;
        
        // Вызываем setenv для обновления переменной окружения
        if (setenv(var_name.c_str(), var_value.c_str(), 1) != 0) {
            std::cerr << "Failed to update environment variable: " << var_name << std::endl;
        }
    }
}

void RequestHttp_v2::fill_cgi_inf() {
    // Ищем позицию знака '?'
    const std::string str_cgi_url = url;
    size_t pos = str_cgi_url.find('?');
    if (pos != std::string::npos) {
        // Получаем URI и записываем его в поле url
        url = str_cgi_url.substr(0, pos);

        // Получаем часть строки с переменными окружения
        std::string env_vars = str_cgi_url.substr(pos + 1);

        // Парсим переменные окружения и записываем в словарь
        size_t start = 0;
        size_t end = 0;
        while ((end = env_vars.find('&', start)) != std::string::npos) {
            std::string pair = env_vars.substr(start, end - start);
            size_t separator = pair.find('=');
            if (separator != std::string::npos) {
                std::string key = pair.substr(0, separator);
                std::string value = pair.substr(separator + 1);
                map_var_env[key] = value;
            }
            start = end + 1;
        }

        // Обрабатываем последнюю пару
        std::string pair = env_vars.substr(start);
        size_t separator = pair.find('=');
        if (separator != std::string::npos) {
            std::string key = pair.substr(0, separator);
            std::string value = pair.substr(separator + 1);
            map_var_env[key] = value;
        }
    }
}

void RequestHttp_v2::get_execute(){
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
        if (flag_cgi){
            update_environment_variables();
            const int buf_size = abs_path.size() + 1;
            char abs_path_buffer[buf_size];
            std::strcpy(abs_path_buffer, abs_path.c_str());
            char *const argv[] = {abs_path_buffer, nullptr};
            int status;
            if (fork() == 0){
                execvp(abs_path_buffer, argv);
            }
            else{
                wait(&status);
            }
            return;
        }
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
        //в этот иф программа никогда не зайдет
        this->status_error = NOT_FOUND;
        this->explanation_error = "NOT_FOUND";
        response_banner_request();
    }
    
}

void RequestHttp_v2::delete_execute(){
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
    
    std::string abs_path =  PATH_FILESYSTEM + url;
    fs::path resource_path(abs_path);
    

    if (fs::exists(resource_path)){
        try {
            if (fs::is_regular_file(resource_path))
                fs::remove(resource_path);
            if (fs::is_directory(resource_path)){
                fs::remove_all(resource_path);
            }
            response_banner_request();
            response_delete_banner();
        } catch (const std::filesystem::filesystem_error& e){
            status_error = FORBIDDEN;
            explanation_error = "SOME DIRECTORY FILES CANNOT BE DELETED\n";
            response_banner_request();
        }
    }

}