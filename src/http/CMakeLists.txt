add_library(HttpModule
    Request.cpp
    Response.cpp
    Router.cpp
)

target_include_directories(HttpModule
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/thread
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/http
)