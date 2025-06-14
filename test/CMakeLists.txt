# Licensed after GNU GPL v3

file(GLOB testfiles "*.cpp")
list(REMOVE_ITEM testfiles "${CMAKE_CURRENT_SOURCE_DIR}/run_tests.cpp")

add_executable(run_tests run_tests.cpp ${testfiles})

target_compile_features(run_tests PUBLIC cxx_std_20)

set(PERFT_TESTS_FILE_PATH "\"${CMAKE_CURRENT_SOURCE_DIR}/Perft_tests.txt\"")
target_compile_definitions(run_tests PRIVATE PERFT_TESTS_PATH=${PERFT_TESTS_FILE_PATH})

target_link_libraries(run_tests PUBLIC chesslib)

add_dependencies(run_tests chess_engine_googletest)
target_link_directories(run_tests PUBLIC ${CMAKE_BINARY_DIR}/chess_engine_googletest/install/lib)
target_link_libraries(run_tests PUBLIC
  $<$<CONFIG:Debug>:gtestd>
  $<$<CONFIG:Release>:gtest>
)

execute_process(
  COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/number_of_perft_tests.py "${CMAKE_CURRENT_SOURCE_DIR}/Perft_tests.txt"
  OUTPUT_VARIABLE PERFT_TESTS
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

target_compile_definitions(run_tests PRIVATE PERFT_TESTS_NUM=${PERFT_TESTS})

if(Boost_FOUND)
  target_include_directories(run_tests PUBLIC ${Boost_INCLUDE_DIRS})
  target_link_libraries(run_tests PUBLIC ${Boost_LIBRARIES})
else()
  message(FATAL_ERROR "Boost not found")
endif()

enable_testing()
add_test(NAME tests COMMAND run_tests)