#define BOOST_TEST_MODULE RadProtoAttributeTests

#include "radproto/attribute.h"
#include <memory>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wparentheses"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

BOOST_AUTO_TEST_SUITE(AttributeTests)

BOOST_AUTO_TEST_CASE(StringDataConstructor)
{
    std::vector<uint8_t> d {'t', 'e', 's', 't'};
    RadProto::String s(1, d.data(), d.size());

    BOOST_CHECK_EQUAL(s.toString(), "test");

    std::vector<uint8_t> values = s.toVector({}, {});
    std::vector<uint8_t> expected {1, 6, 't', 'e', 's', 't'};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(s.type(), 1);
}

BOOST_AUTO_TEST_CASE(StringValueConstructor)
{
    RadProto::String v(1, "test");

    BOOST_CHECK_EQUAL(v.toString(), "test");

    std::vector<uint8_t> values = v.toVector({}, {});
    std::vector<uint8_t> expected {1, 6, 't', 'e', 's', 't'};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(v.type(), 1);
}

BOOST_AUTO_TEST_CASE(StringClone)
{
    RadProto::String c(1, "test");
    std::unique_ptr<RadProto::Attribute> cs(c.clone());

    BOOST_CHECK_EQUAL(cs->toString(), "test");

    std::vector<uint8_t> values = cs->toVector({}, {});
    std::vector<uint8_t> expected {1, 6, 't', 'e', 's', 't'};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(cs->type(), 1);
}

BOOST_AUTO_TEST_CASE(IntegerDataConstructor)
{
    std::vector<uint8_t> d {0, 0, 0, 20};
    RadProto::Integer s(5, d.data(), d.size());

    BOOST_CHECK_EQUAL(s.toString(), "20");

    std::vector<uint8_t> values = s.toVector({}, {});
    std::vector<uint8_t> expected {5, 6, 0, 0, 0, 20};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(s.type(), 5);
}

BOOST_AUTO_TEST_CASE(IntegerValueConstructor)
{
    RadProto::Integer v(5, 20);

    BOOST_CHECK_EQUAL(v.toString(), "20");

    std::vector<uint8_t> values = v.toVector({}, {});
    std::vector<uint8_t> expected {5, 6, 0, 0, 0, 20};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(v.type(), 5);
}

BOOST_AUTO_TEST_CASE(IntegerClone)
{
    RadProto::Integer c(5, 20);
    std::unique_ptr<RadProto::Attribute> cs(c.clone());

    BOOST_CHECK_EQUAL(cs->toString(), "20");

    std::vector<uint8_t> values = cs->toVector({}, {});
    std::vector<uint8_t> expected {5, 6, 0, 0, 0, 20};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(cs->type(), 5);
}

BOOST_AUTO_TEST_SUITE_END()
