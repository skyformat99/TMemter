cmake_minimum_required(VERSION 2.8)

set(CXX_FLAGS -g -Wall -std=c++11)
set(CMAKE_CXX_COMPILER "g++")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(TMemter TMemter.cc main.cc)
