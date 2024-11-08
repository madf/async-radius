find_package (Boost 1.66.0 REQUIRED unit_test_framework)

add_definitions (-DBOOST_TEST_DYN_LINK)
include_directories (${CMAKE_SOURCE_DIR}/include)

add_executable (error_tests error_tests.cpp)
target_link_libraries (error_tests radproto Boost::unit_test_framework)
add_test (error error_tests)

add_executable (attribute_tests attribute_tests.cpp)
target_link_libraries (attribute_tests radproto Boost::unit_test_framework)
add_test (attribute attribute_tests)

add_executable (packet_tests packet_tests.cpp utils.cpp)
target_link_libraries (packet_tests radproto Boost::unit_test_framework)
add_test (packet packet_tests)

add_executable (dictionaries_tests dictionaries_tests.cpp)
target_link_libraries (dictionaries_tests radproto Boost::unit_test_framework)
add_test (dictionaries dictionaries_tests)

add_executable (socket_tests socket_tests.cpp utils.cpp)
target_link_libraries (socket_tests radproto Boost::unit_test_framework)
add_test (socket socket_tests)

if (ENABLE_COVERAGE)
    set (COVERAGE_EXCLUDES '${CMAKE_SOURCE_DIR}/tests/*' '*boost*' '/usr/*')
    add_custom_target(ctest COMMAND ${CMAKE_CTEST_COMMAND})
    SETUP_TARGET_FOR_COVERAGE(NAME all_coverage EXECUTABLE ctest -j ${PROCESSOR_COUNT})
endif (ENABLE_COVERAGE)

configure_file(dictionary dictionary COPYONLY)
configure_file(dictionary.1 dictionary.1 COPYONLY)
configure_file(dictionary.dlink dictionary.dlink COPYONLY)
