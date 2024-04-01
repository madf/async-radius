#define BOOST_TEST_MODULE RadProtoErrorTests

#include "radproto/error.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wparentheses"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

BOOST_AUTO_TEST_SUITE()

BOOST_AUTO_TEST_CASE(ErrorCodeConstruction)
{
const auto ec = RadProto::make_error_code(RadProto::Error::success);

BOOST_CHECK_EQUAL(ec.category().name(), "radproto");
}

BOOST_AUTO_TEST_SUITE_END()
