#ifndef REQUEST_HTTP_V1
#define REQUEST_HTTP_V1

#include <string>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <sys/wait.h>
#include <string>
#include <ctime>

#include "client_class.h"
#include "request_http.h"
#include "macro.h"

namespace fs = std::filesystem;

class RequestHttp_v1 :public RequestHttp{
public:
    RequestHttp_v1(){
        command = 0;
        url = "";
        version = 0;
    }
    RequestHttp_v1(int command_http, std::string url_http, int version_http){
        this->command = command_http;
        this->url = url_http;
        this->version = version_http;
    }
// protected:h
    void get_execute() override;    
    void head_execute() override;  
    void execute_request_http() override;
    bool fill_command(std::string command) override;  
    void response_banner_request() override;
    void response_get_head_banner(fs::path resource_path);

    int fill_request(const std::string &str_method, const std::string &str_url);
    
public:
    ~RequestHttp_v1(){}
};


#endif //REQUEST_HTTP_V1