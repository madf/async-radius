#define BOOST_TEST_MODULE RadProtoPacketTests

#include "radproto/packet.h"
#include "radproto/attribute.h"
#include "radproto/vendor_attribute.h"
#include "attribute_types.h"
#include "radproto/error.h"
#include <memory>
#include <array>
#include <vector>
#include <set>
#include <string>
#include <cstdint> //uint8_t, uint32_t

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wparentheses"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

namespace
{
    RadProto::Attribute* findAttribute(const std::vector<RadProto::Attribute*>& attributes, RadProto::Attribute_Types type)
    {
        for (const auto& b : attributes)
        {
            if (b->type() == type)
                return b;
        }
        return nullptr;
    }
}

BOOST_AUTO_TEST_SUITE(PacketTests)

BOOST_AUTO_TEST_CASE(PacketBufferConstructorRequest)
{
    std::vector<uint8_t> d {0x01, 0xd0, 0x00, 0x5c, 0x1a, 0x40, 0x43, 0xc6, 0x41, 0x0a, 0x08, 0x31, 0x12, 0x16, 0x80, 0x2c, 0x3e, 0x83, 0x12, 0x45, 0x01, 0x06, 0x74, 0x65, 0x73, 0x74, 0x02, 0x12, 0x8c, 0x06, 0xc8, 0x23, 0x55, 0xba, 0x0d, 0xd6, 0x15, 0x1c, 0xbf, 0x9d, 0xd8, 0x1a, 0x4d, 0x87, 0x04, 0x06, 0x7f, 0x00, 0x00, 0x01, 0x05, 0x06, 0x00, 0x00, 0x00, 0x01, 0x50, 0x12, 0xf3, 0xe0, 0x00, 0xe7, 0x7d, 0xeb, 0x51, 0xeb, 0x81, 0x5d, 0x52, 0x37, 0x3d, 0x06, 0xb7, 0x1b, 0x07, 0x06, 0x00, 0x00, 0x00, 0x01, 0x1a, 0x0c, 0x00, 0x00, 0x00, 0xab, 0x01, 0x06, 0x00, 0x00, 0x00, 0x03};

    RadProto::Packet p(d.data(), d.size(), "secret");

    BOOST_CHECK_EQUAL(p.type(), 1);

    BOOST_CHECK_EQUAL(p.id(), 0xd0);

    std::array<uint8_t, 16> authExpected {0x1a, 0x40, 0x43, 0xc6, 0x41, 0x0a, 0x08, 0x31, 0x12, 0x16, 0x80, 0x2c, 0x3e, 0x83, 0x12, 0x45};

    BOOST_TEST(p.auth() == authExpected, boost::test_tools::per_element());

    const auto& attrs = p.attributes();
    auto* attr0 = findAttribute(attrs, RadProto::USER_NAME);

    BOOST_REQUIRE(attr0 != nullptr);
    BOOST_CHECK_EQUAL(attr0->type(), RadProto::USER_NAME);
    BOOST_CHECK_EQUAL(attr0->toString(), "test");

    auto* attr1 = findAttribute(attrs, RadProto::USER_PASSWORD);

    BOOST_REQUIRE(attr1 != nullptr);
    BOOST_CHECK_EQUAL(attr1->type(), RadProto::USER_PASSWORD);
    BOOST_CHECK_EQUAL(attr1->toString(), "123456");

    auto* attr2 = findAttribute(attrs, RadProto::NAS_IP_ADDRESS);

    BOOST_REQUIRE(attr2 != nullptr);
    BOOST_CHECK_EQUAL(attr2->type(), RadProto::NAS_IP_ADDRESS);
    BOOST_CHECK_EQUAL(attr2->toString(), "127.0.0.1");

    auto* attr3 = findAttribute(attrs, RadProto::NAS_PORT);

    BOOST_REQUIRE(attr3 != nullptr);
    BOOST_CHECK_EQUAL(attr3->type(), RadProto::NAS_PORT);
    BOOST_CHECK_EQUAL(attr3->toString(), "1");

    auto* attr4 = findAttribute(attrs, RadProto::MESSAGE_AUTHENTICATOR);

    BOOST_REQUIRE(attr4 != nullptr);
    BOOST_CHECK_EQUAL(attr4->type(), RadProto::MESSAGE_AUTHENTICATOR);
    BOOST_CHECK_EQUAL(attr4->toString(), "F3E000E77DEB51EB815D52373D06B71B");

    auto* attr5 = findAttribute(attrs, RadProto::FRAMED_PROTOCOL);

    BOOST_REQUIRE(attr5 != nullptr);
    BOOST_CHECK_EQUAL(attr5->type(), RadProto::FRAMED_PROTOCOL);
    BOOST_CHECK_EQUAL(attr5->toString(), "1");

    std::vector<RadProto::VendorSpecific> vendor = p.vendorSpecific();

    BOOST_REQUIRE_EQUAL(vendor.size(), 1);
    BOOST_CHECK_EQUAL(vendor[0].vendorId(), 171);
    BOOST_CHECK_EQUAL(vendor[0].vendorType(), 1);
    BOOST_CHECK_EQUAL(vendor[0].toString(), "00000003");

    std::set<uint8_t> types {RadProto::USER_NAME, RadProto::USER_PASSWORD, RadProto::NAS_IP_ADDRESS, RadProto::NAS_PORT, RadProto::MESSAGE_AUTHENTICATOR, RadProto::FRAMED_PROTOCOL};

    BOOST_REQUIRE_EQUAL(attrs.size(), 6);
    BOOST_CHECK(types.count(attrs[0]->type()) == 1);
    BOOST_CHECK(types.count(attrs[1]->type()) == 1);
    BOOST_CHECK(types.count(attrs[2]->type()) == 1);
    BOOST_CHECK(types.count(attrs[3]->type()) == 1);
    BOOST_CHECK(types.count(attrs[4]->type()) == 1);
    BOOST_CHECK(types.count(attrs[5]->type()) == 1);

    std::vector<uint8_t> values = p.makeSendBuffer("secret");

    BOOST_REQUIRE_EQUAL(values.size(), 92);

    BOOST_REQUIRE_EQUAL(d.size(), 92);

    BOOST_TEST(values == d, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(PacketBufferConstructorResponse)
{
    std::vector<uint8_t> d {0x02, 0xd0, 0x00, 0x4d, 0x93, 0xa9, 0x61, 0x8b, 0x2f, 0x4c, 0x5a, 0x51, 0x65, 0x67, 0x3d, 0xb4, 0x07, 0x30, 0xa2, 0x39, 0x01, 0x06, 0x74, 0x65, 0x73, 0x74, 0x05, 0x06, 0x00, 0x00, 0x00, 0x14, 0x04, 0x06, 0x7f, 0x68, 0x16, 0x11, 0x13, 0x08, 0x31, 0x32, 0x33, 0x61, 0x62, 0x63, 0x03, 0x13, 0x01, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x1a, 0x0c, 0x00, 0x00, 0x00, 0xab, 0x01, 0x06, 0x00, 0x00, 0x00, 0x03};

    RadProto::Packet p(d.data(), d.size(), "secret");

    BOOST_CHECK_EQUAL(p.type(), 2);

    BOOST_CHECK_EQUAL(p.id(), 0xd0);

    std::array<uint8_t, 16> authExpected {0x93, 0xa9, 0x61, 0x8b, 0x2f, 0x4c, 0x5a, 0x51, 0x65, 0x67, 0x3d, 0xb4, 0x07, 0x30, 0xa2, 0x39};

    BOOST_TEST(p.auth() == authExpected, boost::test_tools::per_element());

    const auto& attrs = p.attributes();

    auto* attr0 = findAttribute(attrs, RadProto::USER_NAME);

    BOOST_REQUIRE(attr0 != nullptr);
    BOOST_CHECK_EQUAL(attr0->type(), RadProto::USER_NAME);
    BOOST_CHECK_EQUAL(attr0->toString(), "test");

    auto* attr1 = findAttribute(attrs, RadProto::NAS_PORT);

    BOOST_REQUIRE(attr1 != nullptr);
    BOOST_CHECK_EQUAL(attr1->type(), RadProto::NAS_PORT);
    BOOST_CHECK_EQUAL(attr1->toString(), "20");

    auto* attr2 = findAttribute(attrs, RadProto::NAS_IP_ADDRESS);

    BOOST_REQUIRE(attr2 != nullptr);
    BOOST_CHECK_EQUAL(attr2->type(), RadProto::NAS_IP_ADDRESS);
    BOOST_CHECK_EQUAL(attr2->toString(), "127.104.22.17");

    auto* attr3 = findAttribute(attrs, RadProto::CALLBACK_NUMBER);

    BOOST_REQUIRE(attr3 != nullptr);
    BOOST_CHECK_EQUAL(attr3->type(), RadProto::CALLBACK_NUMBER);
    BOOST_CHECK_EQUAL(attr3->toString(), "313233616263");

    auto* attr4 = findAttribute(attrs, RadProto::CHAP_PASSWORD);

    BOOST_REQUIRE(attr4 != nullptr);
    BOOST_CHECK_EQUAL(attr4->type(), RadProto::CHAP_PASSWORD);
    BOOST_CHECK_EQUAL(attr4->toString(), "1 31323334353637383961626364656667");

    std::vector<RadProto::VendorSpecific> vendor = p.vendorSpecific();

    BOOST_REQUIRE_EQUAL(vendor.size(), 1);
    BOOST_CHECK_EQUAL(vendor[0].vendorId(), 171);
    BOOST_CHECK_EQUAL(vendor[0].vendorType(), 1);
    BOOST_CHECK_EQUAL(vendor[0].toString(), "00000003");

    std::set<uint8_t> types {RadProto::USER_NAME, RadProto::NAS_PORT, RadProto::NAS_IP_ADDRESS, RadProto::CALLBACK_NUMBER, RadProto::CHAP_PASSWORD};

    BOOST_REQUIRE_EQUAL(attrs.size(), 5);
    BOOST_CHECK(types.count(attrs[0]->type()) == 1);
    BOOST_CHECK(types.count(attrs[1]->type()) == 1);
    BOOST_CHECK(types.count(attrs[2]->type()) == 1);
    BOOST_CHECK(types.count(attrs[3]->type()) == 1);
    BOOST_CHECK(types.count(attrs[4]->type()) == 1);

    std::vector<uint8_t> values = p.makeSendBuffer("secret");

    BOOST_REQUIRE_EQUAL(values.size(), 77);

    BOOST_REQUIRE_EQUAL(d.size(), 77);

    BOOST_TEST(values == d, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(PacketDataConstructorThrowSizeLess20)
{
    std::vector<uint8_t> d {0x02, 0xd0, 0x00, 0x4d, 0x93, 0xa9, 0x61, 0x8b, 0x2f, 0x4c, 0x5a, 0x51, 0x65, 0x67, 0x3d, 0xb4, 0x07, 0x30, 0xa2};

    BOOST_CHECK_THROW(RadProto::Packet p(d.data(), d.size(), "secret"), RadProto::Exception);
}

BOOST_AUTO_TEST_CASE(PacketDataConstructorThrowSizeLessLength)
{
    std::vector<uint8_t> d {0x02, 0xd0, 0x00, 0x4d, 0x93, 0xa9, 0x61, 0x8b, 0x2f, 0x4c, 0x5a, 0x51, 0x65, 0x67, 0x3d, 0xb4, 0x07, 0x30, 0xa2, 0x39, 0x01, 0x06, 0x74, 0x65, 0x73, 0x74, 0x05, 0x06, 0x00, 0x00, 0x00, 0x14, 0x04, 0x06, 0x7f, 0x68, 0x16, 0x11, 0x13, 0x08, 0x31, 0x32, 0x33, 0x61, 0x62, 0x63, 0x03, 0x13, 0x01, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x1a, 0x0c, 0x00, 0x00, 0x00, 0xab, 0x01, 0x06, 0x00, 0x00, 0x00};

    BOOST_CHECK_THROW(RadProto::Packet p(d.data(), d.size(), "secret"), RadProto::Exception);
}

BOOST_AUTO_TEST_CASE(PacketDataConstructorThrowMakeAttributeInvalidType)
{
    std::vector<uint8_t> d {0x02, 0xd0, 0x00, 0x4d, 0x93, 0xa9, 0x61, 0x8b, 0x2f, 0x4c, 0x5a, 0x51, 0x65, 0x67, 0x3d, 0xb4, 0x07, 0x30, 0xa2, 0x39, 0x17, 0x06, 0x74, 0x65, 0x73, 0x74, 0x05, 0x06, 0x00, 0x00, 0x00, 0x14, 0x04, 0x06, 0x7f, 0x68, 0x16, 0x11, 0x13, 0x08, 0x31, 0x32, 0x33, 0x61, 0x62, 0x63, 0x03, 0x13, 0x01, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x1a, 0x0c, 0x00, 0x00, 0x00, 0xab, 0x01, 0x06, 0x00, 0x00, 0x00, 0x03};

    BOOST_CHECK_THROW(RadProto::Packet p(d.data(), d.size(), "secret"), RadProto::Exception);
}

BOOST_AUTO_TEST_CASE(PacketValueConstructorResponse)
{
    std::vector<uint8_t> d {0x02, 0xd0, 0x00, 0x4d, 0x93, 0xa9, 0x61, 0x8b, 0x2f, 0x4c, 0x5a, 0x51, 0x65, 0x67, 0x3d, 0xb4, 0x07, 0x30, 0xa2, 0x39, 0x01, 0x06, 0x74, 0x65, 0x73, 0x74, 0x05, 0x06, 0x00, 0x00, 0x00, 0x14, 0x04, 0x06, 0x7f, 0x68, 0x16, 0x11, 0x13, 0x08, 0x31, 0x32, 0x33, 0x61, 0x62, 0x63, 0x03, 0x13, 0x01, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x1a, 0x0c, 0x00, 0x00, 0x00, 0xab, 0x01, 0x06, 0x00, 0x00, 0x00, 0x03};

    std::array<uint8_t, 16> auth {0x1a, 0x40, 0x43, 0xc6, 0x41, 0x0a, 0x08, 0x31, 0x12, 0x16, 0x80, 0x2c, 0x3e, 0x83, 0x12, 0x45};

    const std::vector<RadProto::Attribute*> attributes {new RadProto::String(1, "test"), new RadProto::Integer(5, 20), new RadProto::IpAddress(4, {127, 104, 22, 17}), new RadProto::Bytes(19, {'1', '2', '3', 'a', 'b', 'c'}), new RadProto::ChapPassword(3, 1, {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g' })};

    std::vector<RadProto::VendorSpecific> vendorSpecific;
    vendorSpecific.push_back(RadProto::VendorSpecific(171, 1, {0, 0, 0, 3}));

    RadProto::Packet p(2, 208, auth, attributes, vendorSpecific);

    BOOST_CHECK_EQUAL(p.type(), 2);

    BOOST_CHECK_EQUAL(p.id(), 208);

    BOOST_TEST(p.auth() == auth, boost::test_tools::per_element());

    const auto& attrs = p.attributes();

    auto* attr0 = findAttribute(attrs, RadProto::USER_NAME);

    BOOST_REQUIRE(attr0 != nullptr);
    BOOST_CHECK_EQUAL(attr0->type(), RadProto::USER_NAME);
    BOOST_CHECK_EQUAL(attr0->toString(), "test");

    auto* attr1 = findAttribute(attrs, RadProto::NAS_PORT);

    BOOST_REQUIRE(attr1 != nullptr);
    BOOST_CHECK_EQUAL(attr1->type(), RadProto::NAS_PORT);
    BOOST_CHECK_EQUAL(attr1->toString(), "20");

    auto* attr2 = findAttribute(attrs, RadProto::NAS_IP_ADDRESS);

    BOOST_REQUIRE(attr2 != nullptr);
    BOOST_CHECK_EQUAL(attr2->type(), RadProto::NAS_IP_ADDRESS);
    BOOST_CHECK_EQUAL(attr2->toString(), "127.104.22.17");

    auto* attr3 = findAttribute(attrs, RadProto::CALLBACK_NUMBER);

    BOOST_REQUIRE(attr3 != nullptr);
    BOOST_CHECK_EQUAL(attr3->type(), RadProto::CALLBACK_NUMBER);
    BOOST_CHECK_EQUAL(attr3->toString(), "313233616263");

    auto* attr4 = findAttribute(attrs, RadProto::CHAP_PASSWORD);

    BOOST_REQUIRE(attr4 != nullptr);
    BOOST_CHECK_EQUAL(attr4->type(), RadProto::CHAP_PASSWORD);
    BOOST_CHECK_EQUAL(attr4->toString(), "1 31323334353637383961626364656667");

    std::vector<RadProto::VendorSpecific> vendor = p.vendorSpecific();

    BOOST_REQUIRE_EQUAL(vendor.size(), 1);
    BOOST_CHECK_EQUAL(vendor[0].vendorId(), 171);
    BOOST_CHECK_EQUAL(vendor[0].vendorType(), 1);
    BOOST_CHECK_EQUAL(vendor[0].toString(), "00000003");

    std::set<uint8_t> types {RadProto::USER_NAME, RadProto::NAS_PORT, RadProto::NAS_IP_ADDRESS, RadProto::CALLBACK_NUMBER, RadProto::CHAP_PASSWORD};

    BOOST_REQUIRE_EQUAL(attrs.size(), 5);
    BOOST_CHECK(types.count(attrs[0]->type()) == 1);
    BOOST_CHECK(types.count(attrs[1]->type()) == 1);
    BOOST_CHECK(types.count(attrs[2]->type()) == 1);
    BOOST_CHECK(types.count(attrs[3]->type()) == 1);
    BOOST_CHECK(types.count(attrs[4]->type()) == 1);

    std::vector<uint8_t> values = p.makeSendBuffer("secret");

    BOOST_REQUIRE_EQUAL(values.size(), d.size());

    BOOST_TEST(values == d, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(PacketValueConstructorRequest)
{
    std::vector<uint8_t> d {0x01, 0xd0, 0x00, 0x5c, 0x1a, 0x40, 0x43, 0xc6, 0x41, 0x0a, 0x08, 0x31, 0x12, 0x16, 0x80, 0x2c, 0x3e, 0x83, 0x12, 0x45, 0x01, 0x06, 0x74, 0x65, 0x73, 0x74, 0x02, 0x12, 0x8c, 0x06, 0xc8, 0x23, 0x55, 0xba, 0x0d, 0xd6, 0x15, 0x1c, 0xbf, 0x9d, 0xd8, 0x1a, 0x4d, 0x87, 0x04, 0x06, 0x7f, 0x00, 0x00, 0x01, 0x05, 0x06, 0x00, 0x00, 0x00, 0x01, 0x50, 0x12, 0xf3, 0xe0, 0x00, 0xe7, 0x7d, 0xeb, 0x51, 0xeb, 0x81, 0x5d, 0x52, 0x37, 0x3d, 0x06, 0xb7, 0x1b, 0x07, 0x06, 0x00, 0x00, 0x00, 0x01, 0x1a, 0x0c, 0x00, 0x00, 0x00, 0xab, 0x01, 0x06, 0x00, 0x00, 0x00, 0x03};

    std::array<uint8_t, 16> auth {0x1a, 0x40, 0x43, 0xc6, 0x41, 0x0a, 0x08, 0x31, 0x12, 0x16, 0x80, 0x2c, 0x3e, 0x83, 0x12, 0x45};

    const std::vector<RadProto::Attribute*> attributes {new RadProto::String(1, "test"), new RadProto::Encrypted(2, "123456"), new RadProto::IpAddress(4, {127, 0, 0, 1}), new RadProto::Integer(5, 1), new RadProto::Bytes(80, {0xf3, 0xe0, 0x00, 0xe7, 0x7d, 0xeb, 0x51, 0xeb, 0x81, 0x5d, 0x52, 0x37, 0x3d, 0x06, 0xb7, 0x1b}), new RadProto::Integer(7, 1)};

    std::vector<RadProto::VendorSpecific> vendorSpecific;
    vendorSpecific.push_back(RadProto::VendorSpecific(171, 1, {0, 0, 0, 3}));

    RadProto::Packet p(1, 208, auth, attributes, vendorSpecific);

    BOOST_CHECK_EQUAL(p.type(), 1);

    BOOST_CHECK_EQUAL(p.id(), 208);

    BOOST_TEST(p.auth() == auth, boost::test_tools::per_element());

    const auto& attrs = p.attributes();

    auto* attr0 = findAttribute(attrs, RadProto::USER_NAME);

    BOOST_REQUIRE(attr0 != nullptr);
    BOOST_CHECK_EQUAL(attr0->type(), RadProto::USER_NAME);
    BOOST_CHECK_EQUAL(attr0->toString(), "test");

    auto* attr1 = findAttribute(attrs, RadProto::USER_PASSWORD);

    BOOST_REQUIRE(attr1 != nullptr);
    BOOST_CHECK_EQUAL(attr1->type(), RadProto::USER_PASSWORD);
    BOOST_CHECK_EQUAL(attr1->toString(), "123456");

    auto* attr2 = findAttribute(attrs, RadProto::NAS_IP_ADDRESS);

    BOOST_REQUIRE(attr2 != nullptr);
    BOOST_CHECK_EQUAL(attr2->type(), RadProto::NAS_IP_ADDRESS);
    BOOST_CHECK_EQUAL(attr2->toString(), "127.0.0.1");

    auto* attr3 = findAttribute(attrs, RadProto::NAS_PORT);

    BOOST_REQUIRE(attr3 != nullptr);
    BOOST_CHECK_EQUAL(attr3->type(), RadProto::NAS_PORT);
    BOOST_CHECK_EQUAL(attr3->toString(), "1");

    auto* attr4 = findAttribute(attrs, RadProto::MESSAGE_AUTHENTICATOR);

    BOOST_REQUIRE(attr4 != nullptr);
    BOOST_CHECK_EQUAL(attr4->type(), RadProto::MESSAGE_AUTHENTICATOR);
    BOOST_CHECK_EQUAL(attr4->toString(), "F3E000E77DEB51EB815D52373D06B71B");

    auto* attr5 = findAttribute(attrs, RadProto::FRAMED_PROTOCOL);

    BOOST_REQUIRE(attr5 != nullptr);
    BOOST_CHECK_EQUAL(attr5->type(), RadProto::FRAMED_PROTOCOL);
    BOOST_CHECK_EQUAL(attr5->toString(), "1");

    std::vector<RadProto::VendorSpecific> vendor = p.vendorSpecific();

    BOOST_REQUIRE_EQUAL(vendor.size(), 1);
    BOOST_CHECK_EQUAL(vendor[0].vendorId(), 171);
    BOOST_CHECK_EQUAL(vendor[0].vendorType(), 1);
    BOOST_CHECK_EQUAL(vendor[0].toString(), "00000003");

    std::set<uint8_t> types {RadProto::USER_NAME, RadProto::USER_PASSWORD, RadProto::NAS_IP_ADDRESS, RadProto::NAS_PORT, RadProto::MESSAGE_AUTHENTICATOR, RadProto::FRAMED_PROTOCOL};

    BOOST_REQUIRE_EQUAL(attrs.size(), 6);
    BOOST_CHECK(types.count(attrs[0]->type()) == 1);
    BOOST_CHECK(types.count(attrs[1]->type()) == 1);
    BOOST_CHECK(types.count(attrs[2]->type()) == 1);
    BOOST_CHECK(types.count(attrs[3]->type()) == 1);
    BOOST_CHECK(types.count(attrs[4]->type()) == 1);
    BOOST_CHECK(types.count(attrs[5]->type()) == 1);

    std::vector<uint8_t> values = p.makeSendBuffer("secret");

    BOOST_REQUIRE_EQUAL(values.size(), d.size());

    BOOST_TEST(values == d, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_SUITE_END()
