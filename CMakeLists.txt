cmake_minimum_required(VERSION 3.26)
project(Syntax_Analyzer)

set(CMAKE_CXX_STANDARD 14)

include_directories(libs/cpp-httplib)
include_directories(libs/json-develop/single_include/nlohmann)

add_executable(Syntax_Analyzer main.cpp)
target_link_libraries(Syntax_Analyzer ws2_32 wsock32)
