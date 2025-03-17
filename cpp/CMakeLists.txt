file(GLOB_RECURSE srcs "*.cpp")

add_executable(${Enginesrc} ${srcs})
target_link_libraries(${Enginesrc} ${Enginelib})
target_include_directories(${Enginesrc} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
