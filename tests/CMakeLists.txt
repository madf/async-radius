find_package (Boost 1.66.0 REQUIRED unit_test_framework)

add_definitions (-DBOOST_TEST_DYN_LINK)
include_directories (${CMAKE_SOURCE_DIR}/include)

add_executable (error_tests error_tests.cpp)
target_link_libraries (error_tests radproto Boost::unit_test_framework)
add_test (error error_tests)
