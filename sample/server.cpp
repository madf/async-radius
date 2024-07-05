#include "server.h"
#include "packet_codes.h"
#include <functional>
#include <iostream>

using boost::system::error_code;

Server::Server(boost::asio::io_service& io_service, const std::string& secret, uint16_t port, const std::string& filePath)
    : m_radius(io_service, secret, port),
      m_dictionaries(filePath)
{
    startReceive();
}

void Server::startReceive()
{
    m_radius.asyncReceive([this](const auto& error, const auto& packet){ handleReceive(error, packet); });
}

RadProto::Packet Server::makeResponse(const RadProto::Packet& request)
{
    std::vector<RadProto::Attribute*> attributes;
    attributes.push_back(new RadProto::String(m_dictionaries.attributeCode("User-Name"), "test"));
    attributes.push_back(new RadProto::Integer(m_dictionaries.attributeCode("NAS-Port"), 20));
    std::array<uint8_t, 4> address {127, 104, 22, 17};
    attributes.push_back(new RadProto::IpAddress(m_dictionaries.attributeCode("NAS-IP-Address"), address));
    std::vector<uint8_t> bytes {'1', '2', '3', 'a', 'b', 'c'};
    attributes.push_back(new RadProto::Bytes(m_dictionaries.attributeCode("Callback-Number"), bytes));
    std::vector<uint8_t> chapPassword {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g' };
    attributes.push_back(new RadProto::ChapPassword(m_dictionaries.attributeCode("CHAP-Password"), 1, chapPassword));

    std::vector<RadProto::VendorSpecific> vendorSpecific;
    std::vector<uint8_t> vendorValue {0, 0, 0, 3};
    vendorSpecific.push_back(RadProto::VendorSpecific(m_dictionaries.vendorCode("Dlink"), m_dictionaries.vendorAttributeCode("Dlink", "Dlink-User-Level"), vendorValue));

    if (request.type() == RadProto::ACCESS_REQUEST)
        return RadProto::Packet(RadProto::ACCESS_ACCEPT, request.id(), request.auth(), attributes, vendorSpecific);

    return RadProto::Packet(RadProto::ACCESS_REJECT, request.id(), request.auth(), attributes, vendorSpecific);
}

void Server::handleSend(const error_code& ec)
{
    if (ec)
        std::cout << "Error asyncSend: " << ec.message() << "\n";

    startReceive();
}

void Server::handleReceive(const error_code& error, const std::optional<RadProto::Packet>& packet)
{
    if (error)
    {
        std::cout << "Error asyncReceive: " << error.message() << "\n";
        return;
    }

    if (packet == std::nullopt)
    {
        std::cout << "Error asyncReceive: the request packet is missing\n";
        return;
    }
    else
    {
        m_radius.asyncSend(makeResponse(*packet), [this](const auto& ec){ handleSend(ec); });
    }
}

