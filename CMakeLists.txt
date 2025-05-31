cmake_minimum_required(VERSION 4.0.2)
set(CMAKE_CXX_STANDARD 23)

project(HTTPServer)

find_package(spdlog REQUIRED)

add_executable(main main.cpp)

add_subdirectory(src/net)
add_subdirectory(src/core)
add_subdirectory(src/conn)
add_subdirectory(src/http)
add_subdirectory(test)

target_include_directories(main
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/log
)


target_link_libraries(main
    PUBLIC
        NetworkingModule
        CoreModule
        ConnectionModule
        HttpModule
)

