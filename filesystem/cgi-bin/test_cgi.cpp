#include <iostream>
#include <cstdlib>
#include <string>

int main() {
    std::string var_name = std::getenv("VAR_NAME");
    std::string var_port = std::getenv("VAR_PORT");

    if (!var_name.empty() && !var_port.empty()) {
        std::cout << "Name: " << var_name << std::endl;
        std::cout << "Port: " << var_port << std::endl;
    } else {
        std::cout << "Environment variables VAR_NAME and VAR_PORT are not set." << std::endl;
    }

    return 0;
}
