cmake_minimum_required(VERSION 3.10)
project(CminusCompiler)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(include)

# 添加可执行文件
add_executable(cminus_compiler 
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/main.cpp
)