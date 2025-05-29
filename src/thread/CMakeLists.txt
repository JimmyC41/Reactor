find_package(TBB REQUIRED)

add_library(ThreadModule
    ThreadPool.cpp
)

target_link_libraries(ThreadModule
    PUBLIC
        TBB::tbb
)

target_include_directories(ThreadModule
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/thread
)
