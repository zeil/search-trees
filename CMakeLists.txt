cmake_minimum_required(VERSION 3.1.3)
project(search-trees)
include_directories(include)
set(CMAKE_CXX_STANDARD 14)
add_executable(simple-test tests/simple.cpp)
add_executable(file-test tests/file.cpp)
