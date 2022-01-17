#include "server.h"
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
      : socket_(io_service, udp::endpoint(udp::v4(), 9999))
{
    start_receive();
}

void Server::start_receive()
{
    socket_.async_receive_from(boost::asio::buffer(recv_buffer_), remote_endpoint_,
        std::bind(&Server::handle_receive, this, pls::_1, pls::_2));
}

void Server::handle_receive(const error_code& error, std::size_t bytes)
{
    if (!error && bytes >= 20)
    {
        size_t length = recv_buffer_[2] * 256 + recv_buffer_[3];

        if (bytes >= length)
        {
            uint8_t send_code;

            if (recv_buffer_[0] == ACCESS_REQUEST)
                send_code = ACCESS_ACCEPT;

            send_buffer_[0] = send_code;
            send_buffer_[1] = recv_buffer_[1];
            send_buffer_[2] = 0;
            send_buffer_[3] = 20;

            for (int i = 4; i <= 19; i++)
                send_buffer_[i] = recv_buffer_[i];

            std::string secr("secret");

            for (int i = 0; i <= 5; i++)
                send_buffer_[i + 20] = secr[i];

            std::array<uint8_t, 16> md;

            MD5(send_buffer_.data(), 20 + secr.length(), md.data());

            for (int i = 0; i <= 15; i++)
                send_buffer_[i + 4] = md[i];

            socket_.async_send_to(boost::asio::buffer(send_buffer_), remote_endpoint_,
                std::bind(&Server::handle_send, this, pls::_1, pls::_2));

            start_receive();
        }
        else
            std::cout << "Error: request length less than specified length/n";
    }
    else if (error)
        std::cout << "Error async_receivw_from: " << error.message() << "/n";
    else
        std::cout << "Error: request length less than 20 bytes/n";
}

void Server::handle_send(const error_code& /*error*/, std::size_t /*bytes_transferred*/)
{
}
