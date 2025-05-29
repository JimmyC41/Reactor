add_library(CoreModule
    Server.cpp
)

target_include_directories(CoreModule
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/net
        ${CMAKE_SOURCE_DIR}/include/conn
        ${CMAKE_SOURCE_DIR}/include/http
        ${CMAKE_SOURCE_DIR}/include/thread
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/core
)

target_link_libraries(CoreModule
    PRIVATE
        ThreadModule
)