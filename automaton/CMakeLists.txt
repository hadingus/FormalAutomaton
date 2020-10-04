cmake_minimum_required(VERSION 3.16)
project(automaton)

set(CMAKE_CXX_STANDARD 20)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR} automaton_src)
add_subdirectory(automaton_src)

add_executable(CTestsExe main_test.cpp gtests/tests.cpp)
target_link_libraries(CTestsExe ${GTEST_LIBRARIES} AutomatonLib)
