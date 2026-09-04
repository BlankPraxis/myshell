cmake_minimum_required(VERSION 3.10.0)
project(shell VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(shell 
    src/main.cpp
    src/service.cpp
    src/builtins.cpp
)

target_include_directories(shell PRIVATE include)
