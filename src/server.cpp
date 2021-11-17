#include "server.h"
#include <functional> //std::bind
#include <string>
#include <iostream>

using boost::asio::ip::udp;
using boost::system::error_code;

namespace pls = std::placeholders;

Server::Server(boost::asio::io_service& io_service)
      : socket_(io_service, udp::endpoint(udp::v4(), 9999))
{
    start_receive();
}

void Server::start_receive()
{
    socket_.async_receive_from(boost::asio::buffer(recv_buffer_), remote_endpoint_,
        std::bind(&Server::handle_receive, this, pls::_1, pls::_2));
}

void Server::handle_receive(const error_code& error, std::size_t /*bytes_transferred*/)
{
    if (!error || error == boost::asio::error::message_size)
    {
        const std::string message("Hello, World!");

        socket_.async_send_to(boost::asio::buffer(message), remote_endpoint_,
          std::bind(&Server::handle_send, this, message,pls::_1, pls::_2));

        start_receive();
    }
}

void Server::handle_send(std::string /*message*/, const error_code& /*error*/, std::size_t /*bytes_transferred*/)
{
}
