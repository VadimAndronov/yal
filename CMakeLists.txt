cmake_minimum_required(VERSION 2.8)
project(cw_yal)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/usr/include/boost/spirit/include)

set(SOURCE_FILES main.cpp)
add_executable(cw_yal ${SOURCE_FILES} parser.cpp parser.h ast.cpp ast.h grammar.hpp interpreter.cpp interpreter.h environment.cpp environment.h ast_routines.cpp ast_routines.h exception.hpp dummy.hpp)