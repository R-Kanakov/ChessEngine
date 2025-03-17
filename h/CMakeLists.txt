file(GLOB_RECURSE hdrs "*.h")
file(GLOB_RECURSE srcs "*.cpp")

add_library(${Enginelib} STATIC ${hdrs} ${srcs})
target_include_directories(${Enginelib} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})