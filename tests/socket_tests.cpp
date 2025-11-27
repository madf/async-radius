#define BOOST_TEST_MODULE RadProtoDictionariesTests

#include "radproto/socket.h"
#include "radproto/error.h"
#include "radproto/attribute.h"
#include "radproto/vendor_attribute.h"
#include "attribute_codes.h"
#include "utils.h"
#include <boost/asio.hpp>
#include <array>
#include <vector>
#include <set>
#include <string>
#include <cstdint> //uint8_t, uint32_t
#include <stdexcept>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wparentheses"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

using boost::system::error_code;

namespace
{
    bool callbackReceiveCalled = false;
    bool callbackSendCalled = false;

    void checkSend(const error_code& ec)
    {
        callbackSendCalled = true;
        BOOST_REQUIRE(!ec);
    }

    void checkReceive(const error_code& ec, const std::optional<RadProto::Packet>& p, boost::asio::ip::udp::endpoint /*source*/)
    {
        callbackReceiveCalled = true;
        BOOST_REQUIRE(!ec);

        BOOST_REQUIRE(p != std::nullopt);

        RadProto::Packet d = p.value();
        BOOST_CHECK_EQUAL(d.code(), 1);

        BOOST_CHECK_EQUAL(d.id(), 208);

        std::array<uint8_t, 16> auth {0x1a, 0x40, 0x43, 0xc6, 0x41, 0x0a, 0x08, 0x31, 0x12, 0x16, 0x80, 0x2c, 0x3e, 0x83, 0x12, 0x45};

        BOOST_TEST(d.auth() == auth, boost::test_tools::per_element());

        const auto& attrs = d.attributes();

        auto* attr0 = findAttribute(attrs, RadProto::USER_NAME);

        BOOST_REQUIRE(attr0 != nullptr);
        BOOST_CHECK_EQUAL(attr0->code(), RadProto::USER_NAME);
        BOOST_CHECK_EQUAL(attr0->toString(), "test");

        auto* attr1 = findAttribute(attrs, RadProto::USER_PASSWORD);

        BOOST_REQUIRE(attr1 != nullptr);
        BOOST_CHECK_EQUAL(attr1->code(), RadProto::USER_PASSWORD);
        BOOST_CHECK_EQUAL(attr1->toString(), "123456");

        auto* attr2 = findAttribute(attrs, RadProto::NAS_IP_ADDRESS);

        BOOST_REQUIRE(attr2 != nullptr);
        BOOST_CHECK_EQUAL(attr2->code(), RadProto::NAS_IP_ADDRESS);
        BOOST_CHECK_EQUAL(attr2->toString(), "127.0.0.1");

        auto* attr3 = findAttribute(attrs, RadProto::NAS_PORT);

        BOOST_REQUIRE(attr3 != nullptr);
        BOOST_CHECK_EQUAL(attr3->code(), RadProto::NAS_PORT);
        BOOST_CHECK_EQUAL(attr3->toString(), "1");

        auto* attr4 = findAttribute(attrs, RadProto::MESSAGE_AUTHENTICATOR);

        BOOST_REQUIRE(attr4 != nullptr);
        BOOST_CHECK_EQUAL(attr4->code(), RadProto::MESSAGE_AUTHENTICATOR);
        BOOST_CHECK_EQUAL(attr4->toString(), "F3E000E77DEB51EB815D52373D06B71B");

        auto* attr5 = findAttribute(attrs, RadProto::FRAMED_PROTOCOL);

        BOOST_REQUIRE(attr5 != nullptr);
        BOOST_CHECK_EQUAL(attr5->code(), RadProto::FRAMED_PROTOCOL);
        BOOST_CHECK_EQUAL(attr5->toString(), "1");

        std::vector<RadProto::VendorSpecific> vendor = d.vendorSpecific();

        BOOST_REQUIRE_EQUAL(vendor.size(), 1);
        BOOST_CHECK_EQUAL(vendor[0].vendorId(), 171);
        BOOST_CHECK_EQUAL(vendor[0].vendorAttributeCode(), 1);
        BOOST_CHECK_EQUAL(vendor[0].toString(), "00000003");

        std::set<uint8_t> codes {RadProto::USER_NAME, RadProto::USER_PASSWORD, RadProto::NAS_IP_ADDRESS, RadProto::NAS_PORT, RadProto::MESSAGE_AUTHENTICATOR, RadProto::FRAMED_PROTOCOL};

        BOOST_REQUIRE_EQUAL(attrs.size(), 6);
        BOOST_CHECK(codes.count(attrs[0]->code()) == 1);
        BOOST_CHECK(codes.count(attrs[1]->code()) == 1);
        BOOST_CHECK(codes.count(attrs[2]->code()) == 1);
        BOOST_CHECK(codes.count(attrs[3]->code()) == 1);
        BOOST_CHECK(codes.count(attrs[4]->code()) == 1);
        BOOST_CHECK(codes.count(attrs[5]->code()) == 1);
    }
}

BOOST_AUTO_TEST_SUITE(SocketTests)

BOOST_AUTO_TEST_CASE(TestConstructor)
{
    boost::asio::io_context io_context;

    BOOST_CHECK_NO_THROW(RadProto::Socket s(io_context, "secret", 3000));

}

BOOST_AUTO_TEST_CASE(TestAsyncSend)
{
    std::array<uint8_t, 16> auth {0x1a, 0x40, 0x43, 0xc6, 0x41, 0x0a, 0x08, 0x31, 0x12, 0x16, 0x80, 0x2c, 0x3e, 0x83, 0x12, 0x45};

    const std::vector<RadProto::Attribute*> attributes {new RadProto::String(1, "test"), new RadProto::Encrypted(2, "123456"), new RadProto::IpAddress(4, {127, 0, 0, 1}), new RadProto::Integer(5, 1), new RadProto::Bytes(80, {0xf3, 0xe0, 0x00, 0xe7, 0x7d, 0xeb, 0x51, 0xeb, 0x81, 0x5d, 0x52, 0x37, 0x3d, 0x06, 0xb7, 0x1b}), new RadProto::Integer(7, 1)};

    std::vector<RadProto::VendorSpecific> vendorSpecific;
    vendorSpecific.push_back(RadProto::VendorSpecific(171, 1, {0, 0, 0, 3}));

    RadProto::Packet p(1, 208, auth, attributes, vendorSpecific);

    boost::asio::io_context io_context;

    boost::asio::ip::udp::endpoint destination(boost::asio::ip::make_address_v4("127.0.0.1"), 3000);

    RadProto::Socket s(io_context, "secret", 3000);

    s.asyncSend(p, destination, checkSend);

    s.asyncReceive(checkReceive);

    io_context.run();

    BOOST_CHECK_MESSAGE(callbackSendCalled, "Function asyncSend hasn't called checkSend.");
    BOOST_CHECK_MESSAGE(callbackReceiveCalled, "Function asyncReceive hasn't called checkReceive.");
}

BOOST_AUTO_TEST_SUITE_END()
