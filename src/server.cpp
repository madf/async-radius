#include "server.h"
#include "packet_codes.h"
#include "attribute_types.h"
#include <functional> //std::bind
#include <iostream>

using boost::asio::ip::udp;
using boost::system::error_code;

namespace pls = std::placeholders;

Server::Server(boost::asio::io_service& io_service)
      : m_socket(io_service, udp::endpoint(udp::v4(), 9999))
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
        Packet packet = makeResponse(Packet(m_recvBuffer, bytes));

        m_socket.async_send_to(boost::asio::buffer(packet.makeSendBuffer("secret")),
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

std::string typeToString(int type)
{
    switch (type)
    {
        case 1: return "USER_NAME";
        case 2: return "USER_PASSWORD";
        case 3: return "CHAP_PASSWORD";
        case 4: return "NAS_IP_ADDRESS";
        case 5: return "NAS_PORT";
        case 6: return "SERVICE_TYPE";
        case 7: return "FRAMED_PROTOCOL";
        case 8: return "FRAMED_IP_ADDRESS";
        case 9: return "FRAMED_IP_NETMASK";
        case 10: return "FRAMED_ROUTING";
        case 11: return "FILTER_ID";
        case 12: return "FRAMED_MTU";
        case 13: return "FRAMED_COMPRESSION";
        case 14: return "LOGIN_IP_HOST";
        case 15: return "LOGIN_SERVICE";
        case 16: return "LOGIN_TCP_PORT";
        case 17: return "REPLY_MESSAGE";
        case 19: return "CALLBACK_NUMBER";
        case 20: return "CALLBACK_ID";
        case 22: return "FRAMED_ROUTE";
        case 23: return "FRAMED_IPX_NETWORK";
        case 24: return "STATE";
        case 25: return "CLASS";
        case 26: return "VENDOR_SPECIFIC";
        case 27: return "SESSION_TIMEOUT";
        case 28: return "IDLE_TIMEOUT";
        case 29: return "TERMINATION_ACTION";
        case 30: return "CALLED_STATION_ID";
        case 31: return "CALLING_STATION_ID";
        case 32: return "NAS_IDENTIFIER";
        case 33: return "PROXY_STATE";
        case 34: return "LOGIN_LAT_SERVICE";
        case 35: return "LOGIN_LAT_NODE";
        case 36: return "LOGIN_LAT_GROUP";
        case 37: return "FRAMED_APPLETALK_LINK";
        case 38: return "FRAMED_APPLETALK_NETWORK";
        case 39: return "FRAMED_APPLETALK_ZONE";
        case 60: return "CHAP_CHALLENGE";
        case 61: return "NAS_PORT_TYPE";
        case 62: return "PORT_LIMIT";
        case 63: return "LOGIN_LAT_PORT";
    }
    return "unknown";
}

Packet Server::makeResponse(const Packet& request)
{
    std::vector<Attribute*> attributes = request.attributes();
    for (size_t i = 0; i < attributes.size(); ++i)
    {
        std::cout << "Attribute type: " << typeToString(attributes[i]->type()) << "\n";
        std::cout << "Attribute value: " << attributes[i]->value() << "\n";
    }

    if (request.type() == ACCESS_REQUEST)
    {
        std::cout << "Packet type: ACCESS_ACCEPT\n";
        return Packet(ACCESS_ACCEPT, request.id(), request.auth());
    }
    std::cout << "Packet type: ACCESS_REJECT\n";
    return Packet(ACCESS_REJECT, request.id(), request.auth());
}
