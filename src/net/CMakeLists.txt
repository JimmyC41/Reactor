add_library(NetworkingModule
    TCPListener.cpp
    TCPStream.cpp
    KQueueReactor.cpp
)

target_include_directories(NetworkingModule
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/net
)
