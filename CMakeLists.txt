cmake_minimum_required(VERSION 2.6)

find_package(GTest REQUIRED)
include_directories((${GTEST_INCLUDE_DIRS}))

add_executable(runTests CompilerTest.cpp)
target_link_libraries(runTests gtest gtest_main pthread)