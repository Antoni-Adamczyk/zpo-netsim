cmake_minimum_required(VERSION 3.20)

project(zpo_netsim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(zpo_netsim
    src/main.cpp
        src/factory.cpp
        include/factory.hpp
        src/helpers.cpp
        include/helpers.hpp
        src/nodes.cpp
        include/nodes.hpp
        src/package.cpp
        include/package.hpp
        src/reports.cpp
        src/reports.hpp
)

target_include_directories(zpo_netsim
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)
