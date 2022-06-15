configure_file ( version.h.in version.h ESCAPE_QUOTES @ONLY )

target_sources ( ${PROJECT_NAME} PRIVATE main.cpp server.cpp packet.cpp attribute.cpp)

target_link_libraries ( ${PROJECT_NAME} OpenSSL::Crypto Boost::boost Threads::Threads )

target_include_directories ( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} )
