cmake_minimum_required(VERSION 4.0.2)

project(HTTPServer)

add_executable(dummy_main main.cpp)

add_subdirectory(src/net)

target_link_libraries(dummy_main
PUBLIC
    NetworkingModule
)