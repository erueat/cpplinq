cmake_minimum_required(VERSION 2.8)
project(cpplinq-unittest)

MESSAGE (STATUS "${CMAKE_GENERATOR}")

set (BASE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(
    ${BASE_PATH}/../../cpplinq/include
    ${BASE_PATH}/../../gtest/include
    ${BASE_PATH}/../../gtest
    )

file(GLOB_RECURSE src "${BASE_PATH}/src/*.cpp" "${BASE_PATH}/unittests/*.cpp" "${BASE_PATH}/../../gtest/src/gtest-all.cc")

set (EXECUTABLE_OUTPUT_PATH "${BASE_PATH}/bin")

set (CMAKE_CXX_FLAGS "-std=c++17 -g")

add_executable(cpplinq-unittest ${src})
