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
    if (!error || error == boost::asio::error::message_size)
    {
        size_t length;
        try
        {
            length = recv_buffer_.at(2) * 256 + recv_buffer_.at(3);
        }
        catch (std::out_of_range const& exc)
        {
            std::cout << exc.what() << "\n";
        }

        if (bytes >= length)
        {
            uint8_t send_code;

            if (recv_buffer_[0] == ACCESS_REQUEST)
                send_code = ACCESS_ACCEPT;

            std::array<uint8_t, 4096> send_buffer;
            uint8_t send_length1 = 0;
            uint8_t send_length2 = 20;

            send_buffer[0] = send_code;
            send_buffer[1] = recv_buffer_[1];
            send_buffer[2] = send_length1;
            send_buffer[3] = send_length2;

            for (int i = 4; i <= 19; i++)
                send_buffer[i] = recv_buffer_[i];

            std::string secr("secret");

            for (int i = 0; i <= 5; i++)
                send_buffer[i + 20] = secr[i];

            std::array<uint8_t, 16> md;

            MD5(send_buffer.data(), send_length2 + secr.length(), md.data());

            for (int i = 0; i <= 15; i++)
                send_buffer[i + 4] = md[i];

          socket_.async_send_to(boost::asio::buffer(send_buffer), remote_endpoint_,
              std::bind(&Server::handle_send, this, pls::_1, pls::_2));

            start_receive();
        }
    }
}

void Server::handle_send(const error_code& /*error*/, std::size_t /*bytes_transferred*/)
{
}
