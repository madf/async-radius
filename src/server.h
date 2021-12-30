#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <array>
#include <cstdint> //uint8_t, uint32_t

class Server
{
    public:
        Server(boost::asio::io_service& io_service);

    private:
        void start_receive();
        void handle_receive(const boost::system::error_code& error, std::size_t);
        void handle_send(const boost::system::error_code&, std::size_t);

        boost::asio::ip::udp::socket socket_;
        boost::asio::ip::udp::endpoint remote_endpoint_;
        std::array<uint8_t, 4096> recv_buffer_;
        std::array<uint8_t, 4096> send_buffer_;
};

#endif
