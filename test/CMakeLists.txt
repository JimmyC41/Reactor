find_package(GTest REQUIRED)

enable_testing()

add_executable(UnitTests
    RoutingTest.cpp
    ../src/http/Request.cpp
    ../src/http/Response.cpp
    ../src/http/Router.cpp
)

target_include_directories(UnitTests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/http
)

target_link_libraries(UnitTests
    PRIVATE
        GTest::gtest_main
)