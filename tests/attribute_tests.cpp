#define BOOST_TEST_MODULE RadProtoAttributeTests

#include "radproto/attribute.h"
#include <memory>
#include <array>
#include <vector>
#include <string>
#include <cstdint> //uint8_t, uint32_t

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
    std::vector<uint8_t> d {10, 20, 30, 40};
    RadProto::Integer s(5, d.data(), d.size());

    BOOST_CHECK_EQUAL(s.toString(), "169090600");

    std::vector<uint8_t> values = s.toVector({}, {});
    std::vector<uint8_t> expected {5, 6, 10, 20, 30, 40};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(s.type(), 5);
}

BOOST_AUTO_TEST_CASE(IntegerValueConstructor)
{
    RadProto::Integer v(5, 169090600);

    BOOST_CHECK_EQUAL(v.toString(), "169090600");

    std::vector<uint8_t> values = v.toVector({}, {});
    std::vector<uint8_t> expected {5, 6, 10, 20, 30, 40};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(v.type(), 5);
}

BOOST_AUTO_TEST_CASE(IntegerClone)
{
    RadProto::Integer c(5, 169090600);
    std::unique_ptr<RadProto::Attribute> cs(c.clone());

    BOOST_CHECK_EQUAL(cs->toString(), "169090600");

    std::vector<uint8_t> values = cs->toVector({}, {});
    std::vector<uint8_t> expected {5, 6, 10, 20, 30, 40};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(cs->type(), 5);
}

BOOST_AUTO_TEST_CASE(IpAddressDataConstructor)
{
    std::vector<uint8_t> d {127, 104, 22, 17};
    RadProto::IpAddress s(4, d.data(), d.size());

    BOOST_CHECK_EQUAL(s.toString(), "127.104.22.17");

    std::vector<uint8_t> values = s.toVector({}, {});
    std::vector<uint8_t> expected {4, 6, 127, 104, 22, 17};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(s.type(), 4);
}

BOOST_AUTO_TEST_CASE(IpAddressValueConstructor)
{
    std::array<uint8_t, 4> address {127, 104, 22, 17};
    RadProto::IpAddress v(4, address);

    BOOST_CHECK_EQUAL(v.toString(), "127.104.22.17");

    std::vector<uint8_t> values = v.toVector({}, {});
    std::vector<uint8_t> expected {4, 6, 127, 104, 22, 17};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(v.type(), 4);
}

BOOST_AUTO_TEST_CASE(IpAddressClone)
{
    std::array<uint8_t, 4> address {127, 104, 22, 17};
    RadProto::IpAddress c(4, address);
    std::unique_ptr<RadProto::Attribute> cs(c.clone());

    BOOST_CHECK_EQUAL(cs->toString(), "127.104.22.17");

    std::vector<uint8_t> values = cs->toVector({}, {});
    std::vector<uint8_t> expected {4, 6, 127, 104, 22, 17};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(cs->type(), 4);
}

BOOST_AUTO_TEST_CASE(EncryptedDataConstructor)
{
    std::array<uint8_t, 16> auth {0x92, 0xfa, 0xa1, 0xed, 0x98, 0x9b, 0xb4, 0x79, 0xfe, 0x20, 0xe2, 0xf4, 0x7f, 0x4a, 0x5a, 0x70};
    std::vector<uint8_t> d {0x25, 0x38, 0x58, 0x18, 0xae, 0x97, 0xeb, 0xeb, 0xbd, 0x46, 0xfd, 0xb9, 0xd1, 0x17, 0x84, 0xeb};
    RadProto::Encrypted s(2, d.data(), d.size(), "secret", auth);

    BOOST_CHECK_EQUAL(s.toString(), "123456");

    std::vector<uint8_t> values = s.toVector("secret", auth);
    std::vector<uint8_t> expected {0x02, 0x12, 0x25, 0x38, 0x58, 0x18, 0xae, 0x97, 0xeb, 0xeb, 0xbd, 0x46, 0xfd, 0xb9, 0xd1, 0x17, 0x84, 0xeb};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(s.type(), 2);
}

BOOST_AUTO_TEST_CASE(EncryptedValueConstructor)
{
    RadProto::Encrypted v(2, "123456");

    BOOST_CHECK_EQUAL(v.toString(), "123456");

    std::array<uint8_t, 16> auth {0x92, 0xfa, 0xa1, 0xed, 0x98, 0x9b, 0xb4, 0x79, 0xfe, 0x20, 0xe2, 0xf4, 0x7f, 0x4a, 0x5a, 0x70};
    std::vector<uint8_t> values = v.toVector("secret", auth);
    std::vector<uint8_t> expected {0x02, 0x12, 0x25, 0x38, 0x58, 0x18, 0xae, 0x97, 0xeb, 0xeb, 0xbd, 0x46, 0xfd, 0xb9, 0xd1, 0x17, 0x84, 0xeb};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(v.type(), 2);
}

BOOST_AUTO_TEST_CASE(EncryptedClone)
{
    RadProto::Encrypted c(2, "123456");
    std::unique_ptr<RadProto::Attribute> cs(c.clone());

    BOOST_CHECK_EQUAL(cs->toString(), "123456");

    std::array<uint8_t, 16> auth {0x92, 0xfa, 0xa1, 0xed, 0x98, 0x9b, 0xb4, 0x79, 0xfe, 0x20, 0xe2, 0xf4, 0x7f, 0x4a, 0x5a, 0x70};
    std::vector<uint8_t> values = cs->toVector("secret", auth);
    std::vector<uint8_t> expected {0x02, 0x12, 0x25, 0x38, 0x58, 0x18, 0xae, 0x97, 0xeb, 0xeb, 0xbd, 0x46, 0xfd, 0xb9, 0xd1, 0x17, 0x84, 0xeb};

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(cs->type(), 2);
}

BOOST_AUTO_TEST_SUITE_END()
