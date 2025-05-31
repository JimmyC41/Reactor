add_library(HttpModule
    Request.cpp
    Response.cpp
    Router.cpp
)

target_link_libraries(HttpModule
    spdlog::spdlog
)

target_include_directories(HttpModule
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/http
)