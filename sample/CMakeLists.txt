add_executable ( ${PROJECT_NAME} main.cpp )

target_link_libraries ( ${PROJECT_NAME} radproto )

target_link_libraries ( ${PROJECT_NAME} OpenSSL::Crypto Boost::boost Threads::Threads )
