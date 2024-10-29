cmake_minimum_required(VERSION 3.10)

project(aos-soa-tests)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(aos-soa-tests aos_soa.cpp)

target_include_directories(aos-soa-tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
