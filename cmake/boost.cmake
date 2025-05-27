include(ExternalProject)

set(BOOST_SOURCE_DIR ${CMAKE_SOURCE_DIR}/3dparty/boost)

if(MSVC)
set(BOOST_BOOTSTRAP ${BOOST_SOURCE_DIR}/bootstrap.bat)
set(BOOST_TOOLSET "msvc")
else()
set(BOOST_BOOTSTRAP ${BOOST_SOURCE_DIR}/bootstrap.sh)
set(BOOST_TOOLSET "clang")
endif()

set(BOOST_B2_COMMAND ${BOOST_SOURCE_DIR}/b2
--prefix=${CMAKE_CURRENT_BINARY_DIR}/chess_engine_boost/install
--build-dir=${CMAKE_CURRENT_BINARY_DIR}/chess_engine_boost/build
--without-mpi
toolset=${BOOST_TOOLSET}
address-model=64
variant=$<LOWER_CASE:$<CONFIG>>
link=static
runtime-link=shared
threading=multi
--layout=system
install
)

ExternalProject_Add(chess_engine_boost
SOURCE_DIR ${BOOST_SOURCE_DIR}
PREFIX ${CMAKE_CURRENT_BINARY_DIR}/chess_engine_boost
CONFIGURE_COMMAND ${BOOST_BOOTSTRAP}
BUILD_COMMAND ${BOOST_B2_COMMAND}
INSTALL_COMMAND ""
BUILD_IN_SOURCE 1
)

ExternalProject_Get_Property(chess_engine_boost INSTALL_DIR) # change to BOOST_INSTALL_DIR
set(BOOST_INSTALL_DIR ${INSTALL_DIR})

add_library(boost INTERFACE)
target_include_directories(boost INTERFACE ${BOOST_INSTALL_DIR}/include/)
target_link_directories(boost INTERFACE ${BOOST_INSTALL_DIR}/lib)