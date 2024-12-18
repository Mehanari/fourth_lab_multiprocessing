cmake_minimum_required(VERSION 3.29)
project(fourth_lab_multiprocessing)

set(CMAKE_CXX_STANDARD 11)
# added -fopenmp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")

add_executable(fourth_lab_multiprocessing main.cpp)
