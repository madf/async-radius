cmake_minimum_required (VERSION 3.13)

project (radproto VERSION 1.0.0)

option (BUILD_SAMPLE_SERVER "Build sample server." OFF)

set (CPACK_PACKAGE_NAME ${PROJECT_NAME})
set (CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

set ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

enable_language (CXX)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -Wshadow -Wlogical-op -Wuseless-cast -Wno-long-long -Wold-style-cast -Wstrict-aliasing -pedantic")

set ( THREADS_PREFER_PTHREAD_FLAG ON )
find_package ( Threads REQUIRED )

find_package ( Boost 1.66.0 REQUIRED )
find_package ( OpenSSL 1.0.0 REQUIRED )

add_subdirectory (src)

if(BUILD_SAMPLE_SERVER)
    add_subdirectory(sample)
endif(BUILD_SAMPLE_SERVER)

add_custom_target (cppcheck COMMAND cppcheck --enable=all --std=c++14 ${CMAKE_SOURCE_DIR}/src)
