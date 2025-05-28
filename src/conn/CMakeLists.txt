add_library(ConnectionModule
    ConnectionContext.cpp
)

target_include_directories(ConnectionModule
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/net
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/conn
)