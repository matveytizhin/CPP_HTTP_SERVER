#include "client_class.h"

Client::Client(int fd, int mode){
        this->fd = fd;
        this->mode = mode;
    }
Client::Client(int fd){
        this->fd = fd;
    }