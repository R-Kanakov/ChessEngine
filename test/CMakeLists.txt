# Licensed after GNU GPL v3

file(GLOB testfiles "*.cpp")

list(REMOVE_ITEM testfiles "${CMAKE_CURRENT_SOURCE_DIR}/run_tests.cpp")

add_executable(run_tests run_tests.cpp ${testfiles})

target_compile_features(run_tests PUBLIC cxx_std_20)

target_link_libraries(run_tests PUBLIC chesslib)

add_dependencies(run_tests chess_engine_boost)
target_link_directories(run_tests PUBLIC ${CMAKE_BINARY_DIR}/chess_engine_boost/install/lib)

add_dependencies(run_tests chess_engine_googletest)
target_link_directories(run_tests PUBLIC ${CMAKE_BINARY_DIR}/chess_engine_googletest/install/lib)
target_link_libraries(run_tests PUBLIC gtest)

enable_testing()
add_test(NAME tests COMMAND run_tests)