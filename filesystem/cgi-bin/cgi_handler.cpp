#include <iostream>

int main() {
    // Выводим заголовок Content-Type
    std::cout << "Content-Type: text/html\n\n";
    // Выводим HTML-код страницы
    std::cout << "<html><body>\n";
    std::cout << "<h1>Hello, CGI!</h1>\n";
    std::cout << "</body></html>\n";
    
    return 0;
}
