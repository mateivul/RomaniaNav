cmake_minimum_required(VERSION 3.10)
project(MoldovaNavigatorBackend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)

add_executable(moldova_server
    src/main.cpp
    src/graph.cpp
)

find_package(Threads REQUIRED)
target_link_libraries(moldova_server Threads::Threads)

if(WIN32)
    target_link_libraries(moldova_server ws2_32)
endif()
