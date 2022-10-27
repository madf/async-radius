#ifndef SERVER_H
#define SERVER_H

#include "packet.h"
#include <boost/asio.hpp>
#include <array>
#include <cstdint> //uint8_t, uint32_t

class Server
{
    public:
        Server(boost::asio::io_service& io_service, const std::string& secret);

    private:
        void startReceive(const std::string& secret);
        void handleReceive(const std::string& secret, const boost::system::error_code& error, std::size_t bytes);
        void handleSend(const boost::system::error_code& error, std::size_t bytes_transferred);
        Packet makeResponse(const Packet& request);

        boost::asio::ip::udp::socket m_socket;
        boost::asio::ip::udp::endpoint m_remoteEndpoint;
        std::array<uint8_t, 4096> m_recvBuffer;
};

#endif
