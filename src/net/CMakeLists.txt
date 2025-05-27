add_library(NetworkingModule
    TCPListener.cpp
    TCPStream.cpp
)

target_include_directories(NetworkingModule
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/net
)
