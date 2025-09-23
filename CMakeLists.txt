cmake_minimum_required(VERSION 3.10)
project(server_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(server_project main.cpp request_http_v1.cpp request_http_v2.cpp request_http.cpp)

target_link_libraries(server_project)