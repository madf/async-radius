#include "server.h"
#include "request.h"
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
    start_receive();
}

void Server::start_receive()
{
    m_socket.async_receive_from(boost::asio::buffer(m_recvBuffer), m_remoteEndpoint,
        std::bind(&Server::handle_receive, this, pls::_1, pls::_2));
}

void Server::handle_receive(const error_code& error, std::size_t bytes)
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

    Request request(m_recvBuffer);

    if (request.type() == ACCESS_REQUEST)
    {
         m_sendBuffer[0] = ACCESS_ACCEPT;
         std::cout << "Request type: ACCESS_ACCEPT\n";
    }
    else
    {
        m_sendBuffer[0] = ACCESS_REJECT;
        std::cout << "Request type: ACCESS_REJECT\n";
    }

    m_sendBuffer[1] = request.id();
    std::cout << "Request ID: " << static_cast<size_t>(request.id()) << "\n";

    m_sendBuffer[2] = 0;
    m_sendBuffer[3] = 20;

    for (size_t i = 0; i < 16; ++i)
    {
        m_sendBuffer[i + 4] = request.auth()[i];
        std::cout << "auth[" << i << "]: " << static_cast<size_t>(request.auth()[i])  << "\n";
    }

    std::string secr("secret");

    for (size_t i = 0; i < 6; ++i)
        m_sendBuffer[i + 20] = secr[i];

    std::array<uint8_t, 16> md;

    MD5(m_sendBuffer.data(), 20 + secr.length(), md.data());

    for (size_t i = 0; i < 16; ++i)
        m_sendBuffer[i + 4] = md[i];

    m_socket.async_send_to(boost::asio::buffer(m_sendBuffer, 20), m_remoteEndpoint,
        std::bind(&Server::handle_send, this, pls::_1, pls::_2));

    start_receive();
}

void Server::handle_send(const error_code& /*error*/, std::size_t /*bytes_transferred*/)
{
}
