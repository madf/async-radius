#define BOOST_TEST_MODULE RadProtoDictionariesTests

#include "radproto/socket.h"
#include "radproto/error.h"
#include "radproto/attribute.h"
#include "radproto/vendor_attribute.h"
#include "attribute_types.h"
#include "utils.h"
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
    void checkSend(const error_code& ec)
    {
        BOOST_CHECK(true);

        if (ec)
        {
            BOOST_CHECK_EQUAL(ec.category().name(), "radproto");
            BOOST_CHECK_EQUAL(ec.message(), "No error");
        }
    }

    void checkReceive(const error_code& ec, const std::optional<RadProto::Packet>& p)
    {
        BOOST_CHECK_EQUAL(ec.category().name(), "radproto");
        BOOST_CHECK_EQUAL(ec.message(), "No error");

        BOOST_REQUIRE(p != std::nullopt);

        RadProto::Packet d = p.value();
        BOOST_CHECK_EQUAL(d.type(), 2);

        BOOST_CHECK_EQUAL(d.id(), 208);

        std::array<uint8_t, 16> auth {0x1a, 0x40, 0x43, 0xc6, 0x41, 0x0a, 0x08, 0x31, 0x12, 0x16, 0x80, 0x2c, 0x3e, 0x83, 0x12, 0x45};

        BOOST_TEST(d.auth() == auth, boost::test_tools::per_element());

        const std::vector<RadProto::Attribute*> attributes {new RadProto::String(1, "test"), new RadProto::Integer(5, 20), new RadProto::IpAddress(4, {127, 104, 22, 17}), new RadProto::Bytes(19, {'1', '2', '3', 'a', 'b', 'c'}), new RadProto::ChapPassword(3, 1, {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g' })};

        const auto& attrs = d.attributes();

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

        auto* attr5 = findAttribute(attrs, RadProto::FRAMED_PROTOCOL);

        BOOST_REQUIRE(attr5 != nullptr);
        BOOST_CHECK_EQUAL(attr5->type(), RadProto::FRAMED_PROTOCOL);
        BOOST_CHECK_EQUAL(attr5->toString(), "1");

        std::vector<RadProto::VendorSpecific> vendor = d.vendorSpecific();

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
    }
}

BOOST_AUTO_TEST_SUITE(SocketTests)

BOOST_AUTO_TEST_CASE(TestConstructor)
{
boost::asio::io_service io_service;

BOOST_CHECK_NO_THROW(RadProto::Socket s(io_service, "secret", 3000));

}

BOOST_AUTO_TEST_CASE(TestAsyncSend)
{
    std::array<uint8_t, 16> auth {0x1a, 0x40, 0x43, 0xc6, 0x41, 0x0a, 0x08, 0x31, 0x12, 0x16, 0x80, 0x2c, 0x3e, 0x83, 0x12, 0x45};

    const std::vector<RadProto::Attribute*> attributes {new RadProto::String(1, "test"), new RadProto::Integer(5, 20), new RadProto::IpAddress(4, {127, 104, 22, 17}), new RadProto::Bytes(19, {'1', '2', '3', 'a', 'b', 'c'}), new RadProto::ChapPassword(3, 1, {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g' })};

    std::vector<RadProto::VendorSpecific> vendorSpecific;
    vendorSpecific.push_back(RadProto::VendorSpecific(171, 1, {0, 0, 0, 3}));

    RadProto::Packet p(2, 208, auth, attributes, vendorSpecific);

    boost::asio::io_service io_service;

    RadProto::Socket s(io_service, "secret", 3000);
    s.asyncSend(p, checkSend);

    s.asyncReceive(checkReceive);

    io_service.run();
}

BOOST_AUTO_TEST_SUITE_END()
