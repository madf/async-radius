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
    std::vector<uint8_t> expected(d);
    expected.insert(expected.begin(), 6);
    expected.insert(expected.begin(), 1);

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
    std::vector<uint8_t> expected(d);
    expected.insert(expected.begin(), 6);
    expected.insert(expected.begin(), 5);

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
    std::vector<uint8_t> expected(d);
    expected.insert(expected.begin(), 6);
    expected.insert(expected.begin(), 4);

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
    std::vector<uint8_t> expected(d);
    expected.insert(expected.begin(), 0x12);
    expected.insert(expected.begin(), 0x02);

    BOOST_TEST(values == expected, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(s.type(), 2);
}

BOOST_AUTO_TEST_CASE(EncryptedDataConstructor1)
{
    std::array<uint8_t, 16> auth1 {0x40, 0x66, 0x42, 0xfc, 0x3e, 0xa4, 0x30, 0x4a, 0x42, 0x39, 0xdb, 0xb1, 0xf9, 0x8a, 0x09, 0x40};
    std::vector<uint8_t> d1 {0xf9, 0xe4, 0x4a, 0x32, 0x8a, 0xee, 0x19, 0x48, 0x64, 0x50, 0x70, 0x31, 0xf0, 0x92, 0x06, 0x05};
    RadProto::Encrypted s1(2, d1.data(), d1.size(), "secret", auth1);

    BOOST_CHECK_EQUAL(s1.toString(), "123456789876543");

    std::vector<uint8_t> values1 = s1.toVector("secret", auth1);
    std::vector<uint8_t> expected1(d1);
    expected1.insert(expected1.begin(), 0x12);
    expected1.insert(expected1.begin(), 0x02);

    BOOST_TEST(values1 == expected1, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(s1.type(), 2);
}

BOOST_AUTO_TEST_CASE(EncryptedDataConstructor2)
{
    std::array<uint8_t, 16> auth2 {0x9c, 0x8e, 0xef, 0xce, 0x5f, 0x18, 0xc7, 0x30, 0x04, 0xc3, 0x34, 0xee, 0x80, 0xfd, 0xf3, 0x98};
    std::vector<uint8_t> d2 {0x6a, 0xd8, 0x72, 0x2f, 0x87, 0x8a, 0xd5, 0x79, 0x0c, 0x30, 0xc3, 0xf6, 0x41, 0x70, 0xd6, 0x82};
    RadProto::Encrypted s2(2, d2.data(), d2.size(), "secret", auth2);

    BOOST_CHECK_EQUAL(s2.toString(), "1234567898765432");

    std::vector<uint8_t> values2 = s2.toVector("secret", auth2);
    std::vector<uint8_t> expected2(d2);
    expected2.insert(expected2.begin(), 0x12);
    expected2.insert(expected2.begin(), 0x02);

    BOOST_TEST(values2 == expected2, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(s2.type(), 2);
}

BOOST_AUTO_TEST_CASE(EncryptedDataConstructor3)
{
    std::array<uint8_t, 16> auth3 {0xf0, 0xcb, 0x8b, 0x5b, 0xd2, 0xd8, 0x96, 0x0b, 0xf7, 0x80, 0x68, 0x89, 0x27, 0x6a, 0xa4, 0xdc};
    std::vector<uint8_t> d3 {0x52, 0x8a, 0x79, 0x24, 0xf4, 0xa9, 0xc9, 0x04, 0x2b, 0x4a, 0xfe, 0x2f, 0x10, 0xd8, 0xa0, 0xcd, 0x51, 0x99, 0xd3, 0xfd, 0xfb, 0xb0, 0xdc, 0x97, 0x6a, 0x19, 0xd6, 0xcc, 0x17, 0xfb, 0xff, 0x3b};
    RadProto::Encrypted s3(2, d3.data(), d3.size(), "secret", auth3);

    BOOST_CHECK_EQUAL(s3.toString(), "12345678987654321");

    std::vector<uint8_t> values3 = s3.toVector("secret", auth3);
    std::vector<uint8_t> expected3(d3);
    expected3.insert(expected3.begin(), 0x22);
    expected3.insert(expected3.begin(), 0x02);

    BOOST_TEST(values3 == expected3, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(s3.type(), 2);
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

BOOST_AUTO_TEST_CASE(EncryptedValueConstructor1)
{
    RadProto::Encrypted v1(2, "123456789876543");

    BOOST_CHECK_EQUAL(v1.toString(), "123456789876543");

    std::array<uint8_t, 16> auth1 { 0x40, 0x66, 0x42, 0xfc, 0x3e, 0xa4, 0x30, 0x4a, 0x42, 0x39, 0xdb, 0xb1, 0xf9, 0x8a, 0x09, 0x40};
    std::vector<uint8_t> values1 = v1.toVector("secret", auth1);
    std::vector<uint8_t> expected1 {0x02, 0x12, 0xf9, 0xe4, 0x4a, 0x32, 0x8a, 0xee, 0x19, 0x48, 0x64, 0x50, 0x70, 0x31, 0xf0, 0x92, 0x06, 0x05};

    BOOST_TEST(values1 == expected1, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(v1.type(), 2);
}

BOOST_AUTO_TEST_CASE(EncryptedValueConstructor2)
{
    RadProto::Encrypted v2(2, "1234567898765432");

    BOOST_CHECK_EQUAL(v2.toString(), "1234567898765432");

    std::array<uint8_t, 16> auth2 {0x9c, 0x8e, 0xef, 0xce, 0x5f, 0x18, 0xc7, 0x30, 0x04, 0xc3, 0x34, 0xee, 0x80, 0xfd, 0xf3, 0x98};
    std::vector<uint8_t> values2 = v2.toVector("secret", auth2);
    std::vector<uint8_t> expected2 {0x02, 0x12, 0x6a, 0xd8, 0x72, 0x2f, 0x87, 0x8a, 0xd5, 0x79, 0x0c, 0x30, 0xc3, 0xf6, 0x41, 0x70, 0xd6, 0x82};

    BOOST_TEST(values2 == expected2, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(v2.type(), 2);
}

BOOST_AUTO_TEST_CASE(EncryptedValueConstructor3)
{
    RadProto::Encrypted v3(2, "12345678987654321");

    BOOST_CHECK_EQUAL(v3.toString(), "12345678987654321");

    std::array<uint8_t, 16> auth3 {0xf0, 0xcb, 0x8b, 0x5b, 0xd2, 0xd8, 0x96, 0x0b, 0xf7, 0x80, 0x68, 0x89, 0x27, 0x6a, 0xa4, 0xdc};
    std::vector<uint8_t> values3 = v3.toVector("secret", auth3);
    std::vector<uint8_t> expected3 {0x02, 0x22, 0x52, 0x8a, 0x79, 0x24, 0xf4, 0xa9, 0xc9, 0x04, 0x2b, 0x4a, 0xfe, 0x2f, 0x10, 0xd8, 0xa0, 0xcd, 0x51, 0x99, 0xd3, 0xfd, 0xfb, 0xb0, 0xdc, 0x97, 0x6a, 0x19, 0xd6, 0xcc, 0x17, 0xfb, 0xff, 0x3b};

    BOOST_TEST(values3 == expected3, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(v3.type(), 2);
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

BOOST_AUTO_TEST_CASE(EncryptedClone1)
{
    RadProto::Encrypted c1(2, "123456789876543");

    std::unique_ptr<RadProto::Attribute> cs1(c1.clone());

    BOOST_CHECK_EQUAL(cs1->toString(), "123456789876543");

    std::array<uint8_t, 16> auth1 {0x40, 0x66, 0x42, 0xfc, 0x3e, 0xa4, 0x30, 0x4a, 0x42, 0x39, 0xdb, 0xb1, 0xf9, 0x8a, 0x09, 0x40};
    std::vector<uint8_t> values1 = cs1->toVector("secret", auth1);
    std::vector<uint8_t> expected1 {0x02, 0x12, 0xf9, 0xe4, 0x4a, 0x32, 0x8a, 0xee, 0x19, 0x48, 0x64, 0x50, 0x70, 0x31, 0xf0, 0x92, 0x06, 0x05};

    BOOST_TEST(values1 == expected1, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(cs1->type(), 2);
}

BOOST_AUTO_TEST_CASE(EncryptedClone2)
{
    RadProto::Encrypted c2(2, "1234567898765432");

    std::unique_ptr<RadProto::Attribute> cs2(c2.clone());

    BOOST_CHECK_EQUAL(cs2->toString(), "1234567898765432");

    std::array<uint8_t, 16> auth2 {0x9c, 0x8e, 0xef, 0xce, 0x5f, 0x18, 0xc7, 0x30, 0x04, 0xc3, 0x34, 0xee, 0x80, 0xfd, 0xf3, 0x98};
    std::vector<uint8_t> values2 = cs2->toVector("secret", auth2);
    std::vector<uint8_t> expected2 {0x02, 0x12, 0x6a, 0xd8, 0x72, 0x2f, 0x87, 0x8a, 0xd5, 0x79, 0x0c, 0x30, 0xc3, 0xf6, 0x41, 0x70, 0xd6, 0x82};

    BOOST_TEST(values2 == expected2, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(cs2->type(), 2);
}

BOOST_AUTO_TEST_CASE(EncryptedClone3)
{
    RadProto::Encrypted c3(2, "12345678987654321");

    std::unique_ptr<RadProto::Attribute> cs3(c3.clone());

    BOOST_CHECK_EQUAL(cs3->toString(), "12345678987654321");

    std::array<uint8_t, 16> auth3 {0xf0, 0xcb, 0x8b, 0x5b, 0xd2, 0xd8, 0x96, 0x0b, 0xf7, 0x80, 0x68, 0x89, 0x27, 0x6a, 0xa4, 0xdc};
    std::vector<uint8_t> values3 = cs3->toVector("secret", auth3);
    std::vector<uint8_t> expected3 {0x02, 0x22, 0x52, 0x8a, 0x79, 0x24, 0xf4, 0xa9, 0xc9, 0x04, 0x2b, 0x4a, 0xfe, 0x2f, 0x10, 0xd8, 0xa0, 0xcd, 0x51, 0x99, 0xd3, 0xfd, 0xfb, 0xb0, 0xdc, 0x97, 0x6a, 0x19, 0xd6, 0xcc, 0x17, 0xfb, 0xff, 0x3b};

    BOOST_TEST(values3 == expected3, boost::test_tools::per_element());

    BOOST_CHECK_EQUAL(cs3->type(), 2);
}

BOOST_AUTO_TEST_SUITE_END()
