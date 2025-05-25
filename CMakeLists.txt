cmake_minimum_required(VERSION 3.11)

set(PROJECT_NAME Chess_Engine)
project(${PROJECT_NAME})

SET(SRCS
  src/board.cpp
  src/init.cpp
  src/move.cpp
)

SET(TESTS ${CMAKE_SOURCE_DIR}/Tests)

add_library(chesslib STATIC ${SRCS})
target_include_directories(chesslib PUBLIC "inc")

target_compile_features(chesslib PUBLIC cxx_std_20)

target_compile_definitions(chesslib PRIVATE
  $<$<CONFIG:Debug>:DEBUG_ONLY=1>
  $<$<CONFIG:Release>:RELEASE_ONLY=1>
)

add_executable(Engine main/main.cpp)
target_link_libraries(Engine chesslib)

