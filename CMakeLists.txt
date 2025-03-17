cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME Engine)
project(${PROJECT_NAME})

find_program(CMAKE_C_COMPILER clang)
find_program(CMAKE_CXX_COMPILER clang++)

if (NOT CMAKE_C_COMPILER)
  polly_fatal_error("clang not found")
endif()

if (NOT CMAKE_CXX_COMPILER)
  polly_fatal_error("clang++ not found")
endif()

set(
    CMAKE_C_COMPILER
    "${CMAKE_C_COMPILER}"
    CACHE
    STRING
    "C compiler"
    FORCE
)

set(
    CMAKE_CXX_COMPILER
    "${CMAKE_CXX_COMPILER}"
    CACHE
    STRING
    "C++ compiler"
    FORCE
)

set(CMAKE_CXX_STANDARD 20)

set(Enginelib EngineLib)
set(Enginesrc EngineExe)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/h)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cpp)