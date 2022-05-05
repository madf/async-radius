#include "server.h"
#include "packet_codes.h"
#include <functional> //std::bind
#include <iostream>

using boost::asio::ip::udp;
using boost::system::error_code;

namespace pls = std::placeholders;

void printPacket(const Packet& p)
{
    switch (p.type())
    {
        case ACCESS_ACCEPT:
            std::cout << "Packet type: ACCESS_ACCEPT\n";
            break;
        case ACCESS_REJECT:
            std::cout << "Packet type: ACCESS_REJECT\n";
            break;
        case ACCOUNTING_REQUEST:
            std::cout << "Packet type: ACCOUNTING_REQUEST\n";
            break;
        case ACCOUNTING_RESPONSE:
            std::cout << "Packet type: ACCOUNTING_RESPONSE\n";
            break;
        case ACCESS_CHALLENGE:
            std::cout << "Packet type: ACCESS_CHALLENGE\n";
            break;
        case STATUS_SERVER:
            std::cout << "Packet type: STATUS_SERVER\n";
            break;
        case STATUS_CLIENT:
            std::cout << "Packet type: STATUS_CLIENT\n";
            break;
        default:
            std::cout << "Packet type: uncnown\n";
            break;
    }

    std::cout << "ID: " << p.id() << "\n";

    std::cout << "Attributes:\n";
    for (const auto& ap : p.attributes())
        std::cout << "\t" << ap->name() << ": " << ap->value() << "\n";
}

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

        std::cout << "Response packet\n";
        printPacket(packet);

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

Packet Server::makeResponse(const Packet& request)
{
    std::cout << "Request packet\n";
    printPacket(request);

    if (request.type() == ACCESS_REQUEST)
        return Packet(ACCESS_ACCEPT, request.id(), request.auth());

    return Packet(ACCESS_REJECT, request.id(), request.auth());
}
