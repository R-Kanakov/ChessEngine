set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/3dparty/boost")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED)