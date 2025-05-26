set(LIB_PATH ${CMAKE_SOURCE_DIR}/3dparty/boost/libs)

FILE(GLOB children RELATIVE ${LIB_PATH} ${LIB_PATH}/*)

set(subdirs "")
foreach(child ${children})
  if(IS_DIRECTORY ${LIB_PATH}/${child})
    list(APPEND subdirs ${child})
  endif()
endforeach()

foreach(subd ${subdirs})
  include_directories(${CMAKE_SOURCE_DIR}/3dparty/boost/libs/${subd}/include)
endforeach()

include(ExternalProject)
ExternalProject_Add(chess_engine_boost
        SOURCE_DIR        "${CMAKE_SOURCE_DIR}/3dparty/boost"
        PREFIX            "${CMAKE_CURRENT_BINARY_DIR}/chess_engine_boost"
        BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/chess_engine_boost/build"
        INSTALL_DIR       "${CMAKE_CURRENT_BINARY_DIR}/chess_engine_boost/install"
        CONFIGURE_COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_SOURCE_DIR}/3dparty/boost/"
											 -B "${CMAKE_CURRENT_BINARY_DIR}/chess_engine_boost/build/"
											 -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
											 -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
											 -G ${CMAKE_GENERATOR}
											 #-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        BUILD_COMMAND     "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/chess_engine_boost/build"
											 #--config ${CMAKE_BUILD_TYPE}
        INSTALL_COMMAND   "${CMAKE_COMMAND}" --install "${CMAKE_CURRENT_BINARY_DIR}/chess_engine_boost/build"
											 --prefix "${CMAKE_CURRENT_BINARY_DIR}/chess_engine_boost/install"
        TEST_COMMAND      ""
)
