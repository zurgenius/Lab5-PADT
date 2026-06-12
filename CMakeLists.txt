cmake_minimum_required(VERSION 3.16)
project(relational_algebra_optimizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(relalg_demo
    src/common.cpp
    src/io_streams.cpp
    src/main.cpp
    src/ops.cpp
    src/query_optimizer.cpp
    src/table_source.cpp
)

target_include_directories(relalg_demo PRIVATE include)
target_compile_definitions(relalg_demo PRIVATE PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
