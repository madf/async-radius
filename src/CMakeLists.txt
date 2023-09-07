configure_file ( version.h.in version.h ESCAPE_QUOTES @ONLY )

add_library ( radproto STATIC )

target_sources ( radproto PRIVATE server.cpp packet.cpp attribute.cpp vendor_attribute.cpp utils.cpp dictionaries.cpp)

target_include_directories ( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

target_link_libraries ( ${PROJECT_NAME} OpenSSL::Crypto Boost::boost Threads::Threads )

target_include_directories ( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} )
