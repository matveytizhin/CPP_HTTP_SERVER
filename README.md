# C++ HTTP Сервер

Это простой HTTP-сервер, написанный на C++. Он предназначен для обработки базовых HTTP-запросов, обслуживания статических файлов, отображения содержимого каталогов и поддержки пользовательских версий протокола с расширенной функциональностью.

Сервер использует неблокирующий ввод-вывод с системным вызовом `poll()` для одновременной обработки нескольких клиентских соединений и `fork()` для обработки каждого запроса в отдельном процессе.

## Особенности

-   **Асинхронная обработка соединений**:
    -   Использование `poll()` для эффективного управления множеством клиентских сокетов без блокировки основного процесса.
    -   Серверный сокет находится в неблокирующем режиме.
-   **Многопроцессная обработка запросов**:
    -   Каждый HTTP-запрос обрабатывается в дочернем процессе, созданном с помощью `fork()`. Это обеспечивает изоляцию и предотвращает блокировку сервера одним запросом.
-   **Поддержка HTTP/1.1**:
    -   `GET`: получение содержимого файла или листинга директории.
    -   `HEAD`: получение заголовков ресурса без тела ответа.
-   **Поддержка кастомной версии HTTP/1.2**:
    -   `DELETE`: удаление файла или директории.
    -   **CGI-подобный механизм**: выполнение исполняемых файлов на сервере с передачей параметров через URL (например, `/script.cgi?var1=val1&var2=val2`).
-   **Обработка статических файлов**: отдает содержимое файлов из корневого каталога сервера.
-   **Листинг директорий**: при запросе к директории выводит ее содержимое с помощью `ls -l`.
-   **Обработка кодов состояния**: корректно возвращает коды HTTP, такие как `200 OK`, `400 Bad Request`, `403 Forbidden`, `404 Not Found`, `501 Not Implemented`.

## Структура проекта

.
├── build/ # Директория для сборки проекта
├── filesystem/ # Корневая директория для файлов сервера
├── .vscode/ # Настройки для Visual Studio Code
├── client # Исполняемый файл клиента
├── client.cpp # Исходный код простого тестового клиента
├── client_class.cpp # Реализация класса Client
├── client_class.h # Заголовочный файл класса Client
├── CMakeLists.txt # Файл конфигурации для CMake
├── http.h # Главный обработчик входящих HTTP-запросов
├── macro.h # Макросы и константы (ВАЖНО: требует настройки)
├── main.cpp # Главный файл сервера (сокет, poll, цикл accept, fork)
├── request_http.cpp # Базовый класс для обработки HTTP-запросов
├── request_http.h #
├── request_http_v1.cpp # Реализация для HTTP/1.1 (GET, HEAD)
├── request_http_v1.h #
├── request_http_v2.cpp # Реализация для HTTP/1.2 (DELETE, CGI)
├── request_http_v2.h #
└── server_testing.cpp # Тестовый код или альтернативная реализация сервера```

## Сборка и запуск HTTP сервера

1. Откройте файл `macro.h`
2. Найдите строку `#define PATH_FILESYSTEM "..."`
3. Замените путь на актуальный путь к директории `filesystem` в вашем проекте:

```cpp
// Пример в macro.h
#define PATH_FILESYSTEM "/home/your_user/path/to/project/filesystem/"


# Подготовка файловой системы
mkdir -p filesystem
echo "<html><body><h1>Hello, World!</h1></body></html>" > filesystem/index.html
echo "This is another file." > filesystem/test.txt

# Сборка проекта
mkdir build
cd build
cmake ..
make

#Запуск сервера
./server_project

#Использование клиента
# В корневой директории проекта
g++ client.cpp -o client
./client

'''

## Примеры тестирования

```cpp

#GET-запрос (HTTP/1.1)
curl http://localhost:8080/index.html

#HEAD-запрос (HTTP/1.1)
curl -I http://localhost:8080/index.html

#DELETE-запрос (HTTP/1.2)
telnet localhost 8080
DELETE /test.txt HTTP/1.2
'''

'''cpp

#CGI-запрос (HTTP/1.2)
// filesystem/cgi_test.c
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Content-Type: text/plain\r\n\r\n");
    printf("CGI script executed!\n");
    
    char* query = getenv("QUERY_STRING");
    if (query) {
        printf("Query: %s\n", query);
    }
    return 0;
}

gcc filesystem/cgi_test.c -o filesystem/cgi_test
telnet localhost 8080
GET /cgi_test?name=user&id=123 HTTP/1.2
'''
