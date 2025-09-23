#ifndef CLIENT_CLASS
#define CLIENT_CLASS

#include <string>
#include <vector>

class Client{
public:
    int fd;
    int mode;
    std::vector<std::string> list_add_mode_file;
    Client(){};
    Client(int fd);
    Client(int fd, int mode);
    Client(const Client &other) : fd(other.fd), mode(other.mode), list_add_mode_file(other.list_add_mode_file) {}
    
    ~Client(){};
};

#endif //CLIENT_CLASS