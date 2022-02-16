#include "server.h"
#include "packet.h"
#include "packet_codes.h"
#include "attribute_types.h"
#include <functional> //std::bind
#include <string>
#include <iostream>
#include <openssl/md5.h>

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

    size_t length = m_recvBuffer[2] * 256 + m_recvBuffer[3];
    std::cout << "Length: " << length << "\n";

    if (bytes < length)
    {
        std::cout << "Error: request length " << bytes << "is less than specified in the request - " << length << "\n";
        return;
    }

    Packet packet(m_recvBuffer);

    m_socket.async_send_to(boost::asio::buffer(packet.makeSendBuffer(), 20), m_remoteEndpoint,
        std::bind(&Server::handleSend, this, pls::_1, pls::_2));

    startReceive();
}

void Server::handleSend(const error_code& /*error*/, std::size_t /*bytes_transferred*/)
{
}
