#define BOOST_TEST_MODULE RadProtoDictionariesTests

#include "radproto/socket.h"
#include "radproto/error.h"
#include <string>
#include <map>
#include <cstdint> //uint8_t, uint32_t
#include <stdexcept>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wparentheses"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

BOOST_AUTO_TEST_SUITE(SocketTests)

BOOST_AUTO_TEST_CASE(TestConstructor)
{
boost::asio::io_service io_service;

BOOST_CHECK_NO_THROW(RadProto::Socket s(io_service, "secret", 1812));

}

BOOST_AUTO_TEST_SUITE_END()
