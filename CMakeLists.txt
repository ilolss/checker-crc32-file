cmake_minimum_required(VERSION 3.0.0)
project(file_checksum VERSION 0.1.0 LANGUAGES C CXX)

set(BOOST_MINVERSION 1.74.0)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost ${BOOST_MINVERSION} QUIET)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fsanitize=address")

include(CTest)
enable_testing()

set(PROJECT_SOURCES 
    src/main.cpp 
    src/thread_pool.cpp 
    src/thread_safe_map.cpp 
    src/checksum.cpp 
    src/logger.cpp
)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS} include/)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
