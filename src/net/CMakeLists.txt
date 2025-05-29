add_library(NetworkingModule
    TCPListener.cpp
    TCPStream.cpp
    KQueueReactor.cpp
)

target_link_libraries(NetworkingModule
    PUBLIC
        ThreadModule
)

target_include_directories(NetworkingModule
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/core
        ${CMAKE_SOURCE_DIR}/include/thread
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/net
)
