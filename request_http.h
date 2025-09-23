#ifndef REQUEST_HTTP
#define REQUEST_HTTP
#include "client_class.h"

#include <string>
#include <filesystem>
#include "macro.h"
class RequestHttp{
public:
    RequestHttp(){}
    RequestHttp(int command_http, std::string url_http, int version_http){
        this->command = command_http;
        this->url = url_http;
        this->version = version_http;
    }
// protected:
    int command;
    std::string url;
    int version;
    Client client;
    int status_error;
    std::string explanation_error;

    virtual void get_execute() = 0;
    virtual void head_execute() = 0; 
    virtual void execute_request_http() = 0; 
    virtual bool fill_command(std::string command) = 0;
    virtual void response_banner_request() = 0;
    virtual int fill_request(const std::string &str_method, const std::string &str_url) = 0;

    static int access_to_file_directory(const std::string &path_file_system, const std::string &url);

public: 
    virtual ~RequestHttp(){}
};

#endif //REQUEST_HTTPs