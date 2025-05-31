add_library(CoreModule
    Server.cpp
)

target_link_libraries(CoreModule
    PUBLIC
        spdlog::spdlog
)

target_include_directories(CoreModule
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/net
        ${CMAKE_SOURCE_DIR}/include/conn
        ${CMAKE_SOURCE_DIR}/include/http
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/core
        ${CMAKE_SOURCE_DIR}/include/log
)