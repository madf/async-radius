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
        void handle_receive(const boost::system::error_code& error, std::size_t bytes);
        void handle_send(const boost::system::error_code& error, std::size_t bytes_transferred);

        boost::asio::ip::udp::socket m_socket;
        boost::asio::ip::udp::endpoint m_remoteEndpoint;
        std::array<uint8_t, 4096> m_recvBuffer;
};

#endif
