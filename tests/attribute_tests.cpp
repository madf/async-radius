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

BOOST_AUTO_TEST_CASE(ClassStringToVector)
{
    RadProto::String v(1, "test");
    std::vector<uint8_t> values = v.toVector({}, {});
    std::vector<uint8_t> expected {1, 6, 't', 'e', 's', 't'};

    BOOST_CHECK_EQUAL_COLLECTIONS(values.begin(), values.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(ClassStringType)
{
    RadProto::String t(1, "test");

    BOOST_CHECK_EQUAL(t.type(), 1);
}

BOOST_AUTO_TEST_CASE(ClassStringClone)
{
    RadProto::String c(1, "test");

    BOOST_CHECK_EQUAL(c.clone()->type(), 1);

    std::vector<uint8_t> d {'t', 'e', 's', 't'};
    RadProto::String str(1, d.data(), d.size());

    BOOST_CHECK_EQUAL(str.clone()->toString(), "test");

    std::vector<uint8_t> values = c.clone()->toVector({}, {});
    std::vector<uint8_t> expected {1, 6, 't', 'e', 's', 't'};

    BOOST_CHECK_EQUAL_COLLECTIONS(values.begin(), values.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_SUITE_END()
