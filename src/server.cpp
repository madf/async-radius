#include "server.h"
#include "packet_codes.h"
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

void Server::printPacket(const Packet& p)
{
    std::cout << "Packet type: " << packetTypeToString(p.type()) << "\n";

    std::cout << "ID: " << std::to_string(p.id()) << "\n";

    std::cout << "Attributes:\n";
    for (const auto& ap : p.attributes())
        std::cout << "\t" << m_dictionaries.attributes().name(ap->type()) << ": " << ap->toString() << "\n";

    for (const auto& ap : p.vendorSpecific())
    {
        const std::string vendorName = m_dictionaries.vendorNames().name(ap->vendorId());
        std::cout << "\t" << ap->name() << ": " << vendorName << "\n";
        std::cout << "\t" << m_dictionaries.vendorAttributes().name(vendorName, ap->vendorType()) << ": " << ap->toString() << "\n";
    }
}

Server::Server(boost::asio::io_service& io_service, const std::string& secret)
      : m_socket(io_service, udp::endpoint(udp::v4(), 9999)),
        m_secret(secret),
        m_dictionaries("/usr/share/freeradius/dictionary")
{
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
        Packet packet = makeResponse(Packet(m_recvBuffer, bytes, m_secret, m_dictionaries));

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
    attributes.push_back(new String(m_dictionaries.attributes().code("User-Name"), "test"));
    attributes.push_back(new Integer(m_dictionaries.attributes().code("NAS-Port"), 20));
    std::array<uint8_t, 4> address {127, 104, 22, 17};
    attributes.push_back(new IpAddress(m_dictionaries.attributes().code("NAS-IP-Address"), address));
    attributes.push_back(new Encrypted(m_dictionaries.attributes().code("User-Password"), "password123"));
    std::vector<uint8_t> bytes {'1', '2', '3', 'a', 'b', 'c'};
    attributes.push_back(new Bytes(m_dictionaries.attributes().code("Callback-Number"), bytes));
    std::vector<uint8_t> chapPassword {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g' };
    attributes.push_back(new ChapPassword(m_dictionaries.attributes().code("CHAP-Password"), 1, chapPassword));

    std::vector<VendorSpecific*> vendorSpecific;
    std::vector<uint8_t> vendorValue {'0', '0', '0', '3'};
    vendorSpecific.push_back(new VendorSpecific(m_dictionaries.vendorNames().code("Dlink"), m_dictionaries.vendorAttributes().code("Dlink", "Dlink-User-Level"), vendorValue));

    if (request.type() == ACCESS_REQUEST)
        return Packet(ACCESS_ACCEPT, request.id(), request.auth(), attributes, vendorSpecific);

    return Packet(ACCESS_REJECT, request.id(), request.auth(), attributes, vendorSpecific);
}
