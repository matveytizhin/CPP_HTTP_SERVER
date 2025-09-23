#include <iostream>
#include <ctime>
#include <cstdlib> // Для доступа к переменным окружения

int main() {
    // Переменные окружения
    const char* referer = std::getenv("REFERER");
    const char* user_agent = std::getenv("USER_AGENT");

    // Вывод текущей даты и времени
    std::time_t current_time = std::time(nullptr);
    std::tm* current_tm = std::gmtime(&current_time);
    char current_time_str[80];
    std::strftime(current_time_str, 80, "%a, %d %b %Y %T GMT", current_tm);

    // Вывод заголовков HTTP
    std::cout << "Date: " << current_time_str << "\n";
    std::cout << "Host: " << "ServerHttp/1.1:8080\n";
    if (referer) {
        std::cout << "Referer: " << referer << std::endl;
    }
    if (user_agent) {
        std::cout << "User-agent: " << user_agent << "\n";
    }
    std::cout << "Server: MyServer/1.0\n";

    // Дополнительные операции, зависящие от ресурса (resource_path)
    // Здесь вы можете добавить логику для обработки ресурса, например, подсчет файла или определение его типа
    // Здесь просто пример

    // Пример подсчета файлов в директории (для иллюстрации)
    std::cout << "Content-Length: " << "123" << "\n";

    // Пример определения типа контента (для иллюстрации)
    std::cout << "Content-Type: " << "text/plain" << "\n";
    std::cout << "Allow: GET, HEAD\n";
    std::cout << "Last-Modified: " << current_time_str << "\n";

    return 0;
}

