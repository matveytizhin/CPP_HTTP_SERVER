#ifndef REQUEST_HTTP_V2
#define REQUEST_HTTP_V2

#include <string>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/wait.h>
#include <string>
#include <ctime>
#include <map>

#include "client_class.h"
#include "request_http.h"
#include "macro.h"
#include "request_http_v1.h"
namespace fs = std::filesystem;

class RequestHttp_v2 :public RequestHttp_v1{
public:
    bool flag_cgi;
    std::map<std::string, std::string> map_var_env;

    RequestHttp_v2() : RequestHttp_v1(){this->flag_cgi = 0;};
    RequestHttp_v2(int command_http, std::string url_http, int version_http) : RequestHttp_v1(command_http, url_http, version_http) {};
// protected:
    void execute_request_http() override;
    bool fill_command(std::string command) override;
    void get_execute() override;

    void fill_cgi_inf(); 
    void update_environment_variables();
    void delete_execute();
    void response_delete_banner();
    
public:
    ~RequestHttp_v2(){}
};


#endif //REQUEST_HTTP_V2