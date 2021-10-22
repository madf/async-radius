cmake_minimum_required (VERSION 3.13)

project (radiusd VERSION 1.0.0)

set (CPACK_PACKAGE_NAME ${PROJECT_NAME})
set (CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

set ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

enable_language (CXX)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -Wshadow -Wlogical-op -Wuseless-cast -Wno-long-long -Wold-style-cast -Wstrict-aliasing -pedantic")

find_package ( Boost 1.66.0 REQUIRED )

add_executable ( ${PROJECT_NAME} )

add_subdirectory (src)

add_custom_target (cppcheck COMMAND cppcheck --enable=all --std=c++14 ${CMAKE_SOURCE_DIR}/src)
