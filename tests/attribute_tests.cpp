#define BOOST_TEST_MODULE RadProtoAttributeTests

#include "radproto/attribute.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wparentheses"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

BOOST_AUTO_TEST_SUITE()

BOOST_AUTO_TEST_CASE(ClassStringToString)
{
    std::vector<uint8_t> d {'t', 'e', 's', 't'};
    RadProto::String str(1, d.data(), d.size());

    BOOST_CHECK_EQUAL(str.toString(), "test");
}

/*BOOST_AUTO_TEST_CASE(ClassStringToVector)
{
    RadProto::String v(1, "test");

    BOOST_CHECK_EQUAL(v.toVector({}, {}), {1, 6, "t", "e", "s", "t"});
}*/

BOOST_AUTO_TEST_SUITE_END()
