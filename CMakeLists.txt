cmake_minimum_required(VERSION 3.12)

project(TimingPractice)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES timing_practice.cpp)
SET(HEADER_FILES timing_practice.hpp)

add_executable(TimingPractice ${SOURCE_FILES} ${HEADER_FILES})