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

        m_socket.async_send_to(boost::asio::buffer(packet.makeSendBuffer("secret"), 20),
            m_remoteEndpoint, std::bind(&Server::handleSend, this, pls::_1, pls::_2));

        startReceive();
    }
    catch (std::runtime_error& exception)
    {
        std::cout << "Runtime error: " << exception.what() << "\n";
        return;
    }
}

void Server::handleSend(const error_code& /*error*/, std::size_t /*bytes_transferred*/)
{
}

const Packet Server::makeResponse(const Packet& packet)
{
    uint8_t type = packet.type();

    if (type == ACCESS_REQUEST)
    {
        type = ACCESS_ACCEPT;
        std::cout << "Packet type: ACCESS_ACCEPT\n";
    }
    else
    {
        type = ACCESS_REJECT;
        std::cout << "Packet type: ACCESS_REJECT\n";
    }

    uint8_t id = packet.id();

    std::array<uint8_t, 16> auth = packet.auth();

    return Packet(type, id, auth);
}

