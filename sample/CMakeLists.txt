add_executable ( radiusd main.cpp server.cpp)

target_link_libraries ( radiusd radproto )

target_link_libraries ( radiusd OpenSSL::Crypto Boost::boost Threads::Threads )

target_include_directories ( radiusd PUBLIC ${CMAKE_SOURCE_DIR}/src )

target_include_directories ( radiusd PUBLIC ${CMAKE_BINARY_DIR}/src )
