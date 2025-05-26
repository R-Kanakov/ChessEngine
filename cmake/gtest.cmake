if(MSVC)
    option(gtest_force_shared_crt "" TRUE)
endif(MSVC)

include_directories(${CMAKE_SOURCE_DIR}/3dparty/googletest/googletest/include)

include(ExternalProject)
ExternalProject_Add(chess_engine_googletest
        SOURCE_DIR        "${CMAKE_SOURCE_DIR}/3dparty/googletest"
        PREFIX            "${CMAKE_CURRENT_BINARY_DIR}/chess_engine_googletest"
        BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/chess_engine_googletest/build"
        INSTALL_DIR       "${CMAKE_CURRENT_BINARY_DIR}/chess_engine_googletest/install"
        CONFIGURE_COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_SOURCE_DIR}/3dparty/googletest/"
			  		                         -B "${CMAKE_CURRENT_BINARY_DIR}/chess_engine_googletest/build/"
        		  		                     -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
											 -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
											 -G ${CMAKE_GENERATOR}
											 -DCMAKE_BUILD_TYPE=$<CONFIG>
											 -Dgtest_force_shared_crt=ON
											 -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/chess_engine_googletest/install
        BUILD_COMMAND     "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/chess_engine_googletest/build"
											 --config $<CONFIG>
        INSTALL_COMMAND   "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/chess_engine_googletest/build" 
											 --target install
											 --config $<CONFIG>
)
