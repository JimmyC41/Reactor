add_library(NetworkingModule
    TCPListener.cpp
    TCPStream.cpp
    KQueueReactor.cpp
)

target_include_directories(NetworkingModule
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/core
        ${CMAKE_SOURCE_DIR}/include/conn
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/net
)

target_link_libraries(NetworkingModule
    PUBLIC
        spdlog::spdlog
)