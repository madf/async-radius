#include "server.h"
#include "packet_codes.h"
#include "attribute_types.h"
#include "vendor_attribute.h"
#include <functional> //std::bind
#include <iostream>

using boost::asio::ip::udp;
using boost::system::error_code;

namespace pls = std::placeholders;

std::string packetTypeToString(int type)
{
    switch (type)
    {
        case ACCESS_REQUEST: return "ACCESS_REQUEST";
        case ACCESS_ACCEPT: return "ACCESS_ACCEPT";
        case ACCESS_REJECT: return "ACCESS_REJECT";
        case ACCOUNTING_REQUEST: return "ACCOUNTING_REQUEST";
        case ACCOUNTING_RESPONSE: return "ACCOUNTING_RESPONSE";
        case ACCESS_CHALLENGE: return "ACCESS_CHALLENGE";
        case STATUS_SERVER: return "STATUS_SERVER";
        case STATUS_CLIENT: return "STATUS_CLIENT";
    }
    return "uncnown";
}

void printPacket(const Packet& p)
{
    std::cout << "Packet type: " << packetTypeToString(p.type()) << "\n";

    std::cout << "ID: " << std::to_string(p.id()) << "\n";

    std::cout << "Attributes:\n";
    for (const auto& ap : p.attributes())
        std::cout << "\t" << ap->name() << ": " << ap->toString() << "\n";

    for (const auto& ap : p.vendorSpecific())
    {
        std::cout << "\t" << ap->name() << ": " << ap->vendorId() << "\n";
        std::cout << "\t" << std::to_string(ap->vendorType()) << ": " << ap->toString() << "\n";
    }
}

Server::Server(boost::asio::io_service& io_service, const std::string& secret)
      : m_socket(io_service, udp::endpoint(udp::v4(), 9999)),
        m_secret(secret),
        m_dictionaries("/usr/share/freeradius/dictionary")
{
/*    for (const auto &entry: m_dictionaries.attributes().reverseDict())
        std::cout << std::to_string(entry.second) << ": " << entry.first << "\n";*/

    for (const auto &entry: m_dictionaries.attributes().rightDict())
        std::cout << entry.second << ": " << std::to_string(entry.first) << "\n";

    for (const auto &entry: m_dictionaries.attributeValues().rightDict())
        std::cout << "  " << entry.first.first << " - " << entry.second << ": " << std::to_string(entry.first.second) << "\n";

    for (const auto &entry: m_dictionaries.vendorNames().rightDict())
        std::cout << entry.second << ": " << std::to_string(entry.first) << "\n";

    for (const auto &entry: m_dictionaries.vendorAttributes().rightDict())
        std::cout << "  " << entry.second << ": " << std::to_string(entry.first.second) << "\n";

    for (const auto &entry: m_dictionaries.vendorAttributeValues().rightDict())
        std::cout << "    " << entry.first.first << " - " << entry.second << ": " << std::to_string(entry.first.second) << "\n";

    startReceive();
}

void Server::startReceive()
{
    m_socket.async_receive_from(boost::asio::buffer(m_recvBuffer), m_remoteEndpoint,
        std::bind(&Server::handleReceive, this, pls::_1, pls::_2));
}

void Server::handleReceive(const error_code& error, std::size_t bytes)
{
    if (error)
    {
        std::cout << "Error async_receivw_from: " << error.message() << "\n";
        return;
    }

    if (bytes < 20)
    {
        std::cout << "Error: request length less than 20 bytes\n";
        return;
    }

    try
    {
        Packet packet = makeResponse(Packet(m_recvBuffer, bytes, m_secret));

        std::cout << "Response packet\n";
        printPacket(packet);

        m_socket.async_send_to(boost::asio::buffer(packet.makeSendBuffer(m_secret)),
            m_remoteEndpoint, std::bind(&Server::handleSend, this, pls::_1, pls::_2));
    }
    catch (const std::runtime_error& exception)
    {
        std::cout << "Packet error: " << exception.what() << "\n";
    }
    startReceive();
}

void Server::handleSend(const error_code& /*error*/, std::size_t /*bytes_transferred*/)
{
}

Packet Server::makeResponse(const Packet& request)
{
    std::cout << "Request packet\n";
    printPacket(request);

    std::vector<Attribute*> attributes;
    attributes.push_back(new String(USER_NAME, "test"));
    attributes.push_back(new Integer(NAS_PORT, 20));
    std::array<uint8_t, 4> address {127, 104, 22, 17};
    attributes.push_back(new IpAddress(NAS_IP_ADDRESS, address));
    attributes.push_back(new Encrypted(USER_PASSWORD, "password123"));
    std::vector<uint8_t> bytes {'1', '2', '3', 'a', 'b', 'c'};
    attributes.push_back(new Bytes(CALLBACK_NUMBER, bytes));
    std::vector<uint8_t> chapPassword {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g' };
    attributes.push_back(new ChapPassword(CHAP_PASSWORD, 1, chapPassword));

    std::vector<VendorSpecific*> vendorSpecific;
    std::vector<uint8_t> vendorValue {'0', '0', '0', '3'};
    vendorSpecific.push_back(new VendorSpecific(171, 1, vendorValue));

    if (request.type() == ACCESS_REQUEST)
        return Packet(ACCESS_ACCEPT, request.id(), request.auth(), attributes, vendorSpecific);

    return Packet(ACCESS_REJECT, request.id(), request.auth(), attributes, vendorSpecific);
}
