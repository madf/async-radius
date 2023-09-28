add_executable ( ${PROJECT_NAME} main.cpp )

target_link_libraries ( ${PROJECT_NAME} radproto )

target_link_libraries ( ${PROJECT_NAME} OpenSSL::Crypto Boost::boost Threads::Threads )

target_include_directories ( ${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src )

target_include_directories ( ${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/src )
