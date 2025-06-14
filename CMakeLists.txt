# Licensed after GNU GPL v3

cmake_minimum_required(VERSION 3.11)

set(PROJECT_NAME Chess_Engine)

project(${PROJECT_NAME})

include(cmake/gtest.cmake)
include(cmake/boost.cmake)

SET(SRCS
  src/board.cpp
  src/init.cpp
  src/move.cpp
  src/makemove.cpp
)

add_library(chesslib STATIC ${SRCS})
target_include_directories(chesslib PUBLIC "include")
target_compile_features(chesslib PUBLIC cxx_std_20)
target_compile_definitions(chesslib PUBLIC
  $<$<CONFIG:Debug>:DEBUG_ONLY=1>
  $<$<CONFIG:Release>:RELEASE_ONLY=1>
)

add_executable(${PROJECT_NAME} main/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE chesslib)

add_subdirectory(test)
